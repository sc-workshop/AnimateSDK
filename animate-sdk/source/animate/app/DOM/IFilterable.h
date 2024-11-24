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
 * @file  IFilterable.h
 *
 * @brief This file contains the interface for IFilterable. This IFilterable
 *        represents the list of graphical filters.
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
        /**
         * @brief Defines the universally-unique interface ID for 
         *        IFilterable.
         *
         * @note  Textual Representation: {08F582AC-85DD-4653-9DF4-25C9A6C869B7}
         */
        FCM::ConstFCMIID IID_IFILTERABLE =
            {0x8f582ac, 0x85dd, 0x4653, {0x9d, 0xf4, 0x25, 0xc9, 0xa6, 0xc8, 0x69, 0xb7}};
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM 
    {
        /**
        * @class IFilterable
        *
        * @brief This interface represents the list of graphic filters.
        */
        class IFilterable : public FCM::VirtualClass<IID_IFILTERABLE>
        {
        public:
            /**
             * @brief  This function gets the list of filters applied to
             *         the object in context.
             *
             * @param  pGraphicFilterList (OUT)
             *         List of filters.
             *
             * @return On success, FCM_SUCCESS is returned, else an error code is returned.
             */
            virtual FCM::Result _FCMCALL GetGraphicFilters(FCM::PIFCMList& pGraphicFilterList) = 0;

        };
    }
}


/* -------------------------------------------------- Inline / Functions */

#include "animate/core/common/FCMPostConfig.h"