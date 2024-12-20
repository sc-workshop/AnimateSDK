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
 * @file  IFilterTweener.h
 *
 * @brief This file contains interface for IFilterTweener. IFilterTweener 
 *        represents a tweener for a filter. 
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
                 *        IFilterTweener.
                 *
                 * @note  Textual Representation: {2B1CB09E-F507-4BD2-A1EF-1A61E583608F}
                 */
                FCM::ConstFCMIID IID_IFILTER_TWEENER =
                { 0x2b1cb09e, 0xf507, 0x4bd2, {0xa1, 0xef, 0x1a, 0x61, 0xe5, 0x83, 0x60, 0x8f} };
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
                 * @class IFilterTweener
                 *
                 * @brief Defines an interface that represents tweening of filter.
                 *        Filter tweener can be used to obtain interpolated filter
                 *        values at discrete intervals during a motion/classic tween.
                 */
                class IFilterTweener : public FCM::VirtualClass<IID_IFILTER_TWEENER>
                {
                public:
                    /**
                     * @brief  Returns a set of filters at a specific point within the frame span
                     *         duration.
                     *
                     * @param  pTween (IN)
                     *         Tween object
                     *
                     * @param  offset (IN)
                     *         The point in the frame span for which filter values are required.
                     *         It can have values from 0 to (FrameDuration - 2).
                     *         The value at (FrameDuration - 1) is available in the next keyframe.
                     *
                     * @param  pFilterList (OUT)
                     *         This contains an ordered set of graphic filters(IGraphicFilter).
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is
                     *         returned.
                     */
                    virtual FCM::Result _FCMCALL GetFilters(
                        PITween pTween,
                        FCM::U_Int32 offset,
                        FCM::PIFCMList& pFilterList) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"