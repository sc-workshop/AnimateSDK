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
 * @file  ITextRun.h
 *
 * @brief This file contains the interface for ITextRun. ITextRun interface 
 *        represents a string of characters that have the same text style. 
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
        namespace FrameElement
        {
            /**
             * @brief Defines the universally-unique interface ID for
             *        ITextRun.
             *
             * @note  Textual Representation: {72330363-CBBB-4635-BDCC-04BAC5837871}
             */
            FCM::ConstFCMIID IID_ITEXT_RUN =
            { 0x72330363, 0xcbbb, 0x4635, {0xbd, 0xcc, 0x4, 0xba, 0xc5, 0x83, 0x78, 0x71} };
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace FrameElement
        {
            /**
             * @class ITextRun
             *
             * @brief Defines an interface that represents a string of characters with the common
             *        text attributes.
             */
            class ITextRun : public FCM::VirtualClass<IID_ITEXT_RUN>
            {
            public:
                /**
                 * @brief  This function returns the index of the first character within the Text
                 *         element where this textrun starts.
                 *
                 * @param  startIndex (OUT)
                 *         The index of first character within the Text element where this text-run
                 *         starts.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetStartIndex(FCM::U_Int32& startIndex) = 0;


                /**
                 * @brief  This function returns the length of the text run in context.
                 *
                 * @param  length (OUT)
                 *         Length of the text run in context.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetLength(FCM::U_Int32& length) = 0;


                /**
                 * @brief  This function returns text style for the textrun.
                 *
                 * @param  pTextStyle (OUT)
                 *         The text style for the textrun is returned.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetTextStyle(PITextStyle& pTextStyle) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"