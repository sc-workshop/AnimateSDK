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
 * @file  IKerningPair.h
 *
 * @brief This file contains interface for IKerningPair. IKerningPair  
 *        contains methods to obtain KerningPair chars and their corresponding 
 *        kerning amount.
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
                 *        IKerningPair.
                 *
                 * @note  Textual Representation: {7B64D242-D838-4BF8-A14A-C0FA2930D911}
                 */
                FCM::ConstFCMIID IID_IKERNING_PAIR =
                { 0x7b64d242, 0xd838, 0x4bf8, {0xa1, 0x4a, 0xc0, 0xfa, 0x29, 0x30, 0xd9, 0x11} };
            }
        }
    }


    /* -------------------------------------------------- Structs / Unions */
    namespace DOM
    {
        namespace Service
        {
            namespace FontTable
            {
                /**
                 * @struct KERNING_PAIR
                 *
                 * @brief  Defines a kerning pair and includes the kerning amount for the pair.
                 */
                struct KERNING_PAIR
                {
                    /** first char code */
                    FCM::U_Int16 first;


                    /** second char code */
                    FCM::U_Int16 second;


                    /** Kerning amount between the char pair */
                    FCM::Double kernAmount;
                };
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
                * @class IKerningPair
                *
                * @brief Defines the interface to obtain KerningPair char codes and the Kerning
                *        amount.
                */
                class IKerningPair : public FCM::VirtualClass<IID_IKERNING_PAIR>
                {
                public:
                    /**
                     * @brief  This function returns the kerningPair Object that contains KerningPair
                     *         char codes and the Kerning amount.
                     *
                     * @param  kerningPair (OUT)
                     *         kerningPair object is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL GetKerningPair(KERNING_PAIR& kerningPair) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"