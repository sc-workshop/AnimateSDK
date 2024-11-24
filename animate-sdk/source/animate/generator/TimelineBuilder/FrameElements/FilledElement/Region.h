#pragma once

#include "AnimateDOM.h"

#include "Path.h"

namespace Animate::Publisher
{
	class SymbolContext;

	class FilledElementRegion {
	public:
		struct SolidFill
		{
			DOM::Utils::COLOR color;
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

		union
		{
			SolidFill solid;
		};

		FilledElementPath contour;
		std::vector<FilledElementPath> holes;
	};
}