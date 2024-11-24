#include "FilledElement.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	FilledElement::FilledElement(SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape, const DOM::Utils::MATRIX2D& matrix)
		: FilledElement(symbol, shape)
	{
		transformation = matrix;
	}

	FilledElement::FilledElement(SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape) {
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		auto stroke_generator =
			context.GetService<DOM::Service::Shape::IShapeService>(DOM::FLA_SHAPE_SERVICE);

		auto region_generator =
			context.GetService<DOM::Service::Shape::IRegionGeneratorService>(DOM::FLA_REGION_GENERATOR_SERVICE);

		{
			FCM::FCMListPtr regions;
			region_generator->GetFilledRegions(shape, regions.m_Ptr);

			AddRegions(symbol, regions, fill);
		}

		{
			FCM::AutoPtr<DOM::FrameElement::IShape> stroke_fill;
			stroke_generator->ConvertStrokeToFill(shape, stroke_fill.m_Ptr);

			FCM::FCMListPtr regions;
			region_generator->GetFilledRegions(stroke_fill, regions.m_Ptr);

			AddRegions(symbol, regions, stroke);
		}
	}

	bool FilledElement::operator==(const FilledElement& other) const {
		if (fill.size() != other.fill.size() || stroke.size() != other.stroke.size()) { return false; }

		for (size_t i = 0; fill.size() > i; i++) {
			if (fill[i] != other.fill[i]) {
				return false;
			}
		}

		for (size_t i = 0; stroke.size() > i; i++) {
			if (stroke[i] != other.stroke[i]) {
				return false;
			}
		}

		return true;
	}

	void FilledElement::AddRegions(SymbolContext& symbol, FCM::FCMListPtr regions, std::vector<FilledElementRegion>& elements)
	{
		uint32_t region_count;
		regions->Count(region_count);

		for (uint32_t i = 0; region_count > i; i++) {
			FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> region = regions[i];
			if (!region) continue;

			elements.emplace_back(symbol, region);
		}
	}

	DOM::Utils::RECT FilledElement::Bound() const
	{
		DOM::Utils::RECT result{
			{std::numeric_limits<float>::min(),
			std::numeric_limits<float>::min()},
			{std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()}
		};

		for (const FilledElementRegion& region : fill)
		{
			const DOM::Utils::RECT region_bound = region.Bound();

			result.topLeft.x = std::max(region_bound.topLeft.x, result.topLeft.x);
			result.topLeft.y = std::max(region_bound.topLeft.y, result.topLeft.y);
			result.bottomRight.x = std::min(region_bound.bottomRight.x, result.bottomRight.x);
			result.bottomRight.y = std::min(region_bound.bottomRight.y, result.bottomRight.y);
		}

		for (const FilledElementRegion& region : stroke)
		{
			const DOM::Utils::RECT region_bound = region.Bound();

			result.topLeft.x = std::max(region_bound.topLeft.x, result.topLeft.x);
			result.topLeft.y = std::max(region_bound.topLeft.y, result.topLeft.y);
			result.bottomRight.x = std::min(region_bound.bottomRight.x, result.bottomRight.x);
			result.bottomRight.y = std::min(region_bound.bottomRight.y, result.bottomRight.y);
		}

		return result;
	}

	void FilledElement::Transform(const DOM::Utils::MATRIX2D& matrix)
	{
		for (FilledElementRegion& region : fill)
		{
			region.Transform(matrix);
		}

		for (FilledElementRegion& region : stroke)
		{
			region.Transform(matrix);
		}
	}
}