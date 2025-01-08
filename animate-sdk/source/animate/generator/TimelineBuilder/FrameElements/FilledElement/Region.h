#pragma once

#include "AnimateDOM.h"

#include "Path.h"

#include "animate/generator/TimelineBuilder/FrameElements/BitmapElement.h"

#include <variant>

namespace Animate::Publisher
{
	class SymbolContext;

	class FilledElementRegion {
	public:
		struct SolidFill
		{
			DOM::Utils::COLOR color;
		};

		struct BitmapFill
		{
			BitmapFill(SymbolContext& context, FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media, const DOM::Utils::MATRIX2D& matrix);

			BitmapElement bitmap;
			bool is_clipped;
		};

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

		bool operator!=(const FilledElementRegion& other) const {
			return !(*this == other);
		}

		// Bound of contour
		DOM::Utils::RECT Bound() const;

		void Transform(const DOM::Utils::MATRIX2D& matrix);

	public:
		ShapeType type;

		std::variant<SolidFill, BitmapFill> style;

		FilledElementPath contour;
		std::vector<FilledElementPath> holes;
	};
}