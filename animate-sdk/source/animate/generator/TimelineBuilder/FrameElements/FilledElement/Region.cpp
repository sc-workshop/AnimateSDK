#include "Region.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	FilledElementRegion::BitmapFill::BitmapFill(SymbolContext& context, FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media, const DOM::Utils::MATRIX2D& matrix) : 
		bitmap(context, media, matrix)
	{

	}

	FilledElementRegion::FilledElementRegion(SymbolContext& symbol, FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> region) {
		// Fill style
		{
			FCM::AutoPtr<FCM::IFCMUnknown> unknown_style;
			region->GetFillStyle(unknown_style.m_Ptr);

			FCM::AutoPtr<DOM::FillStyle::ISolidFillStyle> solid_style = unknown_style;
			FCM::AutoPtr<DOM::FillStyle::IBitmapFillStyle> bitmap_style = unknown_style;

			if (solid_style) {
				type = ShapeType::SolidColor;

				auto& solid = style.emplace<SolidFill>();
				solid_style->GetColor(solid.color);
			}
			else if (bitmap_style)
			{
				type = ShapeType::Bitmap;

				DOM::Utils::MATRIX2D matrix;
				FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media;

				bitmap_style->GetBitmap(media.m_Ptr);
				bitmap_style->GetMatrix(matrix);

				auto& bitmap = style.emplace<BitmapFill>(symbol, media, matrix);
				bool& is_clipped = bitmap.is_clipped;

				bitmap_style->IsClipped((FCM::Boolean&)is_clipped);
			}
			else {
				throw FCM::FCMPluginException(
					symbol, 
					FCM::FCMPluginException::Reason::UNKNOWN_FILL_STYLE
				);
			}
		}

		// Contour
		{
			FCM::AutoPtr<DOM::Service::Shape::IPath> polygonPath;
			region->GetBoundary(polygonPath.m_Ptr);
			contour = FilledElementPath(polygonPath);
		}

		// Holes
		{
			FCM::FCMListPtr holePaths;
			uint32_t holePathsCount = 0;
			region->GetHoles(holePaths.m_Ptr);
			holePaths->Count(holePathsCount);

			for (uint32_t i = 0; holePathsCount > i; i++) {
				FCM::AutoPtr<DOM::Service::Shape::IPath> holePath = holePaths[i];

				holes.emplace_back(holePath);
			}
		}
	}

	bool FilledElementRegion::operator==(const FilledElementRegion& other) const {
		if (type != other.type) { return false; }
		if (contour != other.contour || holes.size() != other.holes.size()) { return false; }

		switch (type)
		{
		case ShapeType::SolidColor:
			{
				const SolidFill& fill = std::get<SolidFill>(style);
				const SolidFill& other_fill = std::get<SolidFill>(other.style);

				if (*(uint32_t*)&fill.color != *(uint32_t*)&other_fill.color) { return false; };
			}
			break;
		case ShapeType::Bitmap:
			{
				const BitmapFill fill = std::get<BitmapFill>(style);
				const BitmapFill other_fill = std::get<BitmapFill>(other.style);

				if (fill.is_clipped != other_fill.is_clipped) return false;
				if (!(fill.bitmap == other_fill.bitmap)) return false;
			}
			break;
		default:
			return false;
		}

		for (uint32_t i = 0; holes.size() > i; i++) {
			if (holes[i] != other.holes[i]) {
				return false;
			}
		}

		return true;
	}

	DOM::Utils::RECT FilledElementRegion::Bound() const
	{
		DOM::Utils::RECT result{
			{-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max()},
			{std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()}
		};

		for (size_t i = 0; contour.Count() > i; i++)
		{
			const FilledElementPathSegment& segment = contour.GetSegment(i);
			segment.Bound(result);
		}

		return result;
	}

	void FilledElementRegion::Transform(const DOM::Utils::MATRIX2D& matrix)
	{
		contour.Transform(matrix);

		switch (type)
		{
		break;
		case ShapeType::Bitmap:
		{
			BitmapFill& fill = std::get<BitmapFill>(style);
			fill.bitmap.Transform(matrix);
		}
		break;
		case ShapeType::GradientColor:
		{
			// TODO
		}
		break;
		default:
			break;
		}

		for (FilledElementPath& hole : holes)
		{
			hole.Transform(matrix);
		}
	}
}