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
 * @file  FCMTypes.h
 *
 * @brief This file contains definitions for various data types.
 */

#pragma once
#if defined(__GNUC__) && defined(__APPLE__)
#include <sys/types.h>
#endif

#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>

#include "FCMPreConfig.h"

namespace FCM
{
#if defined(__GNUC__) && defined(__APPLE__)

	typedef u_int8_t                U_Int8;
	typedef int8_t                  S_Int8;
	typedef u_int16_t               U_Int16;
	typedef int16_t                 S_Int16;
	typedef u_int32_t               U_Int32;
	typedef int32_t                 S_Int32;
	typedef u_int64_t               U_Int64;
	typedef int64_t                 S_Int64;

#else //Windows

	/**
	 * @typedef U_Int8
	 *
	 * @brief   Type to define an unsigned 8-bit value.
	 */
	typedef unsigned    _int8       U_Int8;

	/**
	 * @typedef S_Int8
	 *
	 * @brief   Type to define a signed 8-bit value.
	 */
	typedef signed      _int8       S_Int8;

	/**
	 * @typedef U_Int16
	 *
	 * @brief   Type to define an unsigned 16-bit value.
	 */
	typedef unsigned    _int16      U_Int16;

	/**
	 * @typedef S_Int16
	 *
	 * @brief   Type to define a signed 16-bit value.
	 */
	typedef signed      _int16      S_Int16;

	/**
	 * @typedef U_Int32
	 *
	 * @brief   Type to define an unsigned 32-bit value.
	 */
	typedef unsigned    _int32      U_Int32;

	/**
	 * @typedef S_Int32
	 *
	 * @brief   Type to define a signed 32-bit value.
	 */
	typedef signed      _int32      S_Int32;

	/**
	 * @typedef U_Int64
	 *
	 * @brief   Type to define a unsigned size 64-bit value.
	 */
	typedef unsigned    _int64      U_Int64;

	/**
	 * @typedef S_Int64
	 *
	 * @brief   Type to define a signed 64-bit value.
	 */
	typedef signed      _int64      S_Int64;

#endif

	/**
	 *  @typedef Float
	 *
	 *  @brief   Type to define a IEEE single precision floating point number.
	 */
	typedef float                   Float;

	/**
	 *  @typedef Double
	 *
	 *  @brief   Type to define a IEEE double-precision floating point number.
	 */
	typedef double                  Double;

	/**
	*  @typedef Long
	*
	*  @brief   Type to define a 64-bit two's complement integer.
	*/
	typedef long                  Long;

	/**
	 *  @typedef Result
	 *
	 *  @brief   Type to define a result. This is usually the return type for most APIs.
	 */
	typedef U_Int32                 Result;

	/**
	 *  @typedef Byte
	 *
	 *  @brief   Type to define a byte.
	 */
	typedef U_Int8                  Byte;

	/**
	 *  @typedef Boolean
	 *
	 *  @brief   Type to define a boolean.
	 */
	typedef U_Int8                  Boolean;

	/**
	 *  @typedef PVoid
	 *
	 *  @brief   Type to define a void pointer.
	 */
	typedef void* PVoid;

	/**
	 *  @typedef PPVoid
	 *
	 *  @brief   Type to define a pointer to pointer to void.
	 */
	typedef void** PPVoid;

	/**
	 *  @typedef StringRep16
	 *
	 *  @brief   Used to define a UTF16 (unicode) encoded string.
	 */
	typedef U_Int16* StringRep16;

	/**
	 *  @typedef CStringRep16
	 *
	 *  @brief   Used to define a constant UTF16 (unicode) encoded string.
	 */
	typedef const U_Int16* CStringRep16;

	/**
	 *  @typedef StringRep8
	 *
	 *  @brief   Used to define a unicode (UTF8) encoded string.
	 */
	typedef char* StringRep8;

	/**
	 *  @typedef CStringRep8
	 *
	 *  @brief   Used to define a constant unicode (UTF8) encoded string.
	 */
	typedef const char* CStringRep8;

	/**
	 * @typedef DictionaryKey
	 *
	 * @brief	Used to define a string that used as dictionary key
	 */
	typedef CStringRep8 DictionaryKey;

	/**
	 * @struct FCMGUID
	 *
	 * @brief  Represents a 128 bit or 16-byte GUID (Globally Unique IDentifier) value.
	 */
	struct FCMGUID {
		/** First 4 bytes of GUID */
		FCM::U_Int32    Data1;

		/** Next 2 bytes of GUID */
		FCM::U_Int16    Data2;

		/** Next 2 bytes of GUID */
		FCM::U_Int16    Data3;

		/** Last 8 bytes of GUID */
		FCM::Byte       Data4[8];

		operator std::string() const
		{
			std::ostringstream result;
			unsigned i;

			result.fill('0');

			result << std::hex;
			result << std::setw(8) << (Data1);
			result << "-";
			result << std::setw(4) << (Data2);
			result << "-";
			result << std::setw(4) << (Data3);
			result << "-";

			for (i = 0; i < 2; ++i)
			{
				result << std::setw(2) << (unsigned int)(Data4[i]);
			}
			result << "-";

			for (; i < 8; ++i)
			{
				result << std::setw(2) << (unsigned int)(Data4[i]);
			}

			std::string guid_str = result.str();

			std::transform(
				guid_str.begin(), 
				guid_str.end(), 
				guid_str.begin(), 
				[](const char val) 
				{ 
					return (char)std::toupper(val); 
				}
			);

			return guid_str;
		}
	};

	/**
	 * @enum  VarType
	 *
	 * @brief Type for the values of a variant (VARIANT)
	 */
	enum class VarType {
		/** Represents a invalid type */
		Invalid = 0,

		/** Represents a FCM::U_Int32. */
		UInt32,

		/** Represents a FCM::Float */
		Float,

		/** Represents a FCM::Boolean */
		Bool,

		/** Represents a FCM::StringRep8 */
		String,

		/** Represents a FCM::Double */
		Double,

		/** Marks the final Entry in the list */
		Last
	};

	/**
	* @enum  TextureMappingType
	*
	*/
	enum class TextureMappingType {
		/** Represents a default type */
		None = 0,

		/** Represents Cylindrical type. */
		Cylinder,

		/** Represents Sphere type. */
		Sphere
	};

	enum class BlendMode {
		/**
		* @enum BlendMode
		*/
		/** Normal */
		NORMAL,

		/** Layer */
		LAYER,

		/** Darken */
		DARKEN,

		/** Multiply */
		MULTIPLY,

		/** Lighten */
		LIGHTEN,

		/** Screen */
		SCREEN,

		/** Overlay */
		OVERLAY,

		/** Hardlight */
		HARDLIGHT,

		/** Add */
		ADD,

		/** Substract */
		SUBTRACT,

		/** Difference */
		DIFFERENCE,

		/** Invert */
		INVERT,

		/** Alpha */
		ALPHA,

		/** Erase */
		ERASE
	};

	/**
	* @struct VARIANT
	*
	* @brief  Defines a data-type called 'variant' that can be used to store different types of data.
	*/
	struct VARIANT
	{
		/** Type of value */
		VarType    type;

		/** Represents actual value */
		union
		{
			/** This value is valid if m_type is kFCMVarype_UInt32 */
			U_Int32         uVal;

			/** This value is valid if m_type is kFCMVarype_Float */
			Float           fVal;

			/** This value is valid if m_type is kFCMVarype_Bool */
			Boolean         bVal;

			/** This value is valid if m_type is kFCMVarype_CString */
			StringRep16     strVal;

			/** This value is valid if m_type is kFCMVarype_Double */
			Double          dVal;

			/** This value is reserved for future use */
			PVoid           reserved;
		} value;
	};

	/**
	 * @brief  This function compares for GUID equality.
	 *
	 * @param  guidOne (IN)
	 *         One of the GUID that needs to be compared.
	 *
	 * @param  guidOther (IN)
	 *         Other GUID that needs to be compared.
	 *
	 * @return Returns true if GUIDs are same; else false is returned.
	 */
	__inline bool operator==(const FCMGUID& guidOne, const FCMGUID& guidOther)
	{
		return ((guidOne.Data1 == guidOther.Data1) &&
			(guidOne.Data2 == guidOther.Data2) &&
			(guidOne.Data3 == guidOther.Data3) &&
			(*(U_Int32*)guidOne.Data4 == *(U_Int32*)guidOther.Data4) &&
			(*(U_Int32*)(guidOne.Data4 + 4) == *((U_Int32*)(guidOther.Data4 + 4))));
	}

	/**
	 * @brief  This function compares for GUID in-equality.
	 *
	 * @param  guidOne (IN)
	 *         One of the GUID that needs to be compared.
	 *
	 * @param  guidOther (IN)
	 *         Other GUID that needs to be compared.
	 *
	 * @return Returns true if GUIDs are not equal; else false is returned.
	 */
	__inline bool operator!=(const FCMGUID& guidOne, const FCMGUID& guidOther)
	{
		return !(guidOne == guidOther);
	}

	/**
	 *  @typedef FCMIID
	 *
	 *  @brief   Represents the interface ID type.
	 */
	typedef FCMGUID FCMIID;

	/**
	 *  @typedef FCMCLSID
	 *
	 *  @brief   Represents the class ID type.
	 */
	typedef FCMGUID FCMCLSID;

	/**
	 *  @typedef SRVCID
	 *
	 *  @brief   Represents the service ID type.
	 */
	typedef FCMGUID SRVCID;

	/**
	 *  @typedef ConstFCMIID
	 *
	 *  @brief   Represents the constant interface ID type.
	 */
	//typedef const FCMIID        ConstFCMIID;
	using ConstFCMIID = const FCMIID;

	/**
	 *  @typedef ConstFCMCLSID
	 *
	 *  @brief   Represents the constant class ID type.
	 */
	typedef const FCMIID        ConstFCMCLSID;

	/**
	 *  @typedef ConstRefFCMIID
	 *
	 *  @brief   Represents the constant reference to interface ID.
	 */
	typedef ConstFCMCLSID& ConstRefFCMIID;

	/**
	 *  @typedef ConstRefFCMCLSID
	 *
	 *  @brief   Represents the constant reference to class ID.
	 */
	typedef ConstFCMCLSID& ConstRefFCMCLSID;

	/**
	 *  @typedef FCMNotifyID
	 *
	 *  @brief   The FCM notification ID type.
	 */
	typedef FCMGUID  FCMNotifyID;

	/**
	 *  @typedef PFCMNotifyID
	 *
	 *  @brief   The pointer to FCM notification ID.
	 */
	typedef FCMGUID* PFCMNotifyID;

	/**
	 *  @typedef ConstFCMNotifyID
	 *
	 *  @brief   The constant FCM notification ID.
	 */
	typedef const FCMNotifyID   ConstFCMNotifyID;

	/**
	 *  @typedef ConstRefFCMNotifyID
	 *
	 *  @brief   The reference to constant FCM notification ID.
	 */
	typedef const FCMNotifyID& ConstRefFCMNotifyID;

	/**
	 *  @var   FCMIID_NULL
	 *
	 *  @brief Represents an invalid Interface Identifier (IID).
	 */
	const FCMIID FCMIID_NULL = { 0, 0, 0, { 0, 0,  0,  0,  0,  0,  0,  0 } };

	/**
	 *  @var   FCMCLSID_NULL
	 *
	 *  @brief Represents an invalid class Identifier (CLSID).
	 */
	const FCMCLSID FCMCLSID_NULL = { 0, 0, 0, { 0, 0,  0,  0,  0,  0,  0,  0 } };

	/** @cond HIDE_PRIVATE */

	

	/** @endcond */
}; //namespace FCM

#include "FCMPostConfig.h"