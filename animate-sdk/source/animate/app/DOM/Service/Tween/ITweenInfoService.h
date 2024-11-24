/*************************************************************************
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2018 Adobe Systems Incorporated
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Adobe Systems Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Adobe Systems Incorporated and its
 * suppliers and are protected by all applicable intellectual property
 * laws, including trade secret and copyright laws.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 **************************************************************************/

/**
 * @file  ITweenInfoService.h
 *
 * @brief This file contains interface for ITweenInfoService. ITweenInfoService
 *        represents a service that provides tween information for a frame, display element or a shape.
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
                 *        ITweenInfoService.
                 *
                 * @note  Textual Representation: {44bf6a6f-9b5b-438d-8594-bb1089723175}

                 */
                FCM::ConstFCMIID IID_ITWEEN_INFO_SERVICE =
                { 0x44bf6a6f, 0x9b5b, 0x438d, {0x85, 0x94, 0xbb, 0x10, 0x89, 0x72, 0x31, 0x75} };
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
                 * @class ITweenInfoService
                 *
                 * @brief This service provides tween information for a property.
                 */
                class ITweenInfoService : public FCM::VirtualClass<IID_ITWEEN_INFO_SERVICE>
                {
                public:
                    /**
                     * @brief  Returns tween information for a frame.
                     *
                     * @param  pFrame (IN)
                     *         The frame of which the information needs to be retrieved
                     *
                     * @param  pTweenInfoList (OUT)
                     *         A tween information list is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     */
                    virtual FCM::Result _FCMCALL GetFrameTweenInfo(DOM::PIFrame pFrame, FCM::PIFCMList& pTweenInfoList) = 0;

                    /**
                     * @brief  Returns tween information for an element.
                     *
                     * @param  pElement (IN)
                     *         The element of which the information needs to be retrieved
                     *
                     * @param  pTweenInfoList (OUT)
                     *         A tween information list is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     */
                    virtual FCM::Result _FCMCALL GetElementTweenInfo(FCM::PIFCMCallback pCallback, DOM::FrameElement::PIFrameDisplayElement pElement, FCM::PIFCMList& pTweenInfoList) = 0;

                    /**
                     * @brief  Returns shape tween information for shapes in a frame.
                     *
                     * @param  pFrame (IN)
                     *         The frame of which the information needs to be retrieved
                     *
                     * @param  pTweenInfo (OUT)
                     *         A tween information dictionary is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     */
                    virtual FCM::Result _FCMCALL GetShapeTweenInfo(DOM::PIFrame pFrame, FCM::PIFCMDictionary& pTweenInfo) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"