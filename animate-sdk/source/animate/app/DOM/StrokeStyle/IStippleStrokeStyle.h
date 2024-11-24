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
 * @file  IStippleStrokeStyle.h
 *
 * @brief This file contains the interface for IStippleStrokeStyle. This interface 
 *        represents a stipple stroke style of an edge.
 */
#pragma once
#include "animate/core/common/FCMTypes.h"
#include "animate/app/DOM/Utils/DOMTypes.h"

#include "animate/app/DOM/DOMDeclarations.h"
#include "animate/core/common/FCMDeclarations.h"

#include "animate/core/common/FCMPreConfig.h"

namespace Animate
{
    /* -------------------------------------------------- Enums */
    namespace DOM
    {
        namespace StrokeStyle
        {
            /**
             * @enum  StippleDotSize
             *
             * @brief Specifies the size of the dot in a stippled stroke.
             */
            enum class StippleDotSize
            {
                /** Tiny dots */
                STIPPLE_DOTSIZE_TINY = 0,

                /** Small dots */
                STIPPLE_DOTSIZE_SMALL,

                /** Medium dots */
                STIPPLE_DOTSIZE_MEDIUM,

                /** Large dots */
                STIPPLE_DOTSIZE_LARGE,
            };


            /**
             * @enum  StippleVariation
             *
             * @brief Specifies the variation of the dot in a stippled stroke.
             */
            enum class StippleVariation
            {
                /** One size for all the dots */
                STIPPLE_VARIATION_ONESIZE = 0,

                /** Small variation in the size of the dots */
                STIPPLE_VARIATION_SMALL,

                /** Varied sizes of dots */
                STIPPLE_VARIATION_VARIEDSIZE,

                /** Randomly sized dots */
                STIPPLE_VARIATION_RANDOMSIZE
            };


            /**
             * @enum  StippleDensity
             *
             * @brief Specifies the density of the dots in a stippled stroke.
             */
            enum class StippleDensity
            {
                /** Very Dense */
                STIPPLE_DENSITY_VERYDENSE = 0,

                /** Dense */
                STIPPLE_DENSITY_DENSE,

                /** Sparse */
                STIPPLE_DENSITY_SPARSE,

                /** Very Sparse */
                STIPPLE_DENSITY_VERYSPARSE,
            };
        }
    }


    /* -------------------------------------------------- Macros / Constants */
    namespace DOM
    {
        namespace StrokeStyle
        {
            /**
             * @brief Defines the universally-unique interface ID for
             *        IStippleStroke.
             *
             * @note  Textual Representation: {87262F76-0018-4B42-83E5-10BD1E66CEE2}
             */
            FCM::ConstFCMIID IID_ISTIPPLE_STROKE_STYLE =
            { 0x87262f76, 0x18, 0x4b42, {0x83, 0xe5, 0x10, 0xbd, 0x1e, 0x66, 0xce, 0xe2} };
        }
    }


    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace StrokeStyle
        {
            /**
             * @class IStippleStrokeStyle
             *
             * @brief Defines an interface that represents a "stipple" stroke style of an edge.
             */
            class IStippleStrokeStyle : public FCM::VirtualClass<IID_ISTIPPLE_STROKE_STYLE>
            {
            public:
                /**
                 * @brief  This function provides the stroke thickness.
                 *
                 * @param  thickness (OUT)
                 *         Thickness of the stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetThickness(FCM::Double& thickness) = 0;


                /**
                 * @brief  This function gets the fill style settings of the stroke.
                 *
                 * @param  pFillStyle (OUT)
                 *         Fill style of the stroke (ISolidFillStyle or IGradientFillStyle
                 *         or IBitmapFillStyle).
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 *
                 * @see    DOM::FillStyle::ISolidFillStyle
                 *
                 * @see    DOM::FillStyle::IGradientFillStyle
                 *
                 * @see    DOM::FillStyle::IBitmapFillStyle
                 */
                virtual FCM::Result _FCMCALL GetFillStyle(FCM::PIFCMUnknown& pFillStyle) = 0;


                /**
                 * @brief  This function gets the dot size type of the stipple stroke style.
                 *
                 * @param  dotSize (OUT)
                 *         Dot size
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetDotSize(StippleDotSize& dotSize) = 0;


                /**
                 * @brief  This function gets the variation of a stipple line.
                 *
                 * @param  variation (OUT)
                 *         Variation of a stippled line.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetVariation(StippleVariation& variation) = 0;


                /**
                 * @brief  This function gets the density of a stipple line.
                 *
                 * @param  density (OUT)
                 *         Density of a stipple line.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetDensity(StippleDensity& density) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"