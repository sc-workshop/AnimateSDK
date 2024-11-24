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
 * @file  IMedia3DExportService.h
 *
 * @brief This file contains the interface for IMedia3DExportService. The IMedia3DExportService
 *        is used to export the Media3D library items.
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
            namespace Media3D
            {
                /**
                 * @brief Defines the universally-unique interface ID for
                 *        IMedia3DExportService.
                 *
                 * @note  Textual Representation:
                     {E9F0B6DB-9611-4BD8-A4AD-2C6EAE7B242B}
                 */
                FCM::ConstFCMIID IID_IMedia3D_EXPORT_SERVICE =
                { 0xe9f0b6db, 0x9611, 0x4bd8, {0xa4, 0xad, 0x2c, 0x6e, 0xae, 0x7b, 0x24, 0x2b} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace Media3D
            {
                /**
                 * @class IMedia3DExportService
                 *
                 * @brief Defines the interface for exporting Media3D library item
                 */
                class IMedia3DExportService : public FCM::VirtualClass<IID_IMedia3D_EXPORT_SERVICE>
                {
                public:
                    /**
                     * @brief  This function is used to copy a Media3D to a specified path.
                     *
                     * @param  path (IN)
                     *         path of Media3D in library.
                     *
                     * @param  pFilePath (IN)
                     *         A string that specifies the absolute path (including name)
                     *         of the exported files. The parent directory should exist.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL CopyFilesTo(
                        const DOM::IFLADocument* pFlaDocument,
                        const FCM::CStringRep16 libPath,
                        FCM::CStringRep16 destPath) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"