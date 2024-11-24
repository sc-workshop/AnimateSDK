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
 * @file  IFLADocService.h
 *
 * @brief This file contains the interface for IFLADocService. The IFLADocService 
 *        interface for document related services such as opening or closing of a FLA file.
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
            namespace Document
            {
                /**
                 * @brief Defines the universally-unique interface ID for
                 *        IFLADocService.
                 *
                 * @note  Textual Representation: {B4A3CDFE-FAAB-4566-8045-5B8447E9A23B}
                 */
                FCM::ConstFCMIID IID_IFLA_DOC_SERVICE =
                { 0xb4a3cdfe, 0xfaab, 0x4566, {0x80, 0x45, 0x5b, 0x84, 0x47, 0xe9, 0xa2, 0x3b} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace Document
            {
                /**
                 * @class IFLADocService
                 *
                 * @brief Defines the interface to services for FLA document.
                 */
                class IFLADocService : public FCM::VirtualClass<IID_IFLA_DOC_SERVICE>
                {
                public:
                    /**
                     * @brief  This function is used to open a FLA file.
                     *
                     * @param  pFullPath (IN)
                     *         The absolute path of the file on disk.
                     *
                     * @param  pOutDocument (OUT)
                     *         Document
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL OpenDocument(FCM::CStringRep16 pFullPath,
                        PIFLADocument& pOutDocument) = 0;


                    /**
                    * @brief  This function is used to close the opened FLA document.
                    *
                    * @param  inDocument (IN)
                    *         IFLADocument interface to close the opened FLA document.
                    *
                    * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                    */
                    virtual FCM::Result _FCMCALL CloseDocument(PIFLADocument inDocument) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"