#pragma once

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <optional>
#include <nlohmann/json.hpp>

#include "AnimateCore.h"

#include "animate/app/DocType/IFeatureMatrix.h"
#include "animate/core/common/FCMPluginInterface.h"

#include "Types.hpp"

namespace Animate::DocType
{
	using namespace FCM;
	class GenericFeatureMatrix : public FCM::FCMObjectBase, public IFeatureMatrix
	{
	public:
		using json = nlohmann::json;

	public:
		BEGIN_MULTI_INTERFACE_MAP(GenericFeatureMatrix)
			INTERFACE_ENTRY(IFeatureMatrix)
		END_INTERFACE_MAP

	public:
		virtual FCM::Result _FCMCALL IsSupported(
			FCM::CStringRep16 featureName,
			FCM::Boolean& isSupported)
		{
#if WK_DEBUG
			isSupported = true;
			return FCM_SUCCESS;
#endif

			std::string featureNameStr = FCM::Locale::ToUtf8((const char16_t*)featureName);

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

		virtual FCM::Result _FCMCALL IsSupported(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::Boolean& isSupported)
		{
#if WK_DEBUG
			isSupported = true;
			return FCM_SUCCESS;
#endif

			std::string featureNameStr = FCM::Locale::ToUtf8((const char16_t*)featureName);
			std::string propertyNameStr = FCM::Locale::ToUtf8((const char16_t*)propertyName);

			if (featureNameStr.empty()) {
				isSupported = true;
				return FCM_SUCCESS;
			}

			if (propertyNameStr.empty()) {
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

		virtual FCM::Result _FCMCALL IsSupported(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::CStringRep16 valueName,
			FCM::Boolean& isSupported)
		{
#if SC_DEBUG
			isSupported = true;
			return FCM_SUCCESS;
#endif
			std::string featureNameStr = FCM::Locale::ToUtf8((const char16_t*)featureName);
			std::string propertyNameStr = FCM::Locale::ToUtf8((const char16_t*)propertyName);
			std::string valueNameStr = FCM::Locale::ToUtf8((const char16_t*)valueName);

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

		virtual FCM::Result _FCMCALL GetDefaultValue(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::VARIANT& output)
		{
			FCM::Result res = FCM_INVALID_PARAM;

#if WK_DEBUG
			return res;
#endif

			std::string featureNameStr = FCM::Locale::ToUtf8((const char16_t*)featureName);
			std::string propertyNameStr = FCM::Locale::ToUtf8((const char16_t*)propertyName);

			Property* property = NULL;
			auto maybe_feature = FindFeature(featureNameStr);
			if (!maybe_feature.has_value()) return res;

			const Feature& feature = maybe_feature.value();
			if (!feature.IsSupported()) return res;
			
			auto maybe_property = feature.FindProperty(propertyNameStr);
			if (!maybe_property.has_value()) return res;

			const Property& feature_property = maybe_property.value();
			if (!feature_property.IsSupported()) return res;

			std::string stringValue = property->GetDefault();
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

		void FromJSON(std::filesystem::path path)
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
				for (auto feature : features) {
					ReadFeature(feature);
				}
			}
		}

	private:
		void ReadFeature(json& feature)
		{
			Feature featureItem = Feature(feature["supported"]);
			std::string featureName = feature["name"];
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

		void ReadProperty(Feature& feature, json& property)
		{
			std::string propertyDefault = "";
			bool isSupported = false;

			if (property["default"].is_string()) {
				propertyDefault = property["default"];
			}

			if (property["supported"].is_boolean()) {
				isSupported = property["supported"];
			}

			Property propertyItem = Property(propertyDefault, isSupported);

			if (property["values"].is_array()) {
				for (auto value : property["values"]) {
					ReadValue(propertyItem, value);
				}
			}

			std::string propertyName = property["name"];
			feature.AddProperty(propertyName, propertyItem);
		}

		void ReadValue(Property& property, json& value)
		{
			property.AddValue(value["name"], Value(value["supported"]));
		}

		std::optional<std::reference_wrapper<const Feature>> FindFeature(const std::string& inFeatureName) const
		{
			Feature::Map::const_iterator itr = m_features.find(inFeatureName);
			if (itr != m_features.end())
			{
				return itr->second;
			}
			return std::nullopt;
		}

	private:
#if WK_DEBUG
		json m_dump = json::object();
#endif
		Feature::Map m_features;
	};
}