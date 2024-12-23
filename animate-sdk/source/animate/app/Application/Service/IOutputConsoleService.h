/******************************************************************************
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright [2014] Adobe Systems Incorporated
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
  * @file  IOutputConsoleService.h
  *
  * @brief This file contains the interface for IOutputConsoleService. This service
  *        can be used to trace messages to the 'Adobe Animate CC' output panel.
  */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
	/* -------------------------------------------------- Enums */

	/* -------------------------------------------------- Macros / Constants */

	namespace Application
	{
		namespace Service
		{
			/**
			 * @brief Defines the universally-unique Interface ID for IOutputConsoleService.
			 *
			 * @note  Textual Representation: {C02BC56E-BC02-4814-B697-8190D58D2022}
			 */
			FCM::ConstFCMIID IID_IOUTPUT_CONSOLE_SERVICE =
			{ 0xc02bc56e, 0xbc02, 0x4814, {0xb6, 0x97, 0x81, 0x90, 0xd5, 0x8d, 0x20, 0x22} };
		}
	}

	/* -------------------------------------------------- Structs / Unions */

	/* -------------------------------------------------- Class Declaration */

	namespace Application
	{
		namespace Service
		{
			/**
			 * @class IOutputConsoleService
			 *
			 * @brief IOutputConsoleService defines an interface to output messages to the
			 *        'Adobe Animate CC' output console.
			 */
			class IOutputConsoleService : public FCM::VirtualClass<IID_IOUTPUT_CONSOLE_SERVICE>
			{
			public:
				/**
				 * @brief Traces a message to the output console.
				 *
				 * @param pMessage (IN)
				 *        The message to be traced.
				 *
				 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
				 */
				virtual FCM::Result _FCMCALL Trace(FCM::CStringRep16 pMessage) = 0;

				/**
				 * @brief Clears the output console.
				 *
				 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
				 */
				virtual FCM::Result _FCMCALL Clear() = 0;
			};
		}
	};

	/* -------------------------------------------------- Inline / Functions */
}
#include "animate/core/common/FCMPostConfig.h"