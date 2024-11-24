#pragma once

#include <vector>

#include "core/memory/ref.h"

#include "AnimateDOM.h"
#include "AnimateService.h"

#include "LineSegment.h"
#include "CubicSegment.h"
#include "QuadSegment.h"
#include "Point.h"
#include "Segment.h"

namespace Animate::Publisher
{

	class FilledElementPath {
	public:
		FilledElementPath();
		FilledElementPath(FCM::AutoPtr<DOM::Service::Shape::IPath> path);

	public:
		bool operator==(const FilledElementPath& other) const;

		bool operator!=(const FilledElementPath& other) const {
			return !operator==(other);
		}

	public:
		const FilledElementPathSegment& GetSegment(size_t index) const;
		FilledElementPathSegment& GetSegment(size_t index);
		void Rasterize(std::vector<Point2D>& points) const;
		size_t Count() const;
		void Transform(const DOM::Utils::MATRIX2D&);

	private:
		std::vector<wk::Ref<FilledElementPathSegment>> m_segments;

		template<typename T, class ... Args>
		T& CreateSegment(Args ... args)
		{
			wk::Ref<T> object = wk::CreateRef<T>(args...);
			m_segments.push_back(object);
			return *object;
		}
	};

}