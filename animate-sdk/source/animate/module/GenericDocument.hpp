#pragma once

#include <string>

#include "AnimateCore.h"

#include "animate/app/DocType/IDocType.h"
#include "animate/app/DocType/IFeatureMatrix.h"
#include "Types.hpp"

namespace Animate::DocType
{
	using namespace FCM;
	template<std::uintptr_t CLS_MATRIX_ID>
	class GenericDocumentType : public IDocType, public FCM::FCMObjectBase
	{
	public:
		BEGIN_INTERFACE_MAP(GenericDocumentType)
			INTERFACE_ENTRY(IDocType)
		END_INTERFACE_MAP

	public:
		virtual FCM::Result _FCMCALL GetFeatureMatrix(PIFeatureMatrix& featureMatrix)
		{
			FCM::Result res = FCM_SUCCESS;

			if (m_features == NULL)
			{
				res = GetCallback()->CreateInstance(
					NULL,
					*((ConstFCMCLSID*)CLS_MATRIX_ID),
					ID_IFEATURE_MATRIX,
					(FCM::PPVoid)&m_features.m_Ptr);
			}

			featureMatrix = m_features;
			return res;
		}

		GenericDocumentType();

		virtual ~GenericDocumentType();

	private:
		FCM::AutoPtr<DocType::FeatureMatrix> m_features;
	};

	static FCM::Result RegisterDocument(
			FCM::PIFCMDictionary plugins,
			FCM::ConstFCMCLSID docTypeID,
			const ModuleInfo& module
	)
	{
		FCM::Result res = FCM_SUCCESS;

		/*
		 * Dictionary structure for a DocType plugin is as follows:
		 *
		 *  Level 0 :
		 *              --------------------------------
		 *             | Application.Component |  ----- | -----------------------------
		 *              --------------------------------                               |
		 *                                                                             |
		 *  Level 1:                                   <-------------------------------
		 *              -----------------------------
		 *             | CLSID_DocType_GUID |  ----- | --------------------------------
		 *              -----------------------------                                  |
		 *                                                                             |
		 *  Level 2:                                                <------------------
		 *              -------------------------------------------------
		 *             | Application.Component.Category.DocType |  ----- |-------------
		 *              -------------------------------------------------              |
		 *                                                                             |
		 *  Level 3:                                                     <-------------
		 *              -----------------------------------------------------------------------
		 *             | Application.Component.Category.Name          |  DOCTYPE_NAME          |
		 *              -----------------------------------------------------------------------
		 *             | Application.Component.Category.UniversalName |  DOCTYPE_UNIVERSAL_NAME|
		 *              -----------------------------------------------------------------------
		 *             | Application.Component.DocType.Desc           |  DOCTYPE_DESCRIPTION   |
		 *              -----------------------------------------------------------------------
		 *
		 *  Note that before calling this function the level 0 dictionary has already
		 *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
		 */

		{
			// Level 1 Dictionary
			FCM::AutoPtr<FCM::IFCMDictionary> plugin;
			res = plugins->AddLevel(
				(const FCM::StringRep8)(((std::string)docTypeID).c_str()),
				&plugin.m_Ptr);

			{
				// Level 2 Dictionary
				FCM::AutoPtr<FCM::IFCMDictionary> category;
				res = plugin->AddLevel((const FCM::StringRep8)Application::kApplicationCategoryKey_DocType, &category.m_Ptr);

				{
					// Level 3 Dictionary

					// Add short name - Used in the "New Document Dialog" / "Start Page".
					res = category->Add(
						(const FCM::StringRep8)Application::kApplicationCategoryKey_Name,
						FCMDictRecTypeID::StringRep8,
						(FCM::PVoid)module.name.c_str(),
						(FCM::U_Int32)module.name.length() + 1
					);

					// Add universal name - Used to refer to it from JSFL and used in error messages
					res = category->Add(
						(const FCM::StringRep8)Application::kApplicationCategoryKey_UniversalName,
						FCMDictRecTypeID::StringRep8,
						(FCM::PVoid)module.universal_name.c_str(),
						(FCM::U_Int32)module.universal_name.length() + 1
					);

					// Add plugin description - Appears in the "New Document Dialog"
					// Plugin description can be localized depending on the LanguageCode.
					res = category->Add(
						(const FCM::StringRep8)Application::kApplicationDocTypeKey_Desc,
						FCMDictRecTypeID::StringRep8,
						(FCM::PVoid)module.description.c_str(),
						(FCM::U_Int32)module.description.length() + 1
					);
				}
			}
		}

		return res;
	}
}