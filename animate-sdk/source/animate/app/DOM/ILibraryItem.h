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
 * @file  ILibraryItem.h
 *
 * @brief This file contains the interface for ILibraryItem. 
 *        Each Library item is a reusable resource residing in the library. 
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

    /**
	* @def   kLibProp_LinkageClass_DictKey
	*
	* @brief Dictionary key for linkage class name.
	*/
    constexpr FCM::StringRep8 kLibProp_LinkageClass_DictKey = "LinkageClass";


    /**
    * @def   kLibProp_LinkageClass_DictType
    *
    * @brief Type of value for linkage class.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageClass_DictType = FCM::FCMDictRecTypeID::StringRep8;

    /**
    * @def   kLibProp_LinkageBaseClass_DictKey
    *
    * @brief Dictionary key for linkage class base name.
    */
    constexpr FCM::StringRep8 kLibProp_LinkageBaseClass_DictKey = "LinkageBaseClass";


    /**
    * @def   kLibProp_LinkageBaseClass_DictType
    *
    * @brief Type of value for linkage base class.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageBaseClass_DictType = FCM::FCMDictRecTypeID::StringRep8;

    /**
	* @def   kLibProp_LinkageIdentifier_DictKey
	*
	* @brief Dictionary key for linkage identifier.
	*/
    constexpr FCM::StringRep8 kLibProp_LinkageIdentifier_DictKey = "LinkageIdentifier";


    /**
    * @def   kLibProp_LinkageIdentifier_DictType
    *
    * @brief Type of value for linkage identifer.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageIdentifier_DictType = FCM::FCMDictRecTypeID::StringRep8;

    /**
    * @def   kLibProp_LinkageURL_DictKey
    *
    * @brief Dictionary key for linkage URL.
    */
    constexpr FCM::StringRep8 kLibProp_LinkageURL_DictKey = "LinkageURL";


    /**
    * @def   kLibProp_LinkageURL_DictType
    *
    * @brief Type of value for linkage URL.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageURL_DictType = FCM::FCMDictRecTypeID::StringRep8;


    /**
	* @def   kLibProp_LinkageExportForRS_DictKey
	*
	* @brief Dictionary key for 'Linkage Export For Runtime Sharing'.
	*/
    constexpr FCM::StringRep8 kLibProp_LinkageExportForRS_DictKey = "LinkageExportForRS";


    /**
    * @def   kLibProp_LinkageExportForRS_DictType
    *
    * @brief Type of value for 'Linkage Export For Runtime Sharing'.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageExportForRS_DictType = FCM::FCMDictRecTypeID::Bool;


    /**
    * @def   kLibProp_LinkageImportForRS_DictKey
    *
    * @brief Dictionary key for 'Import For Runtime Sharing'.
    */
    constexpr FCM::StringRep8 kLibProp_LinkageImportForRS_DictKey = "LinkageImportForRS";


    /**
    * @def   kLibProp_LinkageImportForRS_DictType
    *
    * @brief Type of value for 'Import For Runtime Sharing'.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageImportForRS_DictType = FCM::FCMDictRecTypeID::Bool;


    /**
	* @def   kLibProp_LinkageExportInFirstFrame_DictKey
	*
	* @brief Dictionary key for 'Export In First Frame'.
	*/
    constexpr FCM::StringRep8 kLibProp_LinkageExportInFirstFrame_DictKey = "LinkageExportInFirstFrame";


    /**
    * @def   kLibProp_LinkageExportInFirstFrame_DictType
    *
    * @brief Type of value for 'Export In First Frame'.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageExportInFirstFrame_DictType = FCM::FCMDictRecTypeID::Bool;


    /**
    * @def   kLibProp_LinkageExportForAS_DictKey
    *
    * @brief Dictionary key for 'Export For Actionscript'.
    */
    constexpr FCM::StringRep8 kLibProp_LinkageExportForAS_DictKey = "LinkageExportForAS";


    /**
    * @def   kLibProp_LinkageExportForAS_DictType
    *
    * @brief Type of value for 'Export For Actionscript'.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_LinkageExportForAS_DictType = FCM::FCMDictRecTypeID::Bool;

    /**
    * @def   kLibProp_SymbolType_DictKey
    *
    * @brief Dictionary key for the property "type" of the Symbol.
    */
    constexpr FCM::StringRep8 kLibProp_SymbolType_DictKey = "SymbolType";


    /**
    * @def   kLibProp_SymbolType_DictType
    *
    * @brief Type of value of the property "type" of the Symbol.
    */
    constexpr FCM::FCMDictRecTypeID kLibProp_SymbolType_DictType = FCM::FCMDictRecTypeID::StringRep8;

    /**
    * @def   kLibProp_SymbolType_Button
    *
    * @brief Dictionary value for the key kLibProp_SymbolType_DictKey if the symbol is Button
    */
    constexpr FCM::StringRep8 kLibProp_SymbolType_Button = "Button";

    /**
    * @def   kLibProp_SymbolType_MovieClip
    *
    * @brief Dictionary value for the key kLibProp_SymbolType_DictKey if the symbol is Movie Clip
    */
    constexpr FCM::StringRep8 kLibProp_SymbolType_MovieClip = "MovieClip";

    /**
    * @def   kLibProp_SymbolType_Graphic
    *
    * @brief Dictionary value for the key kLibProp_SymbolType_DictKey if the symbol is Graphic
    */
    constexpr FCM::StringRep8 kLibProp_SymbolType_Graphic = "Graphic";


    namespace DOM
    {
        /**
         * @brief Defines the universally-unique interface ID for
         *        ILibraryItem.
         *
         * @note  Textual Representation: {2EA5B91D-26C8-45A4-8BDC-41AD1F2C7B73}
         */
        FCM::ConstFCMIID IID_ILIBRARY_ITEM =
        { 0x2ea5b91d, 0x26c8, 0x45a4, {0x8b, 0xdc, 0x41, 0xad, 0x1f, 0x2c, 0x7b, 0x73} };
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        /**
        * @class ILibraryItem
        *
        * @brief Defines the interface that represents a library item in the library.
        */

        class ILibraryItem : public FCM::VirtualClass<IID_ILIBRARY_ITEM>
        {
        public:
            /**
             * @brief  Returns the name of the item.
             *
             * @param  ppName (OUT)
             *         Name of the library item with the full path in Library.
             *
             * @return On success, FCM_SUCCESS is returned, else an error code is returned.
             *
             * @note   The memory allocated for 'ppName' in this function must be freed
             *         by the caller using IFCMCalloc::Free().
             */
            virtual FCM::Result _FCMCALL GetName(FCM::StringRep16& ppName) = 0;


            /**
             * @brief  GetProperties returns a set of property names and their values.
             *
             * @param  pPropDict (OUT)
             *         Dictionary containing the properties and their values. The possible properties
             *         in the dictionary are as follows:
             *           kLibProp_LinkageClass_DictKey,
             *           kLibProp_LinkageBaseClass_DictKey,
             *           kLibProp_LinkageIdentifier_DictKey,
             *           kLibProp_LinkageURL_DictKey,
             *           kLibProp_LinkageExportForRS_DictKey,
             *           kLibProp_LinkageImportForRS_DictKey,
             *           kLibProp_LinkageExportInFirstFrame_DictKey,
             *           kLibProp_LinkageExportForAS_DictKey,
             *           kLibProp_SymbolType_DictKey
             *
             * @return On success, FCM_SUCCESS is returned, else an error code is returned.
             */
            virtual FCM::Result _FCMCALL GetProperties(FCM::PIFCMDictionary& pPropDict) = 0;

        };
    }
}

#include "animate/core/common/FCMPostConfig.h"