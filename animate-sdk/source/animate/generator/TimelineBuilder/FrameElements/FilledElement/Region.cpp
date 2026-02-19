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
        if (type != other.type) {
            return false;
        }
        if (contour != other.contour || holes.size() != other.holes.size()) {
            return false;
        }

        switch (type) {
            case ShapeType::SolidColor: {
                const SolidFill& fill = std::get<SolidFill>(style);
                const SolidFill& other_fill = std::get<SolidFill>(other.style);

                if (fill.color != other_fill.color)
                    return false;
            } break;
            case ShapeType::Bitmap: {
                const BitmapFill fill = std::get<BitmapFill>(style);
                const BitmapFill other_fill = std::get<BitmapFill>(other.style);

                if (fill.is_clipped != other_fill.is_clipped)
                    return false;
                if (!(fill.bitmap == other_fill.bitmap))
                    return false;
            } break;
            case ShapeType::GradientColor: {
                const GradientFill fill = std::get<GradientFill>(style);
                const GradientFill other_fill = std::get<GradientFill>(other.style);

                if (fill.type != other_fill.type)
                    return false;

                if (fill.spread != other_fill.spread)
                    return false;

                if (fill.matrix != other_fill.matrix)
                    return false;

                if (fill.points.size() != other_fill.points.size())
                    return false;

                if (fill.focal_point != other_fill.focal_point)
                    return false;

                for (size_t i = 0; fill.points.size() > i; i++) {
                    if (fill.points[i].pos != other_fill.points[i].pos)
                        return false;

                    if (fill.points[i].color != other_fill.points[i].color)
                        return false;
                }

            } break;
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
}