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
 * @file  IHatchedStrokeStyle.h
 *
 * @brief This file contains the interface for IHatchedStrokeStyle. This interface 
 *        represents a hatched stroke of an edge.
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
             * @enum  HatchedThicknessType
             *
             * @brief Defines the thickness of Hatched stroke style.
             */
            enum class HatchedThicknessType
            {
                /** Hairline  */
                HATCHED_THICKNESS_HAIRLINE = 0,

                /** Thin */
                HATCHED_THICKNESS_THIN,

                /** Medium  */
                HATCHED_THICKNESS_MEDIUM,

                /** Thick */
                HATCHED_THICKNESS_THICK,
            };


            /**
             * @enum  HatchedSpaceType
             *
             * @brief Defines the spacing in the Hatched stroke style.
             */
            enum class HatchedSpaceType
            {
                /** Very close space  */
                HATCHED_SPACE_VERYCLOSE = 0,

                /** Close space  */
                HATCHED_SPACE_CLOSE,

                /** Distant space  */
                HATCHED_SPACE_DISTANT,

                /** Very Distant space  */
                HATCHED_SPACE_VERYDISTANT
            };


            /**
             * @enum  HatchedJiggleType
             *
             * @brief Defines the type of jiggles in the Hatched stroke style.
             */
            enum class HatchedJiggleType
            {
                /** No Jiggle  */
                HATCHED_JIGGLE_NONE = 0,

                /** Bounce  */
                HATCHED_JIGGLE_BOUNCE,

                /** Loose  */
                HATCHED_JIGGLE_LOOSE,

                /** Wild  */
                HATCHED_JIGGLE_WILD
            };


            /**
             * @enum  HatchedRotateType
             *
             * @brief Defines the degree of rotation of the pattern within the Hatched stroke style.
             */
            enum class HatchedRotateType
            {
                /** No Rotation  */
                HATCHED_ROTATE_NONE = 0,

                /** Slight Rotation */
                HATCHED_ROTATE_SLIGHT,

                /** Medium Rotation */
                HATCHED_ROTATE_MEDIUM,

                /** Free Rotation */
                HATCHED_ROTATE_FREE
            };


            /**
             * @enum  HatchedCurveType
             *
             * @brief Defines the degree of curved nature of the pattern within the
             *        Hatched stroke style.
             */
            enum class HatchedCurveType
            {
                /** Staight */
                HATCHED_CURVE_STRAIGHT = 0,

                /** Slight Curve */
                HATCHED_CURVE_SLIGHTCURVED,

                /** Medium Curve */
                HATCHED_CURVE_MEDIUMCURVED,

                /** Very curved Curve */
                HATCHED_CURVE_VERYCURVED
            };


            /**
             * @enum  HatchedLengthType
             *
             * @brief Defines the length of the strokes in the pattern within the
             *        Hatched stroke style.
             */
            enum class HatchedLengthType
            {
                /** Equal length(No variation) */
                HATCHED_LENGTH_EQUAL = 0,

                /** Slight variation in the length */
                HATCHED_LENGTH_SLIGHTVARIATION,

                /** Medium variation in the length */
                HATCHED_LENGTH_MEDIUMVARIATION,

                /** Randomly varying length */
                HATCHED_LENGTH_RANDOM
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
             *        IHatchedStrokeStyle.
             *
             * @note  Textual Representation: {29964748-A64A-49F7-97DE-3FC19D054FDD}
             */
            FCM::ConstFCMIID IID_IHATCHED_STROKE_STYLE =
            { 0x29964748, 0xa64a, 0x49f7, {0x97, 0xde, 0x3f, 0xc1, 0x9d, 0x5, 0x4f, 0xdd} };
        }
    }

    /* -------------------------------------------------- Class Declaration */
    namespace DOM
    {
        namespace StrokeStyle
        {
            /**
             * @class IHatchedStrokeStyle
             *
             * @brief Defines an interface that represents a "hatched" stroke of an edge.
             */
            class IHatchedStrokeStyle : public FCM::VirtualClass<IID_IHATCHED_STROKE_STYLE>
            {
            public:
                /**
                 * @brief This function provides the stroke thickness.
                 *
                 * @param thickness (OUT)
                 *        Thickness of the stroke.
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
                 * @brief  This function gets the thickness type of a hatched stroke.
                 *
                 * @param  thicknessType (OUT)
                 *         Thickness type of a hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetHatchedThicknessType(
                    HatchedThicknessType& thicknessType) = 0;


                /**
                 * @brief  This function gets the spacing type of a hatched stroke.
                 *
                 * @param  spaceType (OUT)
                 *         Spacing type of the hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetSpaceType(HatchedSpaceType& spaceType) = 0;


                /**
                 * @brief  This specifies the jiggle type of a hatched stroke.
                 *
                 * @param  jiggleType (OUT)
                 *         Jiggle type of the hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetJiggleType(HatchedJiggleType& jiggleType) = 0;


                /**
                 * @brief  This function gets the rotation type of a hatched stroke.
                 *
                 * @param  rotateType (OUT)
                 *         Rotation type of the hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetRotateType(HatchedRotateType& rotateType) = 0;


                /**
                 * @brief  This function gets the curve type of a hatch stroke.
                 *
                 * @param  curveType (OUT)
                 *         Curve type of the hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetCurveType(HatchedCurveType& curveType) = 0;


                /**
                 * @brief  This function gets the type of hatching for the stroke.
                 *
                 * @param  lengthType (OUT)
                 *         Length type of the hatched stroke.
                 *
                 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
                 */
                virtual FCM::Result _FCMCALL GetLengthType(HatchedLengthType& lengthType) = 0;

            };
        }
    }
}

#include "animate/core/common/FCMPostConfig.h"