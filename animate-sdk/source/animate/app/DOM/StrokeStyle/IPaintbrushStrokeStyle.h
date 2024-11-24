/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright 2015 Adobe Systems Incorporated
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
 * @file  IPaintBrushStrokeStyle.h
 *
 * @brief This file contains the interface for IPaintBrushStrokeStyle. This interface
 *        represents a "paint brush" stroke style of an edge.
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
        namespace StrokeStyle
        {
            /**
             * @brief Defines the universally-unique interface ID for
             *        IPaintBrushStrokeStyle.
             *
             * @note  Textual Representation: {937070AC-CCAE-4EA8-BFDF-7924AC37057B}
             */
            FCM::ConstFCMIID IID_IPAINTBRUSH_STROKE_STYLE =
            { 0x937070ac, 0xccae, 0x4ea8, {0xbf, 0xdf, 0x79, 0x24, 0xac, 0x37, 0x05, 0x7b} };
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace StrokeStyle
        {
            /**
             * @class IPaintBrushStrokeStyle
             *
             * @brief Marker interface that represents a "paint brush" stroke style of an edge.
             */
            class IPaintBrushStrokeStyle : public FCM::VirtualClass<IID_IPAINTBRUSH_STROKE_STYLE>
            {
            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"