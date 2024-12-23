#include "SpriteElement.h"

namespace Animate::Publisher
{
	SpriteElement::SpriteElement(
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmap
	) : m_media_item(media_item), m_info(bitmap)
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		FCM::AutoPtr<DOM::ILibraryItem> library_item = media_item;

		FCM::StringRep16 namePtr;
		library_item->GetName(namePtr);
		m_name = std::u16string((const char16_t*)namePtr);
		context.falloc->Free(namePtr);
	}

	void SpriteElement::ExportImage(std::filesystem::path path) const
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		auto service = context.GetService<DOM::Service::Image::IBitmapExportService>(DOM::FLA_BITMAP_SERVICE);

		FCM::Result res = service->ExportToFile(
			m_media_item,
			(FCM::CStringRep16)path.u16string().c_str(),
			100
		);


		if (FCM_FAILURE_CODE(res)) {
			throw FCM::FCMPluginException(FCM::FCMPluginException::Reason::SERVICE_FAIL);
		}
	}

	const std::u16string& SpriteElement::Name() const
	{
		return m_name;
	}
}