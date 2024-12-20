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
 * @file  ISymbolInstance.h
 *
 * @brief This file contains the interface for ISymbolInstance. ISymbolInstance
 *        interface represents a symbol on the stage.
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
             * @brief Defines the universally-unique interface ID for
             *        ISymbolInstance.
             *
             * @note  Textual Representation: {B1F1A4AA-BB67-4BB7-BBAF-95FF67408E42}
             */
            FCM::ConstFCMIID IID_ISYMBOL_INSTANCE =
            { 0xb1f1a4aa, 0xbb67, 0x4bb7, {0xbb, 0xaf, 0x95, 0xff, 0x67, 0x40, 0x8e, 0x42} };
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace FrameElement
        {
            /**
             * @class ISymbolInstance
             *
             * @brief Defines an interface that represents an instance of a symbol
             *        present on the stage.
             */
            class ISymbolInstance : public FCM::VirtualClass<IID_ISYMBOL_INSTANCE>
            {
            public:
                /**
                 * @brief  This functions returns color effects (in form of a color matrix)
                 *          applied to the symbol instance.
                 *
                 * @param  colorMatrix (OUT)
                 *         Color Matrix representing different color effects.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetColorMatrix(Utils::COLOR_MATRIX& colorMatrix) = 0;


                /**
                 * @brief  This function is used get the registration point of the symbol.
                 *
                 * @param  registrationPoint (OUT)
                 *         Point that defines the X and Y coordinates of the registration point.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetRegistrationPoint(Utils::POINT2D& registrationPoint) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"