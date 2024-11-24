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
 * @file  IFCMClassFactory.h
 *
 * @brief This file contains interface for IFCMClassFactory.
 *        IFCMClassFactory enables a class of objects to be created.
 */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/core/common/FCMDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

/* -------------------------------------------------- Macros / Constants */
namespace FCM 
{
    /**
     * @brief Defines the universally-unique interface ID for IFCMClassFactory.
     *
     * @note  Textual Representation:  {B249192E-D262-43BA-8652-C41FCB3420F5}
     */
    ConstFCMIID FCMIID_IFCMClassFactory =
        {0xb249192e, 0xd262, 0x43ba, {0x86, 0x52, 0xc4, 0x1f, 0xcb, 0x34, 0x20, 0xf5}};
}



/* -------------------------------------------------- Class Declaration */
namespace FCM
{
    /**
     * @class IFCMClassFactory
     *
     * @brief Defines the interface that represents the ClassFactory.
     */

    class IFCMClassFactory : public FCM::VirtualClass<FCMIID_IFCMClassFactory>
    {
    public:
        /**
         * @brief  Creates an uninitialized object.
         *
         * @param  pUnkOuter (IN)
         *         In aggregation, specify a pointer to the controlling IUnknown interface of the aggregate.
         *         Otherwise, this parameter must be NULL.
         *
         * @param  pCallback (IN)
         *         Pointer to callback function.
         *
         * @param  iid (IN)
         *         Reference to the identifier interface.
         *
         * @param  pAny (OUT)
         *         Upon success, contains address of requested interface object.
         *         On failure, set to NULL.
         *
         * @return FCM_SUCCESS is returned for success, else an error code is returned.
         */
        virtual FCM::Result _FCMCALL CreateInstance(
            PIFCMUnknown pUnkOuter,
            PIFCMCallback pCallback,
            ConstRefFCMIID iid,
            PPVoid pAny) = 0;

    };
}  // namespace FCM

#include "animate/core/common/FCMPostConfig.h"