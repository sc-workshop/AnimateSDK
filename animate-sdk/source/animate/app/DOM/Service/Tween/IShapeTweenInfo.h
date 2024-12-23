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
 * @file  IShapeTweenInfo.h
 *
 * @brief This file contains interface for IShapeTweenInfo. IShapeTweenInfo
 *        represents a shape tween information. 
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
                 *        IShapeTweenInfo.
                 *
                 * @note  Textual Representation: {3105E118-297E-4BDC-8055-6FB74BB501C1}
                 */
                FCM::ConstFCMIID IID_ISHAPE_TWEEN_INFO =
                { 0x3105e118, 0x297e, 0x4bdc, {0x80, 0x55, 0x6f, 0xb7, 0x4b, 0xb5, 0x1, 0xc1} };
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
                 * @class IShapeTweenInfo
                 *
                 * @brief Defines an interface that provides information about a shape tween.
                 */
                class IShapeTweenInfo : public FCM::VirtualClass<IID_ISHAPE_TWEEN_INFO>
                {
                public:
                    /*
                    * Returns a shape pair - Starting shape and ending shape (IShape)
                    */
                    virtual FCM::Result _FCMCALL GetShapePair(FCM::PIFCMPair& pShapePair) = 0;

                    /* Returns a shape hint object. Right now shape hint is a marker interface
                    * and can be used to know if the shape tween contains hint information.
                    */
                    virtual FCM::Result _FCMCALL GetShapeHint(PIShapeHintInfo& pShapeHint) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"