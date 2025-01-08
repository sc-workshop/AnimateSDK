#pragma once

#include "animate/app/DOM/Utils/DOMTypes.h"
#include "Segment.h"

namespace Animate::Publisher
{
	class FilledElementPathLineSegment : public FilledElementPathSegment
	{
	public:
		FilledElementPathLineSegment(const DOM::Utils::SEGMENT& segment);

	public:
		virtual FilledElementPathSegment::Type SegmentType() const {
			return FilledElementPathSegment::Type::Line;
		};

		virtual Point2D Rasterize(float t_step) const;

		virtual float IterationStep() const;

		virtual void Transform(const DOM::Utils::MATRIX2D& matrix);
		virtual void Bound(DOM::Utils::RECT& rect) const;

	public:
		Point2D begin;
		Point2D end;
	};
}