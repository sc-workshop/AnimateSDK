/*************************************************************************
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
**************************************************************************/

/**
 * @file  IFCMUnknown.h
 *
 * @brief This file contains interface for IFCMUnknown. All FCM interfaces inherit 
 *        from IFCMUnknown. This interface is similar to Microsoft's COM IUnknown. 
 *        Primary purpose of this interface is to facilitate a way to obtain pointers to
 *        other interfaces through QueryInterface method.
 */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/core/common/FCMMacros.h"

#include "animate/core/common/FCMPreConfig.h"

/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Macros / Constants */

namespace FCM 
{
    /**
     * @brief Defines the universally-unique interface ID for IFCMUnknown.
     *
     * @note  Textual Representation:  {775C253A-376D-4069-8336-C51AFD11163F}
     */
    ConstFCMIID FCMIID_IFCMUnknown = 
     {0x775c253a, 0x376d, 0x4069, {0x83, 0x36, 0xc5, 0x1a, 0xfd, 0x11, 0x16, 0x3f}};
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Declaration */

namespace FCM 
{
    /**
     * @class IFCMUnknown
     *
     * @brief Defines the interface that represents the IUnknown object.
     */
    class IFCMUnknown
    {
    public:
		inline static FCMIID GetIID() {
            return FCMIID_IFCMUnknown;
		};

    public:
        /**
         * @brief  Return pointers to supported interfaces on given object.
         *
         * @param  pInterfaceID (IN)
         *         Pointer to the identifier interface.
         *
         * @param  ppvObj (OUT)
         *         Upon success, contains the address of the requested interface object.
         *
         * @return FCM_SUCCESS is returned for success, else an error code is returned.
         */
        virtual FCM::Result _FCMCALL QueryInterface(
            ConstRefFCMIID pInterfaceID,
            PPVoid ppvObj) = 0;


        /**
         * @brief  Increments the reference count on the given object.
         *
         * @return Returns the new reference count.
         */
        virtual U_Int32 _FCMCALL AddRef() = 0;


        /**
         * @brief  Decrements the reference count on the given object.
         *
         * @return Returns the new reference count.
         */
        virtual U_Int32 _FCMCALL Release() = 0;

    };

};  // namespace FCM

#include "animate/core/common/FCMPostConfig.h"