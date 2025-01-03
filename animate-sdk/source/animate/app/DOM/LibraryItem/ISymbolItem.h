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
 * @file  ISymbolItem.h
 *
 * @brief This file contains interface for ISymbolItem. 
 *        This represents a symbol item in Library.
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
             * @brief Defines the Interface ID (which is universally unique) for
             *        ISymbolItem.
             *
             * @note  Textual Representation: {596975D9-32F0-412D-8332-C5270CAF2CF3}
             */
            FCM::ConstFCMIID IID_ISYMBOL_ITEM =
            { 0x596975d9, 0x32f0, 0x412d, {0x83, 0x32, 0xc5, 0x27, 0xc, 0xaf, 0x2c, 0xf3} };
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace LibraryItem
        {
            /**
            * @class ISymbolItem
            *
            * @brief This interface represents a symbol item.
            */
            class ISymbolItem : public FCM::VirtualClass<IID_ISYMBOL_ITEM>
            {
            public:
                /**
                 * @brief  This function returns timeline.
                 *
                 * @param  pTimeline (OUT)
                 *         Timeline object.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetTimeLine(PITimeline& pTimeline) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"