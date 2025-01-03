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
 * @file  IEdge.h
 *
 * @brief This file contains the interface for IEdge. IEdge interface 
 *        represents an edge of a shape.
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
                 *        IEdge.
                 *
                 * @note  Textual Representation: {4EB8864F-1901-4A59-8690-3EBE8000C280}
                 */
                FCM::ConstFCMIID IID_IEDGE =
                { 0x4eb8864f, 0x1901, 0x4a59, {0x86, 0x90, 0x3e, 0xbe, 0x80, 0x0, 0xc2, 0x80} };
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
                 * @class IEdge
                 *
                 * @brief Defines an interface that represents an edge of a shape.
                 */
                class IEdge : public FCM::VirtualClass<IID_IEDGE>
                {
                public:
                    /**
                     * @brief  This function returns the segment corresponding to the edge.
                     *
                     * @param  segment (OUT)
                     *         Segment
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     *
                     * @note   Caller of this function must set segment.structSize to
                     *         sizeof(SEGMENT).
                     */
                    virtual FCM::Result _FCMCALL GetSegment(DOM::Utils::SEGMENT& segment) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"