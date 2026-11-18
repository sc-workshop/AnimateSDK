#include "ResourcePublisher.h"

#include "animate/generator/TimelineBuilder/FrameBuilder.h"
#include "core/parallel/bs_thread_pool.hpp"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

namespace Animate::Publisher {

    void ResourcePublisher::PublishDocument(FCM::AutoPtr<DOM::IFLADocument> document) {
        // Library item metadata is memoised by host pointer; drop anything from an earlier run.
        SymbolContext::ClearCache();

        // The frame-element interface probes are also memoised by host pointer, and they hold
        // references to host objects. They must be dropped while the document is still open, so
        // this runs on every exit path, including a failed publish.
        struct ElementCacheGuard {
            ~ElementCacheGuard() { FrameBuilder::ClearInterfaceCache(); }
        } elementCacheGuard;

        {
            FCM::Double framerate;
            document->GetFrameRate(framerate);
            document_fps = (uint32_t) std::ceil(framerate);
        }

        // Parsing library items
        FCM::FCMListPtr libraryItems;
        document->GetLibraryItems(libraryItems.m_Ptr);

        // Getting items with linkage names
        std::vector<SymbolContext> symbols;
        GetExportSymbols(libraryItems, symbols);
        m_writer.SetExportedSymbols(symbols);

        // Provisional ids stay monotonic for the whole publishing session. They share a namespace
        // with final ids only by accident, the remap table is the only consumer.
        if (m_provisional == 0)
            m_provisional = m_id;

        std::vector<PreparedSymbol*> roots(symbols.size(), nullptr);
        std::vector<std::exception_ptr> errors(symbols.size());

        // Warm-up: build the first root on the publishing thread. This forces first-use plugin
        // loading and DOM class registration in FCM before any worker thread touches it.
        if (!symbols.empty()) {
            try {
                roots[0] = PrepareRoot(symbols[0]);
            } catch (...) {
                errors[0] = std::current_exception();
            }
        }

        // Extract every remaining root in parallel. One job per root symbol, recursion inside the
        // job stays serial, exactly like the original implementation.
        if (symbols.size() > 1) {
            // Measured on a 12-core machine: 4 workers is the fastest configuration. More
            // workers only add host-mutex queueing - 8 workers was ~30 % slower than 4 - so
            // the default is capped.
            uint32_t threadCount = std::thread::hardware_concurrency();
            threadCount = threadCount > 1 ? threadCount - 1 : 1;
            threadCount = std::min<uint32_t>(threadCount, 4);

            BS::thread_pool pool(threadCount);
            for (size_t i = 1; symbols.size() > i; i++) {
                pool.detach_task([&, i]() {
                    try {
                        roots[i] = PrepareRoot(symbols[i]);
                    } catch (...) {
                        errors[i] = std::current_exception();
                    }
                });
            }

            pool.wait();
        }

        // Commit everything on this thread in the recorded depth-first order.
        for (size_t i = 0; symbols.size() > i; i++) {
            if (errors[i])
                std::rethrow_exception(errors[i]);

            PreparedSymbol* root = roots[i];
            if (!root) {
                throw FCM::FCMPluginException(symbols[i].library_item,
                                              FCM::FCMPluginException::Reason::SYMBOL_EXPORT_FAIL);
            }

            ResourceReference ref(CommitSymbol(*root));
            if (!ref) {
                throw FCM::FCMPluginException(symbols[i].library_item,
                                              FCM::FCMPluginException::Reason::SYMBOL_EXPORT_FAIL);
            }
        }

        // Clear document library cache to avoid collision between different documents
        m_symbolClaims.clear();
    }

    void ResourcePublisher::GetExportSymbols(FCM::FCMListPtr libraryItems, std::vector<SymbolContext>& result) {
        uint32_t itemCount = 0;
        libraryItems->Count(itemCount);

        uint32_t itemIndex = itemCount;
        for (uint32_t i = 0; i < itemCount; i++) {
            FCM::AutoPtr<DOM::ILibraryItem> item = libraryItems[--itemIndex];

            FCM::AutoPtr<DOM::LibraryItem::IFolderItem> folderItem = item;
            if (folderItem) {
                FCM::FCMListPtr childrens;
                folderItem->GetChildren(childrens.m_Ptr);

                GetExportSymbols(childrens, result);
            } else {
                FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem = item;
                if (!symbolItem)
                    continue;

                FCM::AutoPtr<FCM::IFCMDictionary> dict;

                FCM::Result status = item->GetProperties(dict.m_Ptr);
                if (FCM_FAILURE_CODE(status) || dict == nullptr)
                    continue;

                FCM::U_Int32 valueLen = 0;
                FCM::FCMDictRecTypeID type;
                status = dict->GetInfo(kLibProp_LinkageClass_DictKey, type, valueLen);
                if (FCM_FAILURE_CODE(status) || type != FCM::FCMDictRecTypeID::StringRep8 || valueLen == 0)
                    continue;

                result.emplace_back(item);
            }
        }
    }

    void ResourcePublisher::SetIdOffset(uint16_t offset) {
        m_id += offset;
    }

    std::vector<ResourcePublisher::PreparedSymbol*>& ResourcePublisher::Sink() {
        static thread_local std::vector<PreparedSymbol*> stack;
        return stack;
    }

    uint16_t ResourcePublisher::AllocateProvisional() {
        return (uint16_t) m_provisional++;
    }

    void ResourcePublisher::RecordRequest(const PreparedRequest& request) {
        std::vector<PreparedSymbol*>& sink = Sink();
        if (sink.empty())
            return;

        sink.back()->requests.push_back(request);
    }

    ResourcePublisher::PreparedSymbol* ResourcePublisher::PrepareRoot(SymbolContext& symbol) {
        FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> item = symbol.library_item;
        if (!item) {
            throw FCM::FCMPluginException(symbol, FCM::FCMPluginException::Reason::UNKNOWN_LIBRARY_ITEM);
        }

        return PrepareSymbol(symbol, item, true);
    }

    ResourcePublisher::PreparedSymbol* ResourcePublisher::PrepareSymbol(SymbolContext& symbol,
                                                                       const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item,
                                                                       bool required) {
        ResourceClaim claim;
        bool owner = false;
        std::shared_ptr<std::promise<PreparedResource*>> promise;

        {
            std::lock_guard<std::mutex> lock(m_prepareMutex);

            auto cached = m_symbolClaims.find(symbol);
            if (cached != m_symbolClaims.end()) {
                claim = cached->second;

                // "Required" is a property of every call site, not of the thread that won the
                // claim. Aggregate it so the static/movieclip choice is order independent.
                claim.resource->required = claim.resource->required || required;
            } else {
                auto prepared = std::make_unique<PreparedSymbol>(symbol);
                prepared->required = required;

                PreparedSymbol* raw = prepared.get();
                m_symbols.push_back(std::move(prepared));

                promise = std::make_shared<std::promise<PreparedResource*>>();
                claim.resource = raw;
                claim.future = promise->get_future().share();
                m_symbolClaims.emplace(symbol, claim);

                owner = true;
            }
        }

        // Another thread owns this symbol, wait for it without holding any lock
        if (!owner) {
            // A thread that owns the host mutex must never block here: the owning thread needs
            // that same mutex to finish the symbol. Fail fast instead of hanging.
            if (FCM::HostLifetimeLock::Depth() != 0) {
                throw std::runtime_error("Waiting on symbol '" + FCM::Locale::ToUtf8(symbol.name) +
                                         "' while holding the host mutex");
            }

            if (claim.future.wait_for(std::chrono::minutes(10)) != std::future_status::ready) {
                throw std::runtime_error("Symbol preparation deadlock: " + FCM::Locale::ToUtf8(symbol.name));
            }

            PreparedResource* existing = claim.future.get();
            PreparedSymbol* prepared = static_cast<PreparedSymbol*>(existing);
            if (!prepared)
                return nullptr;

            // The owner may have built a static shape before this call site (which needs a
            // movieclip) was reached. Rebuild once with the aggregated flag.
            if (required)
                RebuildSymbolIfRequired(*prepared, item);

            return prepared;
        }

        PreparedSymbol* prepared = static_cast<PreparedSymbol*>(claim.resource);

        PreparedResource* result = nullptr;
        try {
            // The first build is always done without "required". That makes it identical for
            // every thread, so call sites that do not require the object always make the same
            // decision, regardless of who won the claim.
            result = BuildSymbol(*prepared, item, false, true);

            bool needsRebuild;
            {
                std::lock_guard<std::mutex> lock(m_prepareMutex);
                needsRebuild = prepared->required;
            }

            // A required reference may have arrived between claim insertion and this point
            if (needsRebuild)
                RebuildSymbolIfRequired(*prepared, item);
        } catch (...) {
            promise->set_exception(std::current_exception());
            throw;
        }

        promise->set_value(result);
        return static_cast<PreparedSymbol*>(result);
    }

    ResourcePublisher::PreparedResource* ResourcePublisher::BuildSymbol(PreparedSymbol& prepared,
                                                                        const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item,
                                                                        bool required,
                                                                        bool initial) {
        Sink().push_back(&prepared);

        // There is deliberately no build-wide gate here. Host calls are serialised by the
        // per-call HostGate, and the whole host (calls *and* object lifetimes) shares one
        // mutex. Holding that mutex for a whole build would deadlock: a build waits for
        // symbols other workers own, and those workers need the same mutex to finish.
        try {
            FCM::AutoPtr<DOM::ITimeline> timeline;
            {
                FCM::PluginModule::HostGate structure_gate(FCM::PluginModule::HostScope::Structure);
                item->GetTimeLine(timeline.m_Ptr);
            }

            wk::Ref<IDisplayObjectWriter> writer = symbolGenerator.Generate(*prepared.context, timeline, required);

            if (!writer) {
                Sink().pop_back();
                {
                    std::lock_guard<std::mutex> lock(prepared.writerMutex);
                    prepared.writer = nullptr;
                    prepared.hash = 0;
                    prepared.has_content = false;
                }
                prepared.generated_required = true;
                return nullptr;
            }

            writer->PreFinalize();

            // Movieclips hash their children ids, which are only final after commit
            std::size_t hash = writer->Type() != WriterType::MovieClip ? writer->HashCode() : 0;
            bool hasContent = writer->HasContent();
            {
                std::lock_guard<std::mutex> lock(prepared.writerMutex);
                prepared.writer = writer;
                prepared.hash = hash;
                if (initial)
                    prepared.has_content = hasContent;
            }

            if (prepared.provisional == ResourceReference::Null)
                prepared.provisional = AllocateProvisional();

            // "required" only affects generation when the static-shape conversion was possible
            prepared.generated_required =
                required || writer->Type() != WriterType::Graphic ||
                !prepared.context->linkage_name.empty() || prepared.context->slicing.IsEnabled();
        } catch (...) {
            Sink().pop_back();
            throw;
        }

        Sink().pop_back();
        return &prepared;
    }

    void ResourcePublisher::RebuildSymbolIfRequired(PreparedSymbol& prepared,
                                                     const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item) {
        std::lock_guard<std::mutex> lock(prepared.generationMutex);

        if (prepared.generated_required)
            return;

        // The first writer is discarded entirely, so its recorded requests must go too.
        // Otherwise the commit would replay a subtree that is no longer part of the file.
        prepared.requests.clear();

        BuildSymbol(prepared, item, true, false);
    }

    ResourceReference ResourcePublisher::AddLibraryItem(SymbolContext& symbol,
                                                        const FCM::AutoPtr<DOM::ILibraryItem>& item,
                                                        bool required) {
        FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbol_item = item;

        if (symbol_item) {
            PreparedSymbol* prepared = PrepareSymbol(symbol, symbol_item, required);
            if (!prepared)
                return ResourceReference();

            // A non-required call site cannot reference a contentless object, because Finalize
            // would refuse to emit it. Evaluated per call site, so the result never depends on
            // which thread prepared the symbol.
            if (!prepared->CanReference(required))
                return ResourceReference();

            RecordRequest(PreparedRequest{PreparedRequest::Kind::Symbol, prepared, nullptr, nullptr});
            return ResourceReference(prepared->provisional);
        }

        FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item = item;
        if (media_item)
            return PrepareMediaSymbol(symbol, media_item, required);

        throw FCM::FCMPluginException(symbol, FCM::FCMPluginException::Reason::UNKNOWN_LIBRARY_ITEM);
    }

    ResourceReference ResourcePublisher::PrepareMediaSymbol(SymbolContext& symbol,
                                                            const FCM::AutoPtr<DOM::LibraryItem::IMediaItem>& media_item,
                                                            bool required) {
        FCM::AutoPtr<FCM::IFCMUnknown> unknownMedia;
        {
            FCM::PluginModule::HostGate library_gate(FCM::PluginModule::HostScope::Library);
            media_item->GetMediaInfo(unknownMedia.m_Ptr);
        }

        FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmap = unknownMedia;
        if (!bitmap)
            return ResourceReference();

        auto resource = std::make_unique<PreparedInline>(symbol);
        resource->required = required;

        wk::Ref<SharedShapeWriter> writer = m_writer.AddShape(*resource->context);
        resource->writer = writer;

        BitmapElement element(*resource->context, media_item, DOM::Utils::MATRIX2D());
        writer->AddGraphic(element);

        PreparedResource* prepared = PrepareResource(std::move(resource), true);
        if (!prepared || !prepared->CanReference(required))
            return ResourceReference();

        RecordRequest(PreparedRequest{PreparedRequest::Kind::Resource, nullptr, prepared, nullptr});
        return ResourceReference(prepared->provisional);
    }

    ResourceReference ResourcePublisher::AddModifier(MaskedLayerState type) {
        PreparedModifier* modifier = nullptr;

        {
            std::lock_guard<std::mutex> lock(m_prepareMutex);

            auto cached = m_modifierCache.find(type);
            if (cached != m_modifierCache.end()) {
                modifier = cached->second;
            } else {
                auto prepared = std::make_unique<PreparedModifier>();
                prepared->type = type;
                prepared->provisional = AllocateProvisional();

                modifier = prepared.get();
                m_modifiers.push_back(std::move(prepared));
                m_modifierCache.emplace(type, modifier);
            }
        }

        RecordRequest(PreparedRequest{PreparedRequest::Kind::Modifier, nullptr, nullptr, modifier});
        return ResourceReference(modifier->provisional);
    }

    ResourceReference ResourcePublisher::AddTextField(SymbolContext& symbol,
                                                      const FCM::AutoPtr<DOM::FrameElement::IClassicText>& textfieldData,
                                                      const std::optional<FCM::FCMListPtr>& filters) {
        auto resource = std::make_unique<PreparedInline>(symbol);

        wk::Ref<SharedTextFieldWriter> writer = m_writer.AddTextField(*resource->context);
        resource->writer = writer;

        TextFieldGenerator::Generate(writer, textfieldData);

        if (filters.has_value()) {
            FCM::PluginModule::HostGate elements_gate(FCM::PluginModule::HostScope::Elements);

            uint32_t filterCount = 0;
            filters.value()->Count(filterCount);

            for (uint32_t i = 0; filterCount > i; i++) {
                FCM::IFCMUnknown* filterUnknown = (*filters.value())[i];
                FCM::AutoPtr<DOM::GraphicFilter::IGlowFilter> glowFilter = filterUnknown;
                FCM::AutoPtr<DOM::GraphicFilter::IDropShadowFilter> shadowFilter = filterUnknown;

                if (glowFilter) {
                    GlowFilter filter;
                    glowFilter->IsEnabled((FCM::Boolean&) filter.enabled);
                    glowFilter->GetInnerShadow((FCM::Boolean&) filter.inner_shadow);
                    glowFilter->GetKnockout((FCM::Boolean&) filter.knockout);
                    glowFilter->GetStrength(filter.strength);
                    glowFilter->GetQuality(filter.quality);
                    glowFilter->GetBlurX(filter.blurX);
                    glowFilter->GetBlurY(filter.blurY);
                    glowFilter->GetShadowColor(filter.color);
                    glowFilter->GetStrength(filter.strength);

                    writer->SetGlowFilter(filter);
                } else if (shadowFilter) {
                    DropShadowFilter filter;
                    shadowFilter->IsEnabled((FCM::Boolean&) filter.enabled);
                    shadowFilter->GetBlurX(filter.blurX);
                    shadowFilter->GetBlurY(filter.blurY);
                    shadowFilter->GetStrength(filter.strength);
                    shadowFilter->GetShadowColor(filter.color);
                    shadowFilter->GetQuality(filter.quality);

                    shadowFilter->GetAngle(filter.angle);
                    shadowFilter->GetDistance(filter.distance);
                    shadowFilter->GetInnerShadow((FCM::Boolean&) filter.inner_shadow);
                    shadowFilter->GetKnockout((FCM::Boolean&) filter.knockout);

                    writer->SetDropShadowFilter(filter);
                }
            }
        }

        PreparedResource* prepared = PrepareResource(std::move(resource), true);
        if (!prepared || !prepared->CanReference(true))
            return ResourceReference();

        RecordRequest(PreparedRequest{PreparedRequest::Kind::Resource, nullptr, prepared, nullptr});
        return ResourceReference(prepared->provisional);
    }

    ResourceReference ResourcePublisher::AddGroup(SymbolContext& symbol,
                                                  const StaticElementsGroup& elements,
                                                  bool required) {
        auto resource = std::make_unique<PreparedInline>(SymbolContext(symbol.name, SymbolContext::SymbolType::Graphic));
        resource->required = required;

        wk::Ref<SharedShapeWriter> writer = m_writer.AddShape(*resource->context);
        resource->writer = writer;

        writer->AddGroup(symbol, elements);

        PreparedResource* prepared = PrepareResource(std::move(resource), true);
        if (!prepared || !prepared->CanReference(required))
            return ResourceReference();

        RecordRequest(PreparedRequest{PreparedRequest::Kind::Resource, nullptr, prepared, nullptr});
        return ResourceReference(prepared->provisional);
    }

    ResourcePublisher::PreparedResource* ResourcePublisher::PrepareResource(std::unique_ptr<PreparedResource> resource,
                                                                           bool deduplicate) {
        resource->writer->PreFinalize();

        WriterType type = resource->writer->Type();
        if (type != WriterType::MovieClip)
            resource->hash = resource->writer->HashCode();
        resource->has_content = resource->writer->HasContent();

        // Movieclips can only be deduplicated by content after their children got final ids
        if (!deduplicate || type == WriterType::MovieClip) {
            resource->provisional = AllocateProvisional();

            PreparedResource* raw = resource.get();
            {
                std::lock_guard<std::mutex> lock(m_prepareMutex);
                m_prepared.push_back(std::move(resource));
            }

            return raw;
        }

        ResourceClaim claim;
        bool owner = false;
        std::shared_ptr<std::promise<PreparedResource*>> promise;

        {
            std::lock_guard<std::mutex> lock(m_prepareMutex);

            auto& claims = m_inlineClaims[(size_t) type];
            auto cached = claims.find(resource->hash);
            if (cached != claims.end()) {
                claim = cached->second;

                // Same aggregate rule as library symbols
                claim.resource->required = claim.resource->required || resource->required;
            } else {
                PreparedResource* raw = resource.get();
                m_prepared.push_back(std::move(resource));

                promise = std::make_shared<std::promise<PreparedResource*>>();
                claim.resource = raw;
                claim.future = promise->get_future().share();
                claims.emplace(raw->hash, claim);

                raw->provisional = AllocateProvisional();
                owner = true;
            }
        }

        // Identical content is already being prepared, share its reference
        if (!owner)
            return claim.future.get();

        promise->set_value(claim.resource);
        return claim.resource;
    }

    uint16_t ResourcePublisher::CommitSymbol(PreparedSymbol& symbol) {
        if (symbol.committed)
            return symbol.final_id;

        // A symbol that requests itself is either an invalid document or a mutual reference.
        // The original implementation recursed until it overflowed the stack.
        if (symbol.visiting)
            return ResourceReference::Null;

        symbol.visiting = true;
        CommitRequests(symbol);
        symbol.visiting = false;

        return CommitResource(symbol);
    }

    void ResourcePublisher::CommitRequests(PreparedSymbol& symbol) {
        for (const PreparedRequest& request : symbol.requests) {
            switch (request.kind) {
                case PreparedRequest::Kind::Symbol:
                    CommitSymbol(*request.symbol);
                    break;
                case PreparedRequest::Kind::Resource:
                    CommitResource(*request.resource);
                    break;
                case PreparedRequest::Kind::Modifier:
                    CommitModifier(*request.modifier);
                    break;
                default:
                    break;
            }
        }
    }

    uint16_t ResourcePublisher::CommitModifier(PreparedModifier& modifier) {
        if (modifier.committed)
            return modifier.final_id;

        modifier.committed = true;
        modifier.final_id = m_id++;

        m_writer.AddModifier(ResourceReference(modifier.final_id), modifier.type);
        m_idRemap[modifier.provisional] = modifier.final_id;

        return modifier.final_id;
    }

    uint16_t ResourcePublisher::CommitResource(PreparedResource& resource) {
        if (resource.committed)
            return resource.final_id;

        resource.committed = true;

        if (!resource.writer) {
            resource.final_id = ResourceReference::Null;
            return resource.final_id;
        }

        // Children were committed before this resource, so their final ids are known by now
        resource.writer->ApplyResourceRemap(m_idRemap);

        Library* targetLibrary = nullptr;
        switch (resource.writer->Type()) {
            case WriterType::Graphic:
                targetLibrary = &m_graphics;
                break;
            case WriterType::MovieClip:
                targetLibrary = &m_movieClips;
                break;
            case WriterType::TextField:
                targetLibrary = &m_textFields;
                break;
        }

        uint16_t identifier = m_id++;
        std::size_t hash = resource.writer->HashCode();
        auto library_pair = targetLibrary->find(hash);
        bool in_library = library_pair != targetLibrary->end();
        if (in_library) {
            identifier = library_pair->second;
        }

        bool writer_success = resource.writer->Finalize(identifier, resource.required, !in_library);
        if (!writer_success && !in_library) {
            m_id--;
        }

        if (writer_success) {
            targetLibrary->try_emplace(hash, identifier);
        }

        resource.final_id = writer_success || in_library ? identifier : ResourceReference::Null;

        if (resource.provisional != ResourceReference::Null)
            m_idRemap[resource.provisional] = resource.final_id;

        return resource.final_id;
    }

    void ResourcePublisher::Finalize() {
        m_writer.Finalize();
    }
}
