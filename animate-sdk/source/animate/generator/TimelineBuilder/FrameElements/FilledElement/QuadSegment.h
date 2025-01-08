#pragma once

#include "AnimateDOM.h"

#include "Segment.h"

namespace Animate::Publisher
{
	class FilledElementPathQuadSegment : public FilledElementPathSegment
	{
	public:
		FilledElementPathQuadSegment(const DOM::Utils::SEGMENT& segment);

	public:
		virtual FilledElementPathSegment::Type SegmentType() const {
			return FilledElementPathSegment::Type::Quad;
		};

		virtual Point2D Rasterize(float t_step) const;

		virtual float IterationStep() const;

		virtual void Transform(const DOM::Utils::MATRIX2D& matrix);
		virtual void Bound(DOM::Utils::RECT& rect) const;

	public:
		Point2D begin;
		Point2D control;
		Point2D end;
	};

}