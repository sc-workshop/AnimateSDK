#pragma once

#include <vector>
#include <optional>

#include "AnimateDOM.h"
#include "AnimateService.h"

#include "StaticElement.h"
#include "FilledElement/Region.h"

// ----------------------------------------------
// --------------- Mini Docs --------------------
// - Filled Element hierarchy: ------------------
// ---- Fill[]/Stoke[]: -------------------------	// FilledElementRegion
// -------- Type: (Solid, Bitmap, Gradient) -----
// -------- Contour/Holes[]: --------------------	// FilledElementPath
// ------------ Point Holder: (Count, GetSegment )  // FilledElementPathSegment
// ---------------- Point Iterator --------------	// FilledElementPathLineSegment, FilledElementPathCubicSegment, FilledElementPathQuadSegment
// ----------------------------------------------

namespace Animate::Publisher
{
	class SymbolContext;

	class FilledElement : public StaticElement
	{
	public:
		using Contour = std::vector<FilledElementRegion>;

	public:
		FilledElement(
			SymbolContext& symbol, 
			FCM::AutoPtr<DOM::FrameElement::IShape> shape, 
			const std::optional<DOM::Utils::MATRIX2D> matrix = std::nullopt
		);
		FilledElement& FilledElement::operator=(const FilledElement&) = default;

	public:
		bool operator==(const FilledElement& other) const;

		bool operator!=(const FilledElement& other) const {
			return !operator==(other);
		}

	public:
		DOM::Utils::RECT Bound() const;
		void Transform(const DOM::Utils::MATRIX2D& matrix);

	public:
		Contour fill;
		Contour stroke;

		virtual bool IsFilledArea() const
		{
			return true;
		}

	private:
		void AddRegions(SymbolContext& symbol, FCM::FCMListPtr regions, std::vector<FilledElementRegion>& elements);
	};
}
