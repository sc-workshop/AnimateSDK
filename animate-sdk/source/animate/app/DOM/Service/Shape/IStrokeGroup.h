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
 * @file  IStrokeGroup.h
 *
 * @brief This file contains interface for IStrokeGroup. IStrokeGroup represents
 *        a group of strokes that have the same stroke style.
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
            namespace Shape
            {
                /**
                 * @brief Defines the universally-unique interface ID for
                 *        IStrokeGroup.
                 *
                 * @note  Textual Representation:  {56820F87-EF8A-485B-B27A-B484E19FC762}
                 */
                FCM::ConstFCMIID IID_ISTROKE_GROUP =
                { 0x56820f87, 0xef8a, 0x485b, {0xb2, 0x7a, 0xb4, 0x84, 0xe1, 0x9f, 0xc7, 0x62} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace Shape
            {
                /**
                 * @class IStrokeGroup
                 *
                 * @brief Defines a stroke group that represents a group of strokes that have the same
                 *        stroke style.
                 */
                class IStrokeGroup : public FCM::VirtualClass<IID_ISTROKE_GROUP>
                {
                public:
                    /**
                     * @brief  This functions gets the stroke style of the group of edges.
                     *
                     * @param  pStrokeStyle (OUT)
                     *         Stroke style of the stroke group. This can be queried for the following
                     *         interfaces: IDashedStrokeStyle, IDottedStrokeStyle, IHatchedStrokeStyle,
                     *         IRaggedStrokeStyle, ISolidStrokeStyle and IStippleStrokeStyle.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::StrokeStyle::IDashedStrokeStyle
                     *
                     * @see    DOM::StrokeStyle::IDottedStrokeStyle
                     *
                     * @see    DOM::StrokeStyle::IHatchedStrokeStyle
                     *
                     * @see    DOM::StrokeStyle::IRaggedStrokeStyle
                     *
                     * @see    DOM::StrokeStyle::ISolidStrokeStyle
                     *
                     * @see    DOM::StrokeStyle::IStippleStrokeStyle
                     */
                    virtual FCM::Result _FCMCALL GetStrokeStyle(FCM::PIFCMUnknown& pStrokeStyle) = 0;


                    /**
                     * @brief  This function returns a stroke list. The list is ordered as per their rendering
                     *         order.
                     *
                     * @param  pPathList
                     *         Path - List of paths (IPath) having the same stroke style. This list is ordered
                     *         as per the rendering order with the first in the list being the one
                     *         that needs to be rendered first.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::Service::Shape::IPath
                     */
                    virtual FCM::Result _FCMCALL GetPaths(FCM::PIFCMList& pPathList) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"