#include "GenericFeatureMatrix.h"

namespace Animate::DocType
{
	FCM::Result _FCMCALL GenericFeatureMatrix::IsSupported(
			FCM::CStringRep16 featureName,
			FCM::Boolean& isSupported)
	{
		if (!ANIMATE_SDK_USE_FEATURE_MATRIX)
		{
			isSupported = true;
			return FCM_SUCCESS;
		}

		std::u16string featureNameStr((const char16_t*)featureName);

		if (featureNameStr.empty()) {
			isSupported = true;
			return FCM_SUCCESS;
		}

		auto feature = FindFeature(featureNameStr);
		if (!feature.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		isSupported = (feature->get()).IsSupported();
		return FCM_SUCCESS;
	};

	FCM::Result _FCMCALL GenericFeatureMatrix::IsSupported(
		FCM::CStringRep16 featureName,
		FCM::CStringRep16 propertyName,
		FCM::Boolean& isSupported)
	{
		if (!ANIMATE_SDK_USE_FEATURE_MATRIX)
		{
			isSupported = true;
			return FCM_SUCCESS;
		}

		std::u16string featureNameStr((const char16_t*)featureName);
		std::u16string propertyNameStr((const char16_t*)propertyName);

		if (featureNameStr.empty() || propertyNameStr.empty()) {
			isSupported = true;
			return FCM_SUCCESS;
		}

		auto maybe_feature = FindFeature(featureNameStr);
		if (!maybe_feature.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		const Feature& feature = maybe_feature.value();
		if (!feature.IsSupported())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		auto maybe_property = feature.FindProperty(propertyNameStr);
		if (!maybe_property.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		const Property& feature_property = maybe_property.value();
		isSupported = feature_property.IsSupported();
		return FCM_SUCCESS;
	}

	FCM::Result _FCMCALL GenericFeatureMatrix::IsSupported(
		FCM::CStringRep16 featureName,
		FCM::CStringRep16 propertyName,
		FCM::CStringRep16 valueName,
		FCM::Boolean& isSupported)
	{
		if (!ANIMATE_SDK_USE_FEATURE_MATRIX)
		{
			isSupported = true;
			return FCM_SUCCESS;
		}

		std::u16string featureNameStr((const char16_t*)featureName);
		std::u16string propertyNameStr((const char16_t*)propertyName);
		std::u16string valueNameStr((const char16_t*)valueName);

		if (featureNameStr.empty() || propertyNameStr.empty() || valueNameStr.empty()) {
			isSupported = true;
			return FCM_SUCCESS;
		}

		auto maybe_feature = FindFeature(featureNameStr);
		if (!maybe_feature.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		const Feature& feature = maybe_feature.value();
		if (!feature.IsSupported())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		auto maybe_property = feature.FindProperty(propertyNameStr);
		if (!maybe_property.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		const Property& feature_property = maybe_property.value();
		if (!feature_property.IsSupported())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		auto maybe_value = feature_property.FindValue(valueNameStr);
		if (!maybe_value.has_value())
		{
			isSupported = false;
			return FCM_SUCCESS;
		}

		const Value& feature_value = maybe_value.value();
		isSupported = feature_value.IsSupported();
		return FCM_SUCCESS;
	}

	FCM::Result _FCMCALL GenericFeatureMatrix::GetDefaultValue(
		FCM::CStringRep16 featureName,
		FCM::CStringRep16 propertyName,
		FCM::VARIANT& output)
	{
		FCM::Result res = FCM_INVALID_PARAM;

		if (!ANIMATE_SDK_USE_FEATURE_MATRIX)
		{
			return FCM_SUCCESS;
		}

		std::u16string featureNameStr((const char16_t*)featureName);
		std::u16string propertyNameStr((const char16_t*)propertyName);

		if (featureNameStr.empty() || propertyNameStr.empty())
		{
			return res;
		}

		auto maybe_feature = FindFeature(featureNameStr);
		if (!maybe_feature.has_value()) return res;

		const Feature& feature = maybe_feature.value();
		if (!feature.IsSupported()) return res;

		auto maybe_property = feature.FindProperty(propertyNameStr);
		if (!maybe_property.has_value()) return res;

		const Property& feature_property = maybe_property.value();
		if (!feature_property.IsSupported()) return res;

		std::string stringValue = feature_property.GetDefault();
		if (stringValue.empty()) {
			return FCM_INVALID_PARAM;
		}

		res = FCM_SUCCESS;
		switch (output.type) {
		case FCM::VarType::UInt32:
			output.value.uVal = std::stoi(stringValue);
			break;
		case FCM::VarType::Float:
			output.value.fVal = std::stof(stringValue);
			break;
		case FCM::VarType::Bool:
			output.value.bVal = (stringValue == "true");
			break;
		case FCM::VarType::String:
		{
			std::u16string u16StringValue = FCM::Locale::ToUtf16(stringValue);
			output.value.strVal = (FCM::StringRep16)u16StringValue.c_str();
		}
		break;
		case FCM::VarType::Double:
			output.value.dVal = std::stod(stringValue);
			break;
		default:
			res = FCM_INVALID_PARAM;
			break;
		}

		return res;
	}

	void GenericFeatureMatrix::FromJSON(std::filesystem::path path)
	{
		FCM::PluginModule& context = PluginModule::Instance();

		std::ifstream file(path);
		json features = json::array();
		try
		{
			features = json::parse(file, nullptr, true, true);
		}
		catch (const json::exception exception)
		{
			context.Trace("Failed to read FeatureMatrix file");
			context.Trace(exception.what());
		}

		if (features.is_array()) {
			for (auto &feature : features) {
				ReadFeature(feature);
			}
		}
	}

	void GenericFeatureMatrix::ReadFeature(json& feature)
	{
		Feature featureItem = Feature(feature["supported"]);
		std::u16string featureName = Locale::ToUtf16(feature["name"]);
		auto featureProperties = feature["properties"];

		if (featureProperties.is_array()) {
			for (auto& featureProperty : featureProperties) {
				if (featureProperty.is_object()) {
					ReadProperty(featureItem, featureProperty);
				}
			}
		}

		m_features.insert(std::pair(featureName, featureItem));
	}

	void GenericFeatureMatrix::ReadProperty(Feature& feature, json& property)
	{
		std::string propertyDefault = "";
		bool isSupported = false;

		if (property["default"].is_string()) {
			propertyDefault = property["default"];
		}

		if (property["supported"].is_boolean()) {
			isSupported = property["supported"];
		}

		Property propertyItem(propertyDefault, isSupported);

		if (property["values"].is_array()) {
			for (auto& value : property["values"]) {
				ReadValue(propertyItem, value);
			}
		}

		std::u16string propertyName = Locale::ToUtf16(property["name"]);
		feature.AddProperty(propertyName, propertyItem);
	}

	void GenericFeatureMatrix::ReadValue(Property& property, json& value)
	{
		std::u16string value_name = Locale::ToUtf16(value["name"]);
		property.AddValue(value_name, Value(value["supported"]));
	}

	std::optional<std::reference_wrapper<const Feature>> GenericFeatureMatrix::FindFeature(const std::u16string& inFeatureName) const
	{
		Feature::Map::const_iterator itr = m_features.find(inFeatureName);
		if (itr != m_features.end())
		{
			return itr->second;
		}
		return std::nullopt;
	}
}