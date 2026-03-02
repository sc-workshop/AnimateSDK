#include "Region.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher {
    FilledElementRegion::BitmapFill::BitmapFill(SymbolContext& context, FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media, const DOM::Utils::MATRIX2D& matrix) :
        bitmap(context, media, matrix) {
    }

    FilledElementRegion::FilledElementRegion(SymbolContext& symbol, FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> region) {
        // Fill style
        {
            FCM::AutoPtr<FCM::IFCMUnknown> unknown_style;
            region->GetFillStyle(unknown_style.m_Ptr);

            FCM::AutoPtr<DOM::FillStyle::ISolidFillStyle> solid_style = unknown_style;
            FCM::AutoPtr<DOM::FillStyle::IBitmapFillStyle> bitmap_style = unknown_style;
            FCM::AutoPtr<DOM::FillStyle::IGradientFillStyle> gradient_style = unknown_style;

            if (solid_style) {
                type = ShapeType::SolidColor;

                auto& solid = style.emplace<SolidFill>();
                solid_style->GetColor(solid.color);
            } else if (bitmap_style) {
                type = ShapeType::Bitmap;

                DOM::Utils::MATRIX2D matrix;
                FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media;

                bitmap_style->GetBitmap(media.m_Ptr);
                bitmap_style->GetMatrix(matrix);

                auto& bitmap = style.emplace<BitmapFill>(symbol, media, matrix);
                bool& is_clipped = bitmap.is_clipped;

                bitmap_style->IsClipped((FCM::Boolean&) is_clipped);
            } else if (gradient_style) {
                type = ShapeType::GradientColor;
                auto& gradient = style.emplace<GradientFill>();

                gradient_style->GetMatrix(gradient.matrix);
                gradient_style->GetSpread(gradient.spread);

                FCM::AutoPtr<FCM::IFCMUnknown> unknown_gradient_style;
                gradient_style->GetColorGradient(unknown_gradient_style.m_Ptr);

                FCM::AutoPtr<DOM::Utils::ILinearColorGradient> linear_style = unknown_gradient_style;
                FCM::AutoPtr<DOM::Utils::IRadialColorGradient> radial_style = unknown_gradient_style;

                if (linear_style) {
                    gradient.type = GradientFill::FillType::Linear;

                    uint8_t keys_count = 0;
                    linear_style->GetKeyColorCount(keys_count);

                    gradient.points.resize(keys_count);
                    for (uint8_t i = 0; keys_count > i; i++) {
                        linear_style->GetKeyColorAtIndex(i, gradient.points[i]);
                    }

                } else if (radial_style) {
                    gradient.type = GradientFill::FillType::Radial;

                    uint8_t keys_count = 0;
                    radial_style->GetKeyColorCount(keys_count);

                    gradient.points.resize(keys_count);
                    for (uint8_t i = 0; keys_count > i; i++) {
                        radial_style->GetKeyColorAtIndex(i, gradient.points[i]);
                    }

                    radial_style->GetFocalPoint(gradient.focal_point);
                }
            } else {
                throw FCM::FCMPluginException(symbol, FCM::FCMPluginException::Reason::UNKNOWN_FILL_STYLE);
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
        if (type != other.type)
            return false;

        if (contour != other.contour || holes.size() != other.holes.size())
            return false;

        if (style != other.style)
            return false;

        for (uint32_t i = 0; holes.size() > i; i++) {
            if (holes[i] != other.holes[i])
                return false;
        }

        return true;
    }

    DOM::Utils::RECT FilledElementRegion::Bound() const {
        DOM::Utils::RECT result;
        return Bound(result);
    }

    DOM::Utils::RECT FilledElementRegion::Bound(const DOM::Utils::RECT& other) const {
        DOM::Utils::RECT result = other;
        for (size_t i = 0; contour.Count() > i; i++) {
            const FilledElementPathSegment& segment = contour.GetSegment(i);
            segment.Bound(result);
        }

        return result;
    }

    void FilledElementRegion::Transform(const DOM::Utils::MATRIX2D& matrix) {
        contour.Transform(matrix);

        switch (type) {
            break;
            case ShapeType::Bitmap: {
                BitmapFill& fill = std::get<BitmapFill>(style);
                fill.bitmap.Transform(matrix);
            } break;
            case ShapeType::GradientColor: {
                GradientFill& fill = std::get<GradientFill>(style);
                fill.matrix = fill.matrix * matrix;
            } break;
            default:
                break;
        }

        for (FilledElementPath& hole : holes) {
            hole.Transform(matrix);
        }
    }

    bool FilledElementRegion::SolidFill::operator==(const SolidFill& other) const {
        if (color != other.color)
            return false;

        return true;
    }

    bool FilledElementRegion::BitmapFill::operator==(const BitmapFill& other) const {
        if (is_clipped != other.is_clipped)
            return false;

        if (!(bitmap == other.bitmap))
            return false;

        return true;
    }

    bool FilledElementRegion::GradientFill::operator==(const GradientFill& other) const {
        if (type != other.type)
            return false;

        if (spread != other.spread)
            return false;

        if (matrix != other.matrix)
            return false;

        if (points.size() != other.points.size())
            return false;

        if (focal_point != other.focal_point)
            return false;

        for (size_t i = 0; points.size() > i; i++) {
            if (points[i].pos != other.points[i].pos)
                return false;

            if (points[i].color != other.points[i].color)
                return false;
        }

        return true;
    }
}