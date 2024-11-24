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
* @file  ILayer1.h
*
* @brief This file contains the interface for ILayer. The ILayer
*        interface represents a layer in the timeline.
*/
#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"

#include "ILayer.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
	/* -------------------------------------------------- Macros / Constants */
	namespace DOM
	{
		namespace Layer
		{
			/**
			* @brief Defines the universally-unique interface ID for
			*        ILayer1.
			*
			* @note  Textual Representation: {F7EAE5BD-56F2-476C-9053-9F2209821AFC}
			*/
			FCM::ConstFCMIID IID_ILAYER1 =
			{ 0xf7eae5bd, 0x56f2, 0x476c,{ 0x90, 0x53, 0x9f, 0x22, 0x9, 0x82, 0x1a, 0xfc } };


		}
	}

	/* -------------------------------------------------- Class Declaration */
	namespace DOM
	{
		/**
		* @class ILayer1
		*
		* @brief Defines the interface that represents a layer in the timeline.
		*/
		class ILayer1 : public FCM::ChildrenVirtualClass< Layer::IID_ILAYER1, ILayer>
		{
		public:
			/**
			* @brief  gets Zdepth at frame of layer.
			*
			*/
			virtual FCM::Result _FCMCALL GetZDepthAtFrame(FCM::U_Int32 frameIndex, FCM::Long& zDepth) = 0;

			/**
			* @brief  gets Texture Layer type which can be None, Cylinder or Sphere.
			*
			*/
			virtual FCM::Result _FCMCALL GetTextureLayerType(FCM::TextureMappingType& textureMappingType) = 0;

			/**
			* @brief  gets a boolean value, which is true if layer is attached to camera.
			*
			*/
			virtual FCM::Result _FCMCALL IsLayerAttachedToCamera(FCM::Boolean& layerAttachedToCamera) = 0;

		};

	}
}

#include "animate/core/common/FCMPostConfig.h"