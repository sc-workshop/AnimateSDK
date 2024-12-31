#include "SpriteElement.h"

namespace Animate::Publisher
{
	BitmapElement::BitmapElement(
			SymbolContext& symbol,
			FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item,
			const DOM::Utils::MATRIX2D& matrix
	) : StaticElement(symbol), m_media_item(media_item)
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		m_matrix = matrix;
		FCM::AutoPtr<DOM::ILibraryItem> library_item = media_item;

		m_name = context.falloc->GetString16(
			library_item.m_Ptr,
			&DOM::ILibraryItem::GetName
		);
	}

	void BitmapElement::ExportImage(std::filesystem::path path) const
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

	const std::u16string& BitmapElement::Name() const
	{
		return m_name;
	}
}