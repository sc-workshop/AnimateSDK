#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "AnimateCore.h"
#include "AnimateDOM.h"
#include "AnimateGenerator.h"
#include "ResourceReference.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher {
	public:
		void PublishDocument(FCM::AutoPtr<DOM::IFLADocument> document);

	private:
		static void GetExportSymbols(
			FCM::FCMListPtr libraryItems,
			std::vector<SymbolContext>& paths
		);

	private:
		// Shape / Id
		using FilledDictValue = std::pair<std::vector<FilledElement>, ResourceReference>;
		using FilledDict = std::vector<FilledDictValue>;

		// Name  /  Id
		using SymbolDict = std::unordered_map<std::u16string, ResourceReference>;

		// Type / Id
		using ModifierDict = std::unordered_map<MaskedLayerState, ResourceReference>;

		// Info / Id
		using TextsDictValue = std::pair<TextElement, ResourceReference>;
		using TextsDict = std::vector<TextsDictValue>;

		using Library = std::unordered_map<std::size_t, uint16_t, DisplayObjectWriterHasher>;

	private:
		SymbolDict m_libraryCache;
		ModifierDict m_modifierCache;

		Library m_textFields;
		Library m_graphics;
		Library m_movieClips;

		uint16_t m_id = 0;

	public:
		SharedWriter& m_writer;

		SymbolGenerator symbolGenerator;
		uint32_t document_fps = 30;

	public:
		ResourcePublisher(SharedWriter& writer) :
			m_writer(writer), symbolGenerator(*this)
		{
		}

		void SetIdOffset(uint16_t);

		ResourceReference AddLibraryItem(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::ILibraryItem> item,
			bool required = false
		);

		ResourceReference AddModifier(
			MaskedLayerState type
		);

		ResourceReference AddTextField(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::FrameElement::IClassicText> field,
			std::optional<FCM::FCMListPtr> filters = std::nullopt
		);

		ResourceReference AddGroup(
			SymbolContext& symbol,
			const StaticElementsGroup& elements,
			bool required = false
		);

		void Finalize();

	private:
		ResourceReference AddSymbol(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> item,
			bool required = false
		);

		ResourceReference AddMediaSymbol(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			bool required
		);

		ResourceReference FinalizeWriter(
			wk::Ref<IDisplayObjectWriter> writer,
			bool required,
			Library& library,
			std::optional<FCM::FCMListPtr> filters = std::nullopt
		);
	};
}