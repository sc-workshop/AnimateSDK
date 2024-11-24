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
 * @file  IFCMPair.h
 *
 * @brief This file contains interface for IFCMPair. 
 *        IFCMPair is an interface for generic container for storing pair of data.
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
     * @brief Defines the universally-unique interface ID for IFCMPair.
     *
     * @note  Textual Representation: {0C80393E-2037-4CC3-BF69-1CEE8175E16D}
     */
    ConstFCMIID FCMIID_IFCMPair =
        {0xc80393e, 0x2037, 0x4cc3, {0xbf, 0x69, 0x1c, 0xee, 0x81, 0x75, 0xe1, 0x6d}};
}


/* -------------------------------------------------- Enums */

namespace FCM
{
    /**
     * @brief This defines different types of pair IDs.
     */
    enum class FCMPairTypeID
    {
        /** illegal */
        Invalid = 0,

        /** Returns a PIFCMUnknown. Put does AddRef; Remove does Release. */
        PIFCMUnknown,

        /** Returns a FCM::S_Int32. */
        Long,
        
        /** Returns a 32 bit IEEE single precision floating point number. */
        Float,
        
        /** FCM::Boolean */
        Bool,
        
        /** FCMGUID */
        FCMGUID,

        /** Arbitrary bytes */
        Bytes,
        
        /** Null-terminated StringRep8 string */
        StringRep8,

        /** FCM::Double */
        Double,

        /* Last */
        _Last_

    };

};  // namespace FCM


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Declaration */

namespace FCM
{ 
    /**
     * @class IFCMPair
     *
     * @brief Defines the interface that represents a pair.
     */
    class IFCMPair : public FCM::VirtualClass<FCMIID_IFCMPair>
    {
    public:
        virtual FCM::Result _FCMCALL SetValue(
            FCM::U_Int32 index,
            FCMPairTypeID type,
            FCM::PVoid value,
            FCM::U_Int32 valueLen) = 0;

        virtual FCM::Result _FCMCALL GetInfo(
            FCM::U_Int32 index,
            FCMPairTypeID& type,
            FCM::U_Int32& valueLen) = 0;

        virtual FCM::Result _FCMCALL GetValue(
            FCM::U_Int32 index,
            FCMPairTypeID type,
            FCM::PVoid value,
            FCM::U_Int32 valueLen) = 0;

        virtual FCM::Result _FCMCALL Remove(
            FCM::U_Int32 index) = 0;

        virtual FCM::Result _FCMCALL Clear() = 0;

    };

};  // namespace FCM


/* -------------------------------------------------- Inline / Functions */

#include "animate/core/common/FCMPostConfig.h"