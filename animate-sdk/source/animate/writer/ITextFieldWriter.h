#pragma once

#include <memory>
#include <vector>

#include "AnimateDOM.h"

#include "IDisplayObjectWriter.h"

namespace Animate::Publisher
{
	struct TextRun
	{
		DOM::Utils::COLOR fontColor = { 0xFF, 0xFF, 0xFF, 0xFF };
		uint16_t fontSize;
		std::string fontStyle;
		std::u16string fontName;
		FCM::Boolean autoKern;
	};

	struct TextParagraph
	{
		DOM::FrameElement::PARAGRAPH_STYLE style;
		std::vector<TextRun> textRuns;
	};

	struct TextElement {
		DOM::Utils::RECT bound;

		std::u16string text;
		DOM::FrameElement::AA_MODE_PROP renderingMode;
		FCM::Boolean isSelectable;
		DOM::FrameElement::LineMode lineMode;
	};

	class SharedTextFieldWriter : public IDisplayObjectWriter {
	public:
		SharedTextFieldWriter(SymbolContext& context) : IDisplayObjectWriter(context) {};
		virtual ~SharedTextFieldWriter() = default;

	public:
		virtual void Initialize(const TextElement& textfield) = 0;
		virtual void AddParagraph(const TextParagraph& paragraph) = 0;
	};
}