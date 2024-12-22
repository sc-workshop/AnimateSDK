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

	template<typename MatrixT>
	class GenericFeatureMatrixLoader
	{
	public:
		virtual void LoadMatrix(MatrixT* matrix) = 0;
	};

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
			FCM::Boolean& isSupported
		);

		virtual FCM::Result _FCMCALL IsSupported(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::Boolean& isSupported
		);

		virtual FCM::Result _FCMCALL IsSupported(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::CStringRep16 valueName,
			FCM::Boolean& isSupported
		);

		virtual FCM::Result _FCMCALL GetDefaultValue(
			FCM::CStringRep16 featureName,
			FCM::CStringRep16 propertyName,
			FCM::VARIANT& output
		);

		void FromJSON(std::filesystem::path path);

	private:
		void ReadFeature(json& feature);
		void ReadProperty(Feature& feature, json& property);
		void ReadValue(Property& property, json& value);

		std::optional<std::reference_wrapper<const Feature>> FindFeature(const std::u16string& inFeatureName) const;

	private:
		Feature::Map m_features;
	};
}