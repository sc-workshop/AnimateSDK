/******************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2013] Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
******************************************************************************/

/**
 * @file  IDocType.h
 *
 * @brief This file contains interface for IDocType. IDocType provides
 *        information about feature matrix through which 'Adobe Animate CC' is
 *        able to control UI and handle copy/paste of items across docs of
 *        different doctypes. The IDocType interface needs to be implemented by a
 *        "DocType" plugin.
 */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"
#include "animate/app/AppDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
	/* -------------------------------------------------- Macros/Constants */

	namespace DocType
	{
		/**
		 * @brief Defines the universally-unique Interface ID for
		 *        IDocType
		 *
		 * @note  Textual Representation:  {1667C414-52E0-438F-9BFF-7D896D90139B}
		 */
		FCM::ConstFCMIID ID_IDocType =
		{ 0x1667c414, 0x52e0, 0x438f, {0x9b, 0xff, 0x7d, 0x89, 0x6d, 0x90, 0x13, 0x9b} };
	}

	namespace DocType
	{
		/**
		 * @class IDocType
		 *
		 * @brief Defines an interface that represents the document type.
		 */
		class IDocType : public FCM::VirtualClass<ID_IDocType>
		{
			/**
			 * @brief This function must return the FeatureMatrix for the associated document type.
			 *
			 * @param pFeatureMatrix (OUT)
			 *        Feature matrix
			 *
			 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
			 */
			virtual FCM::Result _FCMCALL GetFeatureMatrix(PIFeatureMatrix& pFeatureMatrix) = 0;
		};
	}
}
#include "animate/core/common/FCMPostConfig.h"