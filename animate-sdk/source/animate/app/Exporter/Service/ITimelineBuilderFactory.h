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
 * @file  ITimelineBuilderFactory.h
 *
 * @brief This file contains interface for ITimelineBuilderFactory. 
 *        ITimelineBuilderFactory represents a factory for a timeline builder.
 */
#pragma once
#include "IFrameCommandGenerator.h"

#include "animate/core/common/FCMPreConfig.h"

/* -------------------------------------------------- Macros / Constants */
namespace Animate
{
    namespace Exporter
    {
        namespace Service
        {
            /**
             * @brief Defines the universally-unique interface ID for
             *        ITimelineBuilderFactory.
             *
             * @note  Textual Representation:  {19C111B8-283E-4CE5-A82F-DFB28E177397}
             */
            FCM::ConstFCMIID IID_ITIMELINE_BUILDER_FACTORY =
            { 0x19c111b8, 0x283e, 0x4ce5, {0xa8, 0x2f, 0xdf, 0xb2, 0x8e, 0x17, 0x73, 0x97} };
        }
    }
}

/* -------------------------------------------------- Class Declaration */

namespace Animate
{
    namespace Exporter
    {
        namespace Service
        {
            /**
             * @class ITimelineBuilderFactory
             *
             * @brief This interface represents a factory for a timeline builder.
             */
            class ITimelineBuilderFactory : public FCM::VirtualClass<IID_ITIMELINE_BUILDER_FACTORY>
            {
            public:
                /**
                 * @brief  This function is invoked to create a timeline builder.
                 *
                 * @param  pTimelineBuilder (OUT)
                 *         Timeline builder
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"