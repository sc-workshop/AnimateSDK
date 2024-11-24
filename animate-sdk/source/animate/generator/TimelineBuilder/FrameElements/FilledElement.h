#pragma once

#include <vector>

#include "AnimateDOM.h"
#include "AnimateService.h"

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

	class FilledElement {
	public:
		FilledElement(SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape, const DOM::Utils::MATRIX2D& matrix);
		FilledElement(SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape);

	public:
		bool operator==(const FilledElement& other) const;

		bool operator!=(const FilledElement& other) const {
			return !operator==(other);
		}

	public:
		DOM::Utils::RECT Bound() const;
		void Transform(const DOM::Utils::MATRIX2D& matrix);

	public:
		std::vector<FilledElementRegion> fill;
		std::vector<FilledElementRegion> stroke;

		DOM::Utils::MATRIX2D transformation = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

	private:
		void AddRegions(SymbolContext& symbol, FCM::FCMListPtr regions, std::vector<FilledElementRegion>& elements);
	};
}
