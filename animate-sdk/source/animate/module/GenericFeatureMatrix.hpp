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

#include "FeaturesTypes.hpp"

namespace Animate::DocType
{
	using namespace FCM;
	class GenericFeatureMatrix final : public FCM::FCMObjectBase, public IFeatureMatrix
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

			isSupported = feature->IsSupported();
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

			auto feature = FindFeature(featureNameStr);
			if (!feature.has_value())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			if (!feature->IsSupported())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			auto property = feature->FindProperty(propertyNameStr);
			if (!property.has_value())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			isSupported = property->IsSupported();
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

			auto feature = FindFeature(featureNameStr);
			if (!feature.has_value())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}
			
			if (!feature->IsSupported())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			auto feature_property = feature->FindProperty(propertyNameStr);
			if (!feature_property.has_value())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			if (!feature_property->IsSupported())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			auto property_value = feature_property->FindValue(valueNameStr);
			if (!property_value.has_value())
			{
				isSupported = false;
				return FCM_SUCCESS;
			}

			isSupported = property_value->IsSupported();
			return FCM_SUCCESS;
		}

		virtual FCM::Result _FCMCALL GetDefaultValue(
			FCM::CStringRep16 inFeatureName,
			FCM::CStringRep16 inPropName,
			FCM::VARIANT& outDefVal);

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

		std::optional<const Feature&> FindFeature(const std::string& inFeatureName) const
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