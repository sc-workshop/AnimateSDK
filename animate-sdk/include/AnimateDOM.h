#pragma once

#include "AnimateCore.h"

// Why is this in Utils?..
// Well ok... i guess...
#include "animate/app/DOM/Utils/DOMTypes.h"
#include "animate/app/DOM/Utils/IMatrix2D.h"
#include "animate/app/DOM/Utils/IRect.h"

// Root
#include "animate/app/DOM/ICamera.h"

#include "animate/app/DOM/IFilterable.h"

#include "animate/app/DOM/IFLADocument.h"

#include "animate/app/DOM/IFrame.h"
#include "animate/app/DOM/IFrame1.h"

#include "animate/app/DOM/ILayer.h"
#include "animate/app/DOM/ILayer1.h"
#include "animate/app/DOM/ILayer2.h"

#include "animate/app/DOM/ILibraryItem.h"

#include "animate/app/DOM/ITimeline.h"
#include "animate/app/DOM/ITimeline1.h"

#include "animate/app/DOM/ITween.h"

// Fill Style
#include "animate/app/DOM/FillStyle/IBitmapFillStyle.h"
#include "animate/app/DOM/FillStyle/IGradientFillStyle.h"
#include "animate/app/DOM/FillStyle/ISolidFillStyle.h"

// Frame Elements
#include "animate/app/DOM/FrameElement/IBitmapInstance.h"
#include "animate/app/DOM/FrameElement/IButton.h"
#include "animate/app/DOM/FrameElement/IClassicText.h"
#include "animate/app/DOM/FrameElement/IComponentInstance.h"
#include "animate/app/DOM/FrameElement/IFrameDisplayElement.h"
#include "animate/app/DOM/FrameElement/IGraphic.h"
#include "animate/app/DOM/FrameElement/IGroup.h"
#include "animate/app/DOM/FrameElement/IInstance.h"
#include "animate/app/DOM/FrameElement/IMovieClip.h"
#include "animate/app/DOM/FrameElement/IParagraph.h"
#include "animate/app/DOM/FrameElement/IShape.h"
#include "animate/app/DOM/FrameElement/ISound.h"
#include "animate/app/DOM/FrameElement/ISymbolInstance.h"
#include "animate/app/DOM/FrameElement/ITextBehaviour.h"
#include "animate/app/DOM/FrameElement/ITextRun.h"
#include "animate/app/DOM/FrameElement/ITextStyle.h"

// Graphic Filters
#include "animate/app/DOM/GraphicFilter/IAdjustColorFilter.h"
#include "animate/app/DOM/GraphicFilter/IBevelFilter.h"
#include "animate/app/DOM/GraphicFilter/IBlurFilter.h"
#include "animate/app/DOM/GraphicFilter/IDropShadowFilter.h"
#include "animate/app/DOM/GraphicFilter/IGlowFilter.h"
#include "animate/app/DOM/GraphicFilter/IGradientBevelFilter.h"
#include "animate/app/DOM/GraphicFilter/IGradientGlowFilter.h"

// Library items
#include "animate/app/DOM/LibraryItem/IComponentItem.h"
#include "animate/app/DOM/LibraryItem/IFolderItem.h"
#include "animate/app/DOM/LibraryItem/IFontItem.h"
#include "animate/app/DOM/LibraryItem/IMediaItem.h"
#include "animate/app/DOM/LibraryItem/ISymbolItem.h"

// Media Info
#include "animate/app/DOM/MediaInfo/IBitmapInfo.h"
#include "animate/app/DOM/MediaInfo/ISoundInfo.h"

// Stroke Style
#include "animate/app/DOM/StrokeStyle/IDashedStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IDottedStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IHatchedStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IPaintbrushStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IRaggedStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/ISolidStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IStippleStrokeStyle.h"
#include "animate/app/DOM/StrokeStyle/IStrokeWidth.h"

// Utilities
#include "animate/app/DOM/Utils/ILinearColorGradient.h"
#include "animate/app/DOM/Utils/IRadialColorGradient.h"
#include "animate/app/DOM/Utils/ISoundEffect.h"
