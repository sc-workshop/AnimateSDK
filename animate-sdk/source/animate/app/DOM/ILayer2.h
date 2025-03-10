/******************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2018] Adobe Systems Incorporated
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
* @file  ILayer2.h
*
* @brief This file contains the interface for ILayer. The ILayer
*        interface represents a layer in the timeline.
*/

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"

#include "ILayer1.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
	namespace DOM
	{
		namespace Layer
		{
			/**
			* @brief Defines the universally-unique interface ID for
			*        ILayer2.
			*
			* @note  Textual Representation: {537FD854-358E-4F46-A3E3-C7D0048FD263}
			*/
			FCM::ConstFCMIID IID_ILAYER2 =
			{ 0x537FD854, 0x358E, 0x4F46,{ 0xA3, 0xE3, 0xC7, 0xD0, 0x04, 0x8F, 0xD2, 0x63 } };


		}
	}

	namespace DOM
	{
		/**
		* @class ILayer2
		*
		* @brief Defines the interface that represents a layer in the timeline.
		*/
		class ILayer2 : public FCM::ChildrenVirtualClass< Layer::IID_ILAYER2, ILayer1>
		{
		public:
			/**
			* @brief  gets filters at frame of layer.
			*
			*/
			virtual FCM::Result _FCMCALL GetFiltersAtFrame(FCM::U_Int32 frameIndex, FCM::PIFCMList& pFilterList) = 0;
			/**
			* @brief  gets color transform at frame of layer.
			*
			*/
			virtual FCM::Result _FCMCALL GetColorTransformAtFrame(FCM::U_Int32 frameIndex, Utils::COLOR_MATRIX& colorMatrix) = 0;
			/**
			* @brief  gets blend mode at frame of layer.
			*
			*/
			virtual FCM::Result _FCMCALL GetBlendModeAtFrame(FCM::U_Int32 frameIndex, FCM::BlendMode& blendMode) = 0;

		};
	}
}

#include "animate/core/common/FCMPostConfig.h"