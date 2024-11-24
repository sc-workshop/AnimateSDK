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
 * @file  IPublisher.h
 *
 * @brief This file contains the interface for IPublisher. The IPublisher
 *        interface needs to be implemented by a "Publisher" plugin.
 *        It contains methods to publish a FLA document and a single timeline.
 *        'Adobe Animate CC' would invoke methods in this interface in the
 *        test-movie and publish workflows.
 */

#pragma once
#include "animate/core/common/FCMTypes.h"

#include "animate/core/common/FCMDeclarations.h"
#include "animate/app/DOM/DOMDeclarations.h"

#include "animate/app/Exporter/Service/IFrameCommandGenerator.h"

#include "animate/core/common/FCMPreConfig.h"


 /* -------------------------------------------------- Macros / Constants */
namespace Animate
{
	namespace Publisher
	{
		/**
		* @brief Defines the universally-unique Interface ID for IPublisher.
		*
		* @note Textual Representation: {3ACF3427-4FD-44EF-9FBA-3EBB62F044A2}
		*/
		FCM::ConstFCMIID FCMIID_IPublisher =
		{ 0x3acf3427, 0x4fd, 0x44ef, {0x9f, 0xba, 0x3e, 0xbb, 0x62, 0xf0, 0x44, 0xa2} };
	}
}
/* -------------------------------------------------- Class Declaration */

namespace Animate
{
	namespace Publisher
	{
		/**
		 * @class IPublisher
		 *
		 * @brief Defines an interface that represents publisher. This interface
		 *        must be implemented by a publisher plugin.
		 */
		class IPublisher : public FCM::VirtualClass<FCMIID_IPublisher>
		{
		public:
			/**
			 * @brief  This function is "typically" called by 'Adobe Animate CC' in the Publish/Test-Movie
			 *         workflow. The implementor of this function must publish the entire FLA Document
			 *         on invocation of this function.
			 *
			 * @param  pFlaDocument (IN)
			 *         Document to be published.
			 *
			 * @param  pDictPublishSettings (IN)
			 *         Dictionary containing values set in the publish settings dialog.
			 *
			 * @param  pDictConfig (IN)
			 *         Publish configuration parameters. These are controlled by
			 *         'Adobe Animate CC' and the plug-ins have no control over them. For e.g.,
			 *           pDictConfig[kPublishSettingsKey_PreviewNeeded] = "true" or "false";
			 *
			 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
			 */
			virtual FCM::Result _FCMCALL Publish(
				DOM::PIFLADocument pFlaDocument,
				const FCM::PIFCMDictionary pDictPublishSettings,
				const FCM::PIFCMDictionary pDictConfig) = 0;

			/**
			 * @brief  This function is "typically" called by 'Adobe Animate CC' in the Test-Scene/Test-Movie
			 *         workflow. The implementor of this function must publish the timeline on invocation
			 *         of this function.
			 *
			 * @param  pFlaDocument (IN)
			 *         Document containing the timeline.
			 *
			 * @param  pTimeline (IN)
			 *         Timeline to be published. It may correspond to a movie-clip or
			 *         a scene itself. This will be never be NULL.
			 *
			 * @param  frameRange (IN)
			 *         Range of frames to be published.
			 *
			 * @param  pDictPublishSettings (IN)
			 *         Dictionary containing values set in the publish settings dialog.
			 *
			 * @param  pDictConfig (IN)
			 *         Publish configuration parameters. These are controlled by
			 *         'Adobe Animate CC' and the plug-ins have no control over them. For e.g.,
			 *           pDictConfig[kPublishSettingsKey_PreviewNeeded] = "true" or "false";
			 *
			 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
			 */
			virtual FCM::Result _FCMCALL Publish(
				DOM::PIFLADocument pFlaDocument,
				DOM::PITimeline pTimeline,
				const Exporter::Service::RANGE& frameRange,
				const FCM::PIFCMDictionary pDictPublishSettings,
				const FCM::PIFCMDictionary pDictConfig) = 0;

			/**
			 * @brief  On invocation of this function, the publisher must clear its cache (if
			 *         it maintains one).
			 *
			 * @return On success, FCM_SUCCESS is returned, else an error code is returned.
			 */
			virtual FCM::Result _FCMCALL ClearCache() = 0;
		};
	}
}

/* -------------------------------------------------- Inline / Functions */

#include "animate/core/common/FCMPostConfig.h"