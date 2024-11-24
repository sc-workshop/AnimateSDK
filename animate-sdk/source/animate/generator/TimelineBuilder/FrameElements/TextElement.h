#pragma once

#include <string>
#include <math.h>

#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class TextElement {
	public:
		bool operator==(TextElement const& other) const;

		bool operator!=(TextElement const& other) const
		{
			return !operator==(other);
		}

	public:
		DOM::Utils::RECT bound;

		// IClassicText

		std::u16string text;
		DOM::FrameElement::AA_MODE_PROP renderingMode;
		FCM::Boolean isSelectable;

		// IClassicText -> IParagraph

		DOM::FrameElement::PARAGRAPH_STYLE style;

		// IClassicText -> IParagraph -> ITextRun -> ITextStyle

		DOM::Utils::COLOR fontColor = { 0xFF, 0xFF, 0xFF, 0xFF };
		uint16_t fontSize;
		std::string fontStyle;
		std::u16string fontName;
		FCM::Boolean autoKern;
		DOM::FrameElement::LineMode lineMode;

		// Glow filter
		bool isOutlined = false;
		DOM::Utils::COLOR outlineColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	};
}