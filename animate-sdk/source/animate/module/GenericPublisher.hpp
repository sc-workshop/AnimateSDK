#pragma once

#include "AnimateCore.h"

#include "animate/app/Publisher/IPublisher.h"
#include "GenericPublisherConfig.hpp"
#include "Types.hpp"

namespace Animate::Publisher
{
	using namespace FCM;

	template<class ConfigT = GenericPublisherConfig>
	class GenericPublisherInterface
	{
	public:
		virtual void Publish(const ConfigT& config) = 0;
	};

	template<class ConfigT = GenericPublisherConfig, class PublisherT = GenericPublisherInterface<ConfigT>>
	class GenericPublisher : public IPublisher, public FCM::FCMObjectBase
	{
	public:
		BEGIN_INTERFACE_MAP(GenericPublisher)
			INTERFACE_ENTRY(IPublisher)
		END_INTERFACE_MAP

	public:
		FCM::Result _FCMCALL Publish(DOM::PIFLADocument document, const FCM::PIFCMDictionary publishConfig, const FCM::PIFCMDictionary projectConfig)
		{
			ConfigT config(document, projectConfig, publishConfig);
			PublisherT publisher{};

			publisher.Publish(config);

			return FCM_SUCCESS;
		}

		FCM::Result _FCMCALL Publish(
			DOM::PIFLADocument document,
			DOM::PITimeline pTimeline,
			const Exporter::Service::RANGE& frameRange,
			const FCM::PIFCMDictionary publishSettings,
			const FCM::PIFCMDictionary config
		) {
			return FCM_SERVICE_NOT_FOUND;
		}

		FCM::Result _FCMCALL ClearCache() { return FCM_SERVICE_NOT_FOUND; };

	public:
		static const Animate::FCMPluginID PluginID;
	};

	static FCM::Result RegisterPublisher(
		FCM::PIFCMDictionary plugins,
		FCM::FCMCLSID publisher_id,
		const ModuleInfo& module_info)
	{
		FCM::Result result;

		/*
		 * Dictionary structure for a Publisher plugin is as follows:
		 *
		 *  Level 0 :
		 *              --------------------------------
		 *             | Application.Component |  ----- | -----------------------------
		 *              --------------------------------                               |
		 *                                                                             |
		 *  Level 1:                                   <-------------------------------
		 *              ------------------------------
		 *             | CLSID_Publisher_GUID | ----- | -------------------------------
		 *              ------------------------------                                 |
		 *                                                                             |
		 *  Level 2:                                      <----------------------------
		 *              ---------------------------------------------------
		 *             | Application.Component.Category.Publisher |  ----- |-----------
		 *              ---------------------------------------------------            |
		 *                                                                             |
		 *  Level 3:                                                           <-------
		 *              -------------------------------------------------------------------------
		 *             | Application.Component.Category.Name           | PUBLISHER_NAME          |
		 *              -------------------------------------------------------------------------|
		 *             | Application.Component.Category.UniversalName  | PUBLISHER_UNIVERSAL_NAME|
		 *              -------------------------------------------------------------------------|
		 *             | Application.Component.Publisher.UI            | PUBLISH_SETTINGS_UI_ID  |
		 *              -------------------------------------------------------------------------|
		 *             | Application.Component.Publisher.TargetDocs    |              -----------|--
		 *              -------------------------------------------------------------------------| |
		 *                                                                                         |
		 *  Level 4:                                                    <--------------------------
		 *              -----------------------------------------------
		 *             | CLSID_DocType   |  Empty String               |
		 *              -----------------------------------------------
		 *
		 *  Note that before calling this function the level 0 dictionary has already
		 *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
		 */

		{
			// Level 1 Dictionary
			FCM::AutoPtr<FCM::IFCMDictionary> plugin;
			result = plugins->AddLevel(
				((std::string)publisher_id).c_str(),
				plugin.m_Ptr
			);

			// Level 2 Dictionary
			FCM::AutoPtr<FCM::IFCMDictionary> category;
			result = plugin->AddLevel(
				(const FCM::StringRep8)Application::kApplicationCategoryKey_Publisher,
				category.m_Ptr
			);

			// Level 3 Dictionary
			result = category->Add(
				(const FCM::StringRep8)Application::kApplicationCategoryKey_Name,
				FCMDictRecTypeID::StringRep8,
				(FCM::PVoid)module_info.name,
				(FCM::U_Int32)std::strlen(module_info.name) + 1);

			result = category->Add(
				(const FCM::StringRep8)Application::kApplicationCategoryKey_UniversalName,
				FCMDictRecTypeID::StringRep8,
				(FCM::PVoid)module_info.universal_name,
				(FCM::U_Int32)std::strlen(module_info.universal_name) + 1);

			result = category->Add(
				(const FCM::StringRep8)Application::kApplicationPublisherKey_UI,
				FCMDictRecTypeID::StringRep8,
				(FCM::PVoid)module_info.publisher_ui_id,
				(FCM::U_Int32)std::strlen(module_info.publisher_ui_id) + 1);

			FCM::AutoPtr<FCM::IFCMDictionary> pDocs;
			result = category->AddLevel((const FCM::StringRep8)Application::kApplicationPublisherKey_TargetDocs, pDocs.m_Ptr);

			// Level 4 Dictionary
			std::string emptyString = ""; // TODO: ???
			result = pDocs->Add(
				((std::string)publisher_id).c_str(),
				FCMDictRecTypeID::StringRep8,
				(FCM::PVoid)emptyString.c_str(),
				(FCM::U_Int32)emptyString.length() + 1
			);
		}

		return result;
	}
}