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
 * @file  IMediaItem.h
 *
 * @brief This file contains interface for IMediaItem.
 *        This represents a media symbol in Library.
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
		namespace LibraryItem
		{
			/**
			 * @brief Defines the universally-unique interface ID for
			 *        IMediaItem.
			 *
			 * @note  Textual Representation: {2AD8BEF2-C2B5-4417-9967-8D95B33AC5CD}
			 */
			FCM::ConstFCMIID IID_IMEDIA_ITEM =
			{ 0x2ad8bef2, 0xc2b5, 0x4417, {0x99, 0x67, 0x8d, 0x95, 0xb3, 0x3a, 0xc5, 0xcd} };
		}
	}


	/* -------------------------------------------------- Class Declaration */
	namespace DOM
	{
		namespace LibraryItem
		{
			/**
			 * @class IMediaItem
			 *
			 * @brief Defines the interface for a media resource in library (Bitmap or Audio).
			 */
			class IMediaItem : public FCM::VirtualClass<IID_IMEDIA_ITEM>
			{
			public:
				/**
				 * @brief  Returns the media specific information
				 *
				 * @param  pMediaInfo (OUT)
				 *         Media information for bitmap (IBitmapInfo) or sound(ISoundInfo).
				 *
				 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
				 *
				 * @see    DOM::MediaInfo::IBitmapInfo
				 *
				 * @see    DOM::MediaInfo::ISoundInfo
				 */
				virtual FCM::Result _FCMCALL GetMediaInfo(FCM::PIFCMUnknown& pMediaInfo) = 0;

				virtual FCM::Result _FCMCALL SetCMediaBits(void* bits) = 0;

			};
		}
	}
}

#include "animate/core/common/FCMPostConfig.h"