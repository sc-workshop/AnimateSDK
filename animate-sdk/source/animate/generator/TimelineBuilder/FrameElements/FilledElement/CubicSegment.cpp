#include "CubicSegment.h"

namespace Animate::Publisher
{
	FilledElementPathCubicSegment::FilledElementPathCubicSegment(const DOM::Utils::SEGMENT& segment) :
		begin(segment.cubicBezierCurve.anchor1.x, segment.cubicBezierCurve.anchor1.y),
		control_l(segment.cubicBezierCurve.control1.x, segment.cubicBezierCurve.control1.y),
		control_r(segment.cubicBezierCurve.control2.x, segment.cubicBezierCurve.control2.y),
		end(segment.cubicBezierCurve.anchor2.x, segment.cubicBezierCurve.anchor2.y)
	{
	}

	Point2D FilledElementPathCubicSegment::Rasterize(float t) const {
		float u = 1.0f - t;

		float x = u * u * u * begin.x + 3 * u * u * t * control_l.x + 3 * u * t * t * control_r.x + t * t * t * end.x;
		float y = u * u * u * begin.y + 3 * u * u * t * control_l.y + 3 * u * t * t * control_r.y + t * t * t * end.y;

		return { x, y };
	};

	float FilledElementPathCubicSegment::IterationStep() const {
		Point2D distance = { std::abs(end.x - begin.x), std::abs(end.y - begin.y) };

		return (1.0f / distance.x) + (1.0f / distance.y);
	};

	void FilledElementPathCubicSegment::Transform(const DOM::Utils::MATRIX2D& matrix)
	{
		begin.Transform(matrix);
		control_l.Transform(matrix);
		control_r.Transform(matrix);
		end.Transform(matrix);
	}

	void FilledElementPathCubicSegment::Bound(DOM::Utils::RECT& rect) const 
	{
		begin.Bound(rect);

		const float step = IterationStep() * 1.5f;
		for (float t = 0.f; 1.0f > t; t += step)
		{
			Rasterize(t).Bound(rect);
		}

		end.Bound(rect);
	}
}