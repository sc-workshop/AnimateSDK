#pragma once

#include <filesystem>

#include "AnimateDOM.h"
#include "AnimateService.h"

#include "FrameElement.h"

namespace Animate::Publisher
{
	class BitmapElement : public StaticElement
	{
	public:
		BitmapElement(
			SymbolContext& context,
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			const DOM::Utils::MATRIX2D& matrix
		);
		~BitmapElement() = default;

	public:
		const std::u16string& Name() const;
		void ExportImage(std::filesystem::path path) const;

		virtual bool IsSprite() const
		{
			return true;
		}

	private:
		std::u16string m_name;
		FCM::AutoPtr<DOM::LibraryItem::IMediaItem> m_media_item;
	};
}