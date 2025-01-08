#pragma once

#include "AnimateDOM.h"
#include "Segment.h"

namespace Animate::Publisher
{
	class FilledElementPathCubicSegment : public FilledElementPathSegment
	{
	public:
		FilledElementPathCubicSegment(const DOM::Utils::SEGMENT& segment);

	public:
		virtual FilledElementPathSegment::Type SegmentType() const {
			return FilledElementPathSegment::Type::Cubic;
		};

		virtual Point2D Rasterize(float t_step) const;

		virtual float IterationStep() const;

		virtual void Transform(const DOM::Utils::MATRIX2D& matrix);
		virtual void Bound(DOM::Utils::RECT& rect) const;

	public:
		Point2D begin;
		Point2D control_l;
		Point2D control_r;
		Point2D end;
	};
}