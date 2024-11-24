/*************************************************************************
*
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright 2019 Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and may be covered by U.S. and Foreign Patents,
* patents in process, and are protected by trade secret or copyright law.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

/**
 * @file  IComponentItem.h
 *
 * @brief This file contains interface for IComponentItem. 
 *        This represents a component item in Library.
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
             *        IComponentItem.
             *
             * @note  Textual Representation: {846A7370-88A4-4B67-BDD8-9587CB025391}
             */
            FCM::ConstFCMIID IID_ICOMPONENT_ITEM =
            { 0x846a7370, 0x88a4, 0x4b67,{ 0xbd, 0xd8, 0x95, 0x87, 0xcb, 0x2, 0x53, 0x91 } };

        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace LibraryItem
        {
            /**
            * @class IComponentItem
            *
            * @brief This interface represents a component item.
            */
            class IComponentItem : public FCM::VirtualClass<IID_ICOMPONENT_ITEM>
            {
            public:
                /**
                 * @brief  This function returns component info.
                 *
                 * @param  pComponentInfo (OUT)
                 *         Dictionary containing component's info.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetComponentInfo(FCM::PIFCMDictionary& pComponentInfo) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"