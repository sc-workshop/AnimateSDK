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
 * @file  IGlyph.h
 *
 * @brief This file contains interface for IGlyph. IGlyph  
 *        contains methods to obtain information about a glyph.
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
            namespace FontTable
            {
                /**
                 * @brief Defines the universally-unique interface ID for
                 *        IGlyph.
                 *
                 * @note  Textual Representation: {CC4BCE73-F455-4010-AE92-17CF8B6E1205}
                 */
                FCM::ConstFCMIID IID_IGLYPH =
                { 0xcc4bce73, 0xf455, 0x4010, {0xae, 0x92, 0x17, 0xcf, 0x8b, 0x6e, 0x12, 0x5} };
            }
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace FontTable
            {
                /**
                * @class IGlyph
                *
                * @brief Defines the interface that defines a glyph.
                */
                class IGlyph : public FCM::VirtualClass<IID_IGLYPH>
                {
                public:
                    /**
                     * @brief  This function returns the char code for the Glyph.
                     *
                     * @param  charCode (OUT)
                     *         the char code for the Glyph is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL GetCharCode(FCM::U_Int16& charCode) = 0;


                    /**
                     * @brief  This function returns the advance value for the Glyph.
                     *
                     * @param  advance (OUT)
                     *         The advance value for the Glyph is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL GetAdvance(FCM::Double& advance) = 0;


                    /**
                     * @brief  This function returns Outline of the Glyph as a IShape object.
                     *
                     * @param  pShape (OUT)
                     *         Outline of the Glyph is returned as a IShape object.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL GetOutline(FrameElement::PIShape& pShape) = 0;
                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"