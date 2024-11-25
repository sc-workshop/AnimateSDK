#pragma once

#include <filesystem>

#include "AnimateDOM.h"
#include "AnimateService.h"

namespace Animate::Publisher
{
	class SpriteElement
	{
	public:
		SpriteElement(DOM::ILibraryItem* library_item, DOM::LibraryItem::IMediaItem* media_item, DOM::MediaInfo::IBitmapInfo* bitmap);
		~SpriteElement();

	public:
		const char16_t* Name() const;
		void ExportImage(std::filesystem::path path) const;

	private:
		DOM::ILibraryItem* m_item;
		DOM::LibraryItem::IMediaItem* m_media_item;
		DOM::MediaInfo::IBitmapInfo* m_info;

		char16_t* m_name;
	};
}