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
  * @file  IFrame1.h
  *
  * @brief This file contains the interface for IFrame1. The IFrame1
  *        interface represents a frame in the layer of a timeline.
  */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
	/* -------------------------------------------------- Macros / Constants */
	namespace DOM
	{
		/**
		 * @brief Defines the universally-unique interface ID for
		 *        IFrame1.
		 *
		 * @note  Textual Representation: {6AA01C45-7BAB-46FA-B199-46AEDA6456D4}
		 */
		FCM::ConstFCMIID IID_IFRAME1 =
		{ 0x6AA01C45, 0x7BAB, 0x46FA,{ 0xB1, 0x99, 0x46, 0xAE, 0xDA, 0x64, 0x56, 0xD4 } };
	}

	/* -------------------------------------------------- Class Declaration */
	namespace DOM
	{
		/**
		 * @class IFrame1
		 *
		 * @brief Defines the interface that represents a frame in a layer in the
		 *        timeline.
		 */
		class IFrame1 : public FCM::ChildrenVirtualClass<IID_IFRAME1, IFrame>
		{
		public:
			/**
			 * gets the rig matrix of the frame
			 */
			virtual FCM::Result _FCMCALL GetRigProperties(FCM::U_Int32& frameIndex, DOM::Utils::RIG_PROPERTIES& prop) = 0;

			/**
			 * gets the rig parent of the frame
			 */
			virtual FCM::Result _FCMCALL GetRigParent(FCM::U_Int32& frameIndex, PILayer& pLayer) = 0;

		};
	}
}

#include "animate/core/common/FCMPostConfig.h"