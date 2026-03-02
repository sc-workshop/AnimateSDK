#pragma once

#include "AnimateDOM.h"
#include "Path.h"
#include "animate/generator/TimelineBuilder/FrameElements/BitmapElement.h"

#include <variant>

namespace Animate::Publisher {
    class SymbolContext;

    class FilledElementRegion {
    public:
        struct SolidFill {
        public:
            bool operator==(const SolidFill& other) const;

        public:
            DOM::Utils::COLOR color;
        };

        struct BitmapFill {
        public:
            bool operator==(const BitmapFill& other) const;

        public:
            BitmapFill(SymbolContext& context, FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media, const DOM::Utils::MATRIX2D& matrix);

            BitmapElement bitmap;
            bool is_clipped = false;
        };

        struct GradientFill {
        public:
            enum class FillType {
                Linear,
                Radial
            };

            bool operator==(const GradientFill& other) const;

        public:
            DOM::Utils::MATRIX2D matrix;
            DOM::FillStyle::GradientSpread spread;

            FillType type;
            std::vector<DOM::Utils::GRADIENT_COLOR_POINT> points;
            int32_t focal_point;
        };

    public:
        using FillStyle = std::variant<SolidFill, BitmapFill, GradientFill>;

    public:
        enum class ShapeType {
            SolidColor,
            Bitmap,
            GradientColor,
        };

    public:
        FilledElementRegion(SymbolContext& symbol, FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> region);

    public:
        bool operator==(const FilledElementRegion& other) const;

        bool operator!=(const FilledElementRegion& other) const { return !(*this == other); }

        // Bound of contour
        DOM::Utils::RECT Bound() const;
        DOM::Utils::RECT Bound(const DOM::Utils::RECT& other) const;

        void Transform(const DOM::Utils::MATRIX2D& matrix);

    public:
        ShapeType type;
        FillStyle style;

        FilledElementPath contour;
        std::vector<FilledElementPath> holes;
    };
}