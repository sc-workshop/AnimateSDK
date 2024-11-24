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
 * @file  IBitmapInfo.h
 *
 * @brief This file contains the definition for IBitmapInfo interface. This 
 *        interface represents the information related to a bitmap item.
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
        namespace MediaInfo
        {

            /**
             * @brief Defines the universally-unique interface ID for
             *        IBitmapInfo.
             *
             * @note  Textual Representation: {20194070-7A5C-4627-ADF1-0A6754D8AC84}
             */
            FCM::ConstFCMIID IID_IBITMAP_INFO =
            { 0x20194070, 0x7a5c, 0x4627, {0xad, 0xf1, 0xa, 0x67, 0x54, 0xd8, 0xac, 0x84} };
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace MediaInfo
        {
            /**
             * @class IBitmapInfo
             *
             * @brief This interface represents bitmap attributes.
             */
            class IBitmapInfo : public FCM::VirtualClass<IID_IBITMAP_INFO>
            {
            public:
                /**
                 * @brief  Returns the bitmap width.
                 *
                 * @param  width (OUT)
                 *         The width of the bitmap.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetWidth(FCM::S_Int32& width) = 0;


                /**
                 * @brief  Returns the bitmap height.
                 *
                 * @param  height (OUT)
                 *         The height of the bitmap.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetHeight(FCM::S_Int32& height) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"