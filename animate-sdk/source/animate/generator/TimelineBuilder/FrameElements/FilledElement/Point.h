#pragma once

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class Point2D {
	public:
		Point2D() {};
		Point2D(float x, float y) : x(x), y(y) {};

	public:
		bool operator==(const Point2D& other) const;

		bool operator!=(const Point2D& other) const {
			return !operator==(other);
		}

	public:
		void Transform(const DOM::Utils::MATRIX2D& matrix);

	public:
		void Bound(DOM::Utils::RECT& rect) const;

	public:
		float x = 0;
		float y = 0;
	};
}