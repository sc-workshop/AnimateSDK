#pragma once

#include "AnimateCore.h"
#include "AnimateDOM.h"
#include "AnimateGenerator.h"
#include "ResourceReference.h"
#include "animate/publisher/symbol/SymbolContext.h"

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Animate::Publisher {
    /**
     * @brief Extracts library resources in parallel and commits them in a deterministic order.
     *
     * Extraction is split into two phases:
     *  - Phase A (prepare) builds every export root symbol on a worker thread. All expensive work
     *    (DOM reads, tween evaluation, rasterization, text layout) happens here. Resource ids are
     *    provisional and come from a global atomic counter, so nothing touches the output file.
     *  - Phase B (commit) replays the recorded depth-first request order on the publishing thread,
     *    allocates real ids, applies library deduplication and appends to the output file.
     *
     * Replay order is exactly the call order of the single-threaded implementation, so the
     * produced file stays byte-identical.
     */
    class ResourcePublisher {
    public:
        void PublishDocument(FCM::AutoPtr<DOM::IFLADocument> document);

    private:
        struct PreparedSymbol;
        struct PreparedResource;
        struct PreparedModifier;

        /**
         * @brief One recorded call of the extraction recursion.
         *
         * Requests are stored per symbol and replayed in recorded order during commit, which
         * reproduces the original depth-first order regardless of how threads interleaved.
         */
        struct PreparedRequest {
            enum class Kind : uint8_t {
                Symbol = 0,
                Resource,
                Modifier
            };

            PreparedRequest(Kind kind, PreparedSymbol* symbol, PreparedResource* resource, PreparedModifier* modifier = nullptr) :
                kind(kind),
                symbol(symbol),
                resource(resource),
                modifier(modifier) {}

            Kind kind = Kind::Resource;
            PreparedSymbol* symbol = nullptr;
            PreparedResource* resource = nullptr;
            PreparedModifier* modifier = nullptr;
        };

        /**
         * @brief A finalized-on-commit output object (shape, textfield or movieclip).
         */
        struct PreparedResource {
            explicit PreparedResource(const SymbolContext& context) :
                context(std::make_unique<SymbolContext>(context)) {}

            /**
             * @brief Owned copy of the symbol context.
             *
             * Declared before the writer so the writer, which keeps a reference to it, is destroyed
             * first. The caller's context is long gone when commit happens.
             */
            std::unique_ptr<SymbolContext> context;

            // Guards writer/hash. Symbols can be rebuilt on the thread that first needs a
            // non-static representation, while another thread may still be inspecting the old one.
            mutable std::mutex writerMutex;

            wk::Ref<IDisplayObjectWriter> writer;

            // Compared against the per-type library during commit. Computed during prepare for
            // everything except movieclips, which hash their children ids and therefore must wait
            // until the commit recorded those ids.
            std::size_t hash = 0;

            // Content of the initial (required=false) build. Kept immutable so a call site that
            // does not require the object gets the same answer no matter when it runs relative
            // to a rebuild.
            bool has_content = false;

            // Aggregated with OR over every call site. It is not "whichever thread arrived first":
            // that would make the static/movieclip choice depend on thread scheduling.
            bool required = false;
            bool committed = false;

            uint16_t provisional = ResourceReference::Null;
            uint16_t final_id = ResourceReference::Null;

            /**
             * @brief Reports whether this object can be referenced by a call site.
             *
             * A call that does not require a reference cannot use a contentless object, because
             * the writer would refuse to emit it. Mirrors the old FinalizeWriter null return.
             */
            bool CanReference(bool callRequired) const {
                std::lock_guard<std::mutex> lock(writerMutex);
                return writer && (callRequired || has_content);
            }
        };

        /**
         * @brief A library symbol, with the ordered list of requests it made while preparing.
         */
        struct PreparedSymbol : PreparedResource {
            using PreparedResource::PreparedResource;

            std::vector<PreparedRequest> requests;

            // Serializes rebuilds triggered by a late "required" reference
            std::mutex generationMutex;

            // Whether the current writer was generated with required == true
            bool generated_required = false;

            // Recursion guard: true while this symbol's own requests are being replayed
            bool visiting = false;
        };

        /**
         * @brief An output object that is not a library symbol (shape, textfield, bitmap shape).
         */
        struct PreparedInline : PreparedResource {
            using PreparedResource::PreparedResource;
        };

        /**
         * @brief A masked layer state, deduplicated by type across the whole export.
         */
        struct PreparedModifier {
            MaskedLayerState type = MaskedLayerState::MASK_LAYER;
            uint16_t provisional = ResourceReference::Null;
            uint16_t final_id = ResourceReference::Null;
            bool committed = false;
        };

        /**
         * @brief Get-or-create slot for a shared resource.
         *
         * The first thread to insert the claim builds the resource, all others wait on the future
         * without holding any lock.
         */
        struct ResourceClaim {
            PreparedResource* resource = nullptr;
            std::shared_future<PreparedResource*> future;
        };

    private:
        static void GetExportSymbols(FCM::FCMListPtr libraryItems, std::vector<SymbolContext>& paths);

        /**
         * @brief Stack of symbols whose request list is currently receiving requests.
         *
         * Thread local on purpose: every thread owns the symbols it builds and only appends to
         * their request lists, so recording needs no synchronization.
         */
        static std::vector<PreparedSymbol*>& Sink();

    private:
        // Phase A - preparation (parallel)
        void CheckCancellation();
        PreparedSymbol* PrepareRoot(SymbolContext& symbol);

        PreparedSymbol* PrepareSymbol(SymbolContext& symbol, const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item, bool required);
        PreparedResource* BuildSymbol(PreparedSymbol& prepared,
                                      const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item,
                                      bool required,
                                      bool initial);
        void RebuildSymbolIfRequired(PreparedSymbol& prepared,
                                     const FCM::AutoPtr<DOM::LibraryItem::ISymbolItem>& item);
        ResourceReference PrepareMediaSymbol(SymbolContext& symbol, const FCM::AutoPtr<DOM::LibraryItem::IMediaItem>& media_item, bool required);
        PreparedResource* PrepareResource(std::unique_ptr<PreparedResource> resource, bool deduplicate);
        uint16_t AllocateProvisional();
        void RecordRequest(const PreparedRequest& request);

        // Phase B - commit (serial)
        uint16_t CommitSymbol(PreparedSymbol& symbol);
        uint16_t CommitResource(PreparedResource& resource);
        void CommitRequests(PreparedSymbol& symbol);
        uint16_t CommitModifier(PreparedModifier& modifier);

    private:
        using Library = std::unordered_map<std::size_t, uint16_t, DisplayObjectWriterHasher>;

        // Shape / TextField content hash -> prepared resource
        std::unordered_map<std::size_t, ResourceClaim> m_inlineClaims[3];

        // Symbol context -> prepared symbol
        std::unordered_map<SymbolContext, ResourceClaim> m_symbolClaims;

        // Masked layer state -> prepared modifier
        std::unordered_map<MaskedLayerState, PreparedModifier*> m_modifierCache;

        std::mutex m_prepareMutex;

        std::vector<std::unique_ptr<PreparedResource>> m_prepared;
        std::vector<std::unique_ptr<PreparedSymbol>> m_symbols;
        std::vector<std::unique_ptr<PreparedModifier>> m_modifiers;

        // Independent of m_id so provisional ids can never be mistaken for final ones
        std::atomic<uint32_t> m_provisional = 0;

        // Provisional -> final id, filled during commit in commit order
        std::unordered_map<uint16_t, uint16_t> m_idRemap;

        Library m_textFields;
        Library m_graphics;
        Library m_movieClips;

        uint16_t m_id = 0;

    public:
        SharedWriter& m_writer;

        SymbolGenerator symbolGenerator;
        uint32_t document_fps = 30;

        /// @brief Cooperative cancellation hook, called at safe points between symbols.
        /// The callback may throw whatever exception the host wants to unwind the publish with.
        std::function<void()> cancellationPoint;

        /// @brief Prepare-phase progress: (completed roots, total roots) for the current document.
        std::function<void(size_t, size_t)> prepareProgress;

        /// @brief Called once before the ordered serial commit of prepared roots.
        std::function<void(size_t)> commitStarted;

    public:
        ResourcePublisher(SharedWriter& writer) :
            m_writer(writer),
            symbolGenerator(*this) {}

        /**
         * @brief Special junk function for cases when publishing into external file with existing items
         * @param Id offset
         */
        void SetIdOffset(uint16_t id);

        /**
         * @brief Adds library item into file
         * @param symbol Symbol context
         * @param item Native library item
         * @param required When True, function is required to return non-null symbol reference
         * @return Id reference to item
         */
        ResourceReference AddLibraryItem(SymbolContext& symbol, const FCM::AutoPtr<DOM::ILibraryItem>& item, bool required = false);

        /**
         * @brief Adds modifier (a.k.a masked layer state) into file
         * @param type Masked layer state
         * @return Id reference to item
         */
        ResourceReference AddModifier(MaskedLayerState type);

        /**
         * @brief Adds text field into file
         * @param symbol Symbol context
         * @param field Native text field
         * @param filters Array of filters, for special settings setup
         * @return Id reference to item
         */
        ResourceReference AddTextField(SymbolContext& symbol,
                                       const FCM::AutoPtr<DOM::FrameElement::IClassicText>& field,
                                       const std::optional<FCM::FCMListPtr>& filters = std::nullopt);

        /**
         * @brief Adds a group of sprites and/or filled shapes
         * @param symbol Symbol context
         * @param elements Group of static elements
         * @param required When True, function is required to return non-null symbol reference
         * @return Id reference to item
         */
        ResourceReference AddGroup(SymbolContext& symbol, const StaticElementsGroup& elements, bool required = false);

        /**
         * @brief Finalizes and closes file writer
         */
        void Finalize();
    };
}
