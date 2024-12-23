#pragma once

#include <filesystem>

#include "AnimateDOM.h"
#include "AnimateService.h"

namespace Animate::Publisher
{
	class SpriteElement
	{
	public:
		SpriteElement(
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmap
		);
		~SpriteElement() = default;

	public:
		const std::u16string& Name() const;
		void ExportImage(std::filesystem::path path) const;

	private:
		std::u16string m_name;
		FCM::AutoPtr<DOM::LibraryItem::IMediaItem> m_media_item;
		FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> m_info;
	};
}