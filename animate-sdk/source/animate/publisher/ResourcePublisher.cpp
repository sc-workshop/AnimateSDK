#include "ResourcePublisher.h"

namespace Animate::Publisher
{
	void ResourcePublisher::PublishDocument(FCM::AutoPtr<DOM::IFLADocument> document) {
		{
			FCM::Double framerate;
			document->GetFrameRate(framerate);
			document_fps = (uint32_t)std::ceil(framerate);
		}

		FCM::FCMListPtr libraryItems;
		document->GetLibraryItems(libraryItems.m_Ptr);

		std::vector<FCM::AutoPtr<DOM::ILibraryItem>> items;
		GetItems(libraryItems, items);

		for (size_t i = 0; items.size() > i; i++)
		{
			FCM::AutoPtr<DOM::ILibraryItem>& item = items[i];

			FCM::AutoPtr<FCM::IFCMDictionary> dict;
			FCM::Result status = item->GetProperties(dict.m_Ptr);

			if (FCM_FAILURE_CODE(status) || dict == nullptr) continue;

			std::string linkage;
			dict->Get(kLibProp_LinkageClass_DictKey, linkage);

			if (linkage.empty()) continue;

			SymbolContext symbol(item, linkage);
			uint16_t id = AddLibraryItem(symbol, item, true);

			if (id == UINT16_MAX)
			{
				throw FCM::FCMPluginException(symbol, FCM::FCMPluginException::Reason::SYMBOL_EXPORT_FAIL);
			}
		}
	}

	void ResourcePublisher::GetItems(FCM::FCMListPtr libraryItems, std::vector<FCM::AutoPtr<DOM::ILibraryItem>>& result) {
		uint32_t itemCount = 0;
		libraryItems->Count(itemCount);

		uint32_t itemIndex = itemCount;
		for (uint32_t i = 0; i < itemCount; i++)
		{
			FCM::AutoPtr<DOM::ILibraryItem> item = libraryItems[--itemIndex];

			FCM::AutoPtr<DOM::LibraryItem::IFolderItem> folderItem = item;
			if (folderItem)
			{
				FCM::FCMListPtr childrens;
				folderItem->GetChildren(childrens.m_Ptr);

				GetItems(childrens, result);
			}
			else
			{
				FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem = item;
				if (!symbolItem) continue;

				result.push_back(item);
			}
		}
	}

	uint16_t ResourcePublisher::AddLibraryItem(
		SymbolContext& symbol,
		FCM::AutoPtr<DOM::ILibraryItem> item,
		bool required
	) {
		if (m_libraryCache.count(symbol.name))
		{
			return m_libraryCache[symbol.name];
		}

		FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbol_item = item;
		FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item = item;

		uint16_t result = 0xFFFF;

		if (symbol_item) {
			result = AddSymbol(symbol, symbol_item, required);
		}
		else if (media_item) {
			result = AddMediaSymbol(symbol, media_item, required);
		}
		else
		{
			throw FCM::FCMPluginException(symbol, FCM::FCMPluginException::Reason::UNKNOWN_LIBRARY_ITEM);
		}
			
		m_libraryCache[symbol.name] = result;

		return result;
	}

	uint16_t ResourcePublisher::AddSymbol(
		SymbolContext& symbol,
		FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> item,
		bool required
	) {
		FCM::AutoPtr<DOM::ITimeline> timeline;
		item->GetTimeLine(timeline.m_Ptr);

		IDisplayObjectWriter* writer = symbolGenerator.Generate(symbol, timeline, required);

		return FinalizeWriter(writer, m_id++, required, m_movieClips);
	};

	uint16_t ResourcePublisher::AddMediaSymbol(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			bool required
	)
	{
		FCM::AutoPtr<FCM::IFCMUnknown> unknownMedia;
		media_item->GetMediaInfo(unknownMedia.m_Ptr);

		FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmap = unknownMedia;

		if (bitmap)
		{
			SharedShapeWriter* writer = m_writer.AddShape(symbol);

			BitmapElement element(symbol, media_item, DOM::Utils::MATRIX2D());
			writer->AddGraphic(element);

			return FinalizeWriter(writer, m_id++, required, m_graphics);
		}

		return 0xFFFF;
	}

	uint16_t ResourcePublisher::AddModifier(
		MaskedLayerState type
	) {
		ModifierDict::const_iterator pos = m_modifierCache.find(type);
		if (pos != m_modifierCache.end())
		{
			return pos->second;
		}

		uint16_t identifer = m_id++;

		m_writer.AddModifier(identifer, type);
		m_modifierCache[type] = identifer;

		return identifer;
	}

	uint16_t ResourcePublisher::AddTextField(
		SymbolContext& symbol,
		FCM::AutoPtr<DOM::FrameElement::IClassicText> textfieldData,
		std::optional<FCM::FCMListPtr> filters
	) {
		SharedTextFieldWriter* writer = m_writer.AddTextField(symbol);
		TextFieldGenerator::Generate(writer, textfieldData);

		return FinalizeWriter(writer, m_id++, false, m_textFields, filters);
	}

	uint16_t ResourcePublisher::AddGroup(
		SymbolContext& symbol,
		const StaticElementsGroup& elements,
		bool required
	) {
		SymbolContext shape_symbol(symbol.name, SymbolContext::SymbolType::Graphic);
		SharedShapeWriter* writer = m_writer.AddShape(shape_symbol);

		writer->AddGroup(symbol, elements);

		return FinalizeWriter(writer, m_id++, required, m_graphics);
	}

	uint16_t ResourcePublisher::FinalizeWriter(
		IDisplayObjectWriter* writer, 
		uint16_t identifier, 
		bool required,
		Library& library,
		std::optional<FCM::FCMListPtr> filters
	)
	{
		if (filters.has_value())
		{
			uint32_t filterCount = 0;
			filters.value()->Count(filterCount);

			for (uint32_t i = 0; filterCount > i; i++)
			{
				FCM::AutoPtr<DOM::GraphicFilter::IGlowFilter> glowFilter = filters.value()[i];

				if (glowFilter) {
					GlowFilter filter;
					glowFilter->GetBlurX(filter.blurX);
					glowFilter->GetBlurY(filter.blurY);
					glowFilter->GetShadowColor(filter.color);
					glowFilter->GetStrength(filter.strength);

					writer->SetGlowFilter(filter);
				}
			}
		}

		std::size_t hash = writer->HashCode();
		auto library_pair = library.find(hash);
		bool in_library = library_pair != library.end();
		if (in_library)
		{
			identifier = library_pair->second;
		}

		bool writer_success = true;
		if (!in_library)
		{
			writer_success = writer->Finalize(identifier, in_library ? false : required);
		}

		if (!writer_success || in_library)
		{
			m_id--;
		}

		if (writer_success)
		{
			library[hash] = identifier;
		}

		delete writer;
		return writer_success || in_library ? identifier : 0xFFFF;
	}

	void ResourcePublisher::Finalize()
	{
		m_writer.Finalize();
	}
}