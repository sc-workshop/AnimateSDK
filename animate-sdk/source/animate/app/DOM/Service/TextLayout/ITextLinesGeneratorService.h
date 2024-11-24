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
 * @file  ITextLinesGeneratorService.h
 *
 * @brief This file contains the interface for ITextLinesGeneratorService. 
 *        ITextLinesGeneratorService contains methods that fetch text lines from the 
 *        text element.
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
        namespace Service
        {
            namespace TextLayout
            {
                /**
                 * @brief Defines the universally-unique Interface ID for
                 *        ITextLinesGeneratorService.
                 *
                 * @note  Textual Representation: {30D8005F-7046-4E66-A437-4FCD6A78DED5}
                 */
                FCM::ConstFCMIID IID_ITEXT_LINES_GENERATOR_SERVICE =
                { 0x30d8005f, 0x7046, 0x4e66, {0xa4, 0x37, 0x4f, 0xcd, 0x6a, 0x78, 0xde, 0xd5} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace TextLayout
            {
                /**
                 * @class ITextLinesGeneratorService
                 *
                 * @brief Defines the interface to generate text lines for a text element.
                 */
                class ITextLinesGeneratorService : public FCM::VirtualClass<IID_ITEXT_LINES_GENERATOR_SERVICE>
                {
                public:
                    /**
                     * @brief  This function returns the list of text lines present in the text
                     *         element.
                     *
                     * @param  pClassicText (IN)
                     *         The text element from which text lines are to be fetched.
                     *
                     * @param  pTextLineList (OUT)
                     *         The list of text lines (ITextLine) present in the text element.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::Service::TextLayout::ITextLine
                     */
                    virtual FCM::Result _FCMCALL GetTextLines(
                        const FrameElement::PIClassicText& pClassicText,
                        FCM::PIFCMList& pTextLineList) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"