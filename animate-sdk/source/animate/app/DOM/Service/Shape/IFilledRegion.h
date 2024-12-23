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
 * @file  IFilledRegion.h
 *
 * @brief This file contains the interface for IFilledRegion. IFilledRegion interface 
 *        represents a closed path which is filled.
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
                 *        IFilledRegion.
                 *
                 * @note  Textual Representation: {8D1FE648-075B-4FA6-BF03-61C73E459EB3}
                 */
                FCM::ConstFCMIID IID_IFILLED_REGION =
                { 0x8d1fe648, 0x75b, 0x4fa6, {0xbf, 0x3, 0x61, 0xc7, 0x3e, 0x45, 0x9e, 0xb3} };
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
                 * @class IFilledRegion
                 *
                 * @brief Defines the interface that represents a filled region.
                 */
                class IFilledRegion : public FCM::VirtualClass<IID_IFILLED_REGION>
                {
                public:
                    /**
                     * @brief  This function gets all the edges that defines the boundary of the
                     *         filled region.
                     *
                     * @param  pPath (OUT)
                     *         Outline Region
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL GetBoundary(PIPath& pPath) = 0;


                    /**
                     * @brief  This function gets a list of all the holes contained in the region.
                     *
                     * @param  pHoleList (OUT)
                     *         Hole list (List of IPath)
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::Service::Shape::IPath
                     */
                    virtual FCM::Result _FCMCALL GetHoles(FCM::PIFCMList& pHoleList) = 0;


                    /**
                     * @brief  This functions gets the filled region style for the group of filled
                     *         regions.
                     *
                     * @param  pFillStyle (OUT)
                     *         Fill style of the filled regions. This can be queried for the following
                     *         interfaces: ISolidFillStyle, IBitmapFillStyle and IGradientFillStyle.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @see    DOM::FillStyle::ISolidFillStyle
                     *
                     * @see    DOM::FillStyle::IBitmapFillStyle
                     *
                     * @see    DOM::FillStyle::IGradientFillStyle
                     */
                    virtual FCM::Result _FCMCALL GetFillStyle(FCM::PIFCMUnknown& pFillStyle) = 0;
                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"