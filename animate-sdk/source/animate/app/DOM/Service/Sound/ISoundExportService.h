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
 * @file  ISoundExportService.h
 *
 * @brief This file contains the interface for ISoundExportService. The ISoundExportService 
 *        is used to export the Sound library items to different audio formats.
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
            namespace Sound
            {
                /**
                 * @brief Defines the universally-unique Interface ID for
                 *        ISoundExportService.
                 *
                 * @note  Textual Representation: {DEAD57CE-C4DC-44D2-B140-E82374272CF5}
                 */
                FCM::ConstFCMIID IID_ISOUND_EXPORT_SERVICE =
                { 0xdead57ce, 0xc4dc, 0x44d2, {0xb1, 0x40, 0xe8, 0x23, 0x74, 0x27, 0x2c, 0xf5} };
            }
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace Sound
            {
                /**
                 * @class ISoundExportService
                 *
                 * @brief Defines the interface for converting sound library item into different
                 *        formats.
                 */
                class ISoundExportService : public FCM::VirtualClass<IID_ISOUND_EXPORT_SERVICE>
                {
                public:
                    /**
                     * @brief  This function exports a sound to a ".wav" or ".mp3" format.
                     *
                     * @param  pSound (IN)
                     *         An IMediaItem of a sound in library.
                     *
                     * @param  pFilePath (IN)
                     *         A string that specifies the absolute path (including name)
                     *         of the exported file. The parent directory should exist.
                     *         The file extension must be ".WAV" or ".MP3".
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL ExportToFile(
                        const LibraryItem::PIMediaItem pSound,
                        FCM::CStringRep16 pFilePath) = 0;

                };
            }
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"