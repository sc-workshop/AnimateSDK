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
 * @file  ITweenerService.h
 *
 * @brief This file contains interface for ITweenerService. ITweenerService 
 *        represents a service that provides a tweener for a given property. 
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
            namespace Tween
            {

                /**
                 * @brief Defines the universally-unique interface ID for
                 *        ITweenerService.
                 *
                 * @note  Textual Representation: {DB5ED0C2-01D0-4DA0-907C-99963343138A}
                 */
                FCM::ConstFCMIID IID_ITWEENER_SERVICE =
                { 0xdb5ed0c2, 0x1d0, 0x4da0, {0x90, 0x7c, 0x99, 0x96, 0x33, 0x43, 0x13, 0x8a} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace Tween
            {
                /**
                 * @class ITweenerService
                 *
                 * @brief This describes the tweener service.
                 */
                class ITweenerService : public FCM::VirtualClass<IID_ITWEENER_SERVICE>
                {
                public:
                    /**
                     * @brief  Returns a tweener for a given property.
                     *
                     * @param  propId (IN)
                     *         Property identifier
                     *
                     * @param  reserved (IN)
                     *         Reserved for future use
                     *
                     * @param  pTweener (OUT)
                     *         A tweener object is returned. This can queried for one of the following:
                     *         IShapeTweener, IGeometricTweener, IFilterTweener and IColorTweener.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::Service::Tween::IGeometricTweener
                     *
                     * @see    DOM::Service::Tween::IFilterTweener
                     *
                     * @see    DOM::Service::Tween::IColorTweener
                     *
                     * @see    DOM::Service::Tween::IShapeTweener
                     */
                    virtual FCM::Result _FCMCALL GetTweener(
                        const FCM::FCMGUID& propId,
                        void* reserved,
                        FCM::PIFCMUnknown& pTweener) = 0;

                };
            }
        }
    }
}


#include "animate/core/common/FCMPostConfig.h"