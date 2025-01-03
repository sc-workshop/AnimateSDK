#include "QuadSegment.h"

namespace Animate::Publisher
{
	FilledElementPathQuadSegment::FilledElementPathQuadSegment(const DOM::Utils::SEGMENT& segment) :
		begin(segment.quadBezierCurve.anchor1.x, segment.quadBezierCurve.anchor1.y),
		control(segment.quadBezierCurve.control.x, segment.quadBezierCurve.control.y),
		end(segment.quadBezierCurve.anchor2.x, segment.quadBezierCurve.anchor2.y)
	{
	}

	Point2D FilledElementPathQuadSegment::Rasterize(float t_step) const {
		float u = 1.0f - t_step;
		float x = u * u * begin.x + 2 * u * t_step * control.x + t_step * t_step * end.x;
		float y = u * u * begin.y + 2 * u * t_step * control.y + t_step * t_step * end.y;

		return { x, y };
	};

	float FilledElementPathQuadSegment::IterationStep() const {
		Point2D distance(0, 0);
		distance.x += std::abs(begin.x - control.x);
		distance.y += std::abs(begin.y - control.y);

		distance.x += std::abs(control.x - end.x);
		distance.y += std::abs(control.y - end.y);

		return (1.0f / distance.x) + (1.0f / distance.y);
	};

	void FilledElementPathQuadSegment::Transform(const DOM::Utils::MATRIX2D& matrix)
	{
		begin.Transform(matrix);
		control.Transform(matrix);
		end.Transform(matrix);
	}
}