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
 * @file  IGroup.h
 *
 * @brief This file contains the interface for IGroup. This represents
 *        a "Group" of frame display elements.
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
             * @brief Defines the Interface ID (which is universally unique) for
             *        IGroup.
             *
             * @note  Textual Representation: {E43E72E4-B425-46A4-9123-F3363AA59A37}
             */
            FCM::ConstFCMIID IID_IGROUP =
            { 0xe43e72e4, 0xb425, 0x46a4, {0x91, 0x23, 0xf3, 0x36, 0x3a, 0xa5, 0x9a, 0x37} };
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace FrameElement
        {
            /**
             * @class IGroup
             *
             * @brief  Defines an interface that represents a Group.
             */
            class IGroup : public FCM::VirtualClass<IID_IGROUP>
            {
            public:
                /**
                 * @brief  This function returns an array of FrameDisplayElements in Group.
                 *         The raw shapes in the group will be returned as part of
                 *         an array of FrameDisplayElements.
                 *
                 * @param  pMembers (OUT)
                 *         List of FrameDisplayElements. Each item in the list can be queried
                 *         for IFrameDisplayElement.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 *
                 * @see    DOM::FrameElement::IFrameDisplayElement
                 */
                virtual FCM::Result _FCMCALL GetMembers(FCM::PIFCMList& pMembers) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"