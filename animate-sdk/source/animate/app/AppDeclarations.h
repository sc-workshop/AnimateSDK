#pragma once

#include "animate/core/common/FCMMacros.h"

namespace Animate
{
	namespace Exporter
	{
		namespace Service
		{
			FORWARD_DECLARE_INTERFACE(IFrameCommandGenerator);
			FORWARD_DECLARE_INTERFACE(IResourcePalette);
			FORWARD_DECLARE_INTERFACE(ISWFExportService);
			FORWARD_DECLARE_INTERFACE(ITimelineBuilder);
			FORWARD_DECLARE_INTERFACE(ITimelineBuilder2);
			FORWARD_DECLARE_INTERFACE(ITimelineBuilder3);
			FORWARD_DECLARE_INTERFACE(ITimelineBuilderFactory);
		}
	}

	namespace Publisher
	{
		FORWARD_DECLARE_INTERFACE(IPublisher);
	}

	namespace Application
	{
		namespace Service
		{
			FORWARD_DECLARE_INTERFACE(IApplicationService);
			FORWARD_DECLARE_INTERFACE(IOutputConsoleService);
		}
	}

	namespace DocType
	{
		FORWARD_DECLARE_INTERFACE(IDocType);
		FORWARD_DECLARE_INTERFACE(IFeatureMatrix);
	}
}