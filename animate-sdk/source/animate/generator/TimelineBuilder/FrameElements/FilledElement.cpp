#include "FilledElement.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	FilledElement::FilledElement(
		SymbolContext& symbol, FCM::AutoPtr<DOM::FrameElement::IShape> shape,
			const std::optional<DOM::Utils::MATRIX2D> matrix
	) : StaticElement(symbol)
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		if (matrix.has_value())
		{
			m_matrix = matrix.value();
		}

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
			{-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max()},
			{std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()}
		};

		for (const FilledElementRegion& region : fill)
		{
			result = result + region.Bound();
		}

		for (const FilledElementRegion& region : stroke)
		{
			result = result + region.Bound();
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