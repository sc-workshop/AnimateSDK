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
 * @file  IFontTableGeneratorService.h
 *
 * @brief This file contains interface for IFontTableGeneratorService.  
 *        IFontTableGeneratorService contains methods to generate font table for 
 *        a font item.
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
            namespace FontTable
            {
                /**
                 * @brief Defines the universally-unique interface ID for
                 *        IFontTableGeneratorService.
                 *
                 * @note  Textual Representation: {DFA12563-76B3-4F88-9E04-FD150CB63968}
                 */
                FCM::ConstFCMIID IID_IFONT_TABLE_GENERATOR_SERVICE =
                { 0xdfa12563, 0x76b3, 0x4f88, {0x9e, 0x4, 0xfd, 0x15, 0xc, 0xb6, 0x39, 0x68} };
            }
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace Service
        {
            namespace FontTable
            {
                /**
                * @class IFontTableGeneratorService
                *
                * @brief Defines the interface to generate the font table for a font.
                */
                class IFontTableGeneratorService : public FCM::VirtualClass<IID_IFONT_TABLE_GENERATOR_SERVICE>
                {
                public:
                    /**
                     * @brief  This function generates font table for a set of specified
                     *         characters of a specified font.
                     *
                     * @param  pFlaDoc (IN)
                     *         Document for which font table needs to be generated.
                     *
                     * @param  pCharacterString (IN)
                     *         The set of characters.
                     *
                     * @param  pFontName (IN)
                     *         The name of the font for which font table needs to be generated.
                     *
                     * @param  pFontStyle (IN)
                     *         The style of the font in string.
                     *
                     * @param  pFontTable (OUT)
                     *         The font table for the specified chars in the font is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL CreateFontTable(
                        const PIFLADocument& pFlaDoc,
                        FCM::CStringRep16 pCharacterString,
                        FCM::CStringRep16 pFontName,
                        FCM::CStringRep8 pFontStyle,
                        PIFontTable& pFontTable) = 0;


                    /**
                     * @brief  This function generates font table for embedded chars and embedded
                     *         char ranges in the font item.
                     *
                     * @param  pFontItem (IN)
                     *         The font item for which font table needs to be generated.
                     *
                     * @param  pFontTable (OUT)
                     *         The font table for embedded chars and embedded char ranges in
                     *         the font item is returned.
                     *
                     * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                     */
                    virtual FCM::Result _FCMCALL CreateFontTableForFontItem(
                        const LibraryItem::PIFontItem& pFontItem,
                        PIFontTable& pFontTable) = 0;

                };
            }
        }
    }
}


#include "animate/core/common/FCMPostConfig.h"