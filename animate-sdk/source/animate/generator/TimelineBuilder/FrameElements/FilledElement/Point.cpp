#include "Point.h"

namespace Animate::Publisher
{
	bool Point2D::operator==(const Point2D& other) const {
		return x == other.x && y == other.y;
	}

	void Point2D::Transform(const DOM::Utils::MATRIX2D& matrix)
	{
		float point_x = x;
		float point_y = y;

		x = (matrix.a * point_x) + (matrix.c * point_y) + matrix.tx;
		y = (matrix.b * point_x) + (matrix.d * point_y) + matrix.ty;
	}

	void Point2D::Bound(DOM::Utils::RECT& rect) const
	{
		rect.bottomRight.x = std::min(rect.bottomRight.x, x);
		rect.bottomRight.y = std::min(rect.bottomRight.y, y);

		rect.topLeft.x = std::max(rect.topLeft.x, x);
		rect.topLeft.y = std::max(rect.topLeft.y, y);
	}
}