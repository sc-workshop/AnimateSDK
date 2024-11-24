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
 * @file  IFCMCalloc.h
 *
 * @brief This file contains interface for IFCMCalloc. 
 *        The IFCMCalloc interface provides a mechanism for allocating and deallocating memory.
 *        This interface is used when there is shared memory between Plug-in and FCM Application and
 *        the memory being allocated by Plug-in is freed by FCM Application (or) vice-versa.
 */

#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/core/common/FCMDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Macros / Constants */

namespace FCM 
{
    /**
     * @brief Defines the universally unique ID for the interface IFCMCalloc.
     *
     * @note  Textual Representation:  {1920B8DD-6CD8-4D9A-8272-4CBA3F29086F}
     */
    ConstFCMIID FCMIID_IFCMCalloc =
        {0x1920b8dd, 0x6cd8, 0x4d9a, {0x82, 0x72, 0x4c, 0xba, 0x3f, 0x29, 0x8, 0x6f}};
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Declaration */

 namespace FCM
 { 
     /**
     * @class IFCMCalloc
     *
     * @brief Defines the interface that represents the calloc.
     */
     class IFCMCalloc : public FCM::VirtualClass<FCMIID_IFCMCalloc>
     {
     public:
         /**
          * @brief  Allocates memory of the size requested by the parameter cb.
          *
          * @param  cb (IN)
          *         Size of the memory to be allocated.
          *
          * @return Returns a pointer to the allocated memory.
          */
         virtual PVoid _FCMCALL Alloc(U_Int32 cb) = 0;


         /**
         * @brief  Deallocates the memory pointed by the param pv.
         *
         * @param  pv (IN)
         *         Pointer to memory to be deallocated.
         */
         virtual void _FCMCALL Free(PVoid pv) = 0;

     };

};  // namespace FCM


/* -------------------------------------------------- Inline / Functions */

#include "animate/core/common/FCMPostConfig.h"