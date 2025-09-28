#include "TextFieldGenerator.h"

namespace Animate::Publisher
{
	void TextFieldGenerator::Generate(wk::Ref<SharedTextFieldWriter> writer, FCM::AutoPtr<DOM::FrameElement::IClassicText> textfieldData)
	{
		FCM::PluginModule& context = FCM::PluginModule::Instance();

		{
			TextElement element;
			((FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement>)textfieldData)->GetObjectSpaceBounds(element.bound);

			element.text = context.falloc->GetString16(
					textfieldData.m_Ptr,
					&DOM::FrameElement::IClassicText::GetText
			);

			element.renderingMode.structSize = sizeof(element.renderingMode);
			textfieldData->GetAntiAliasModeProp(element.renderingMode);

			FCM::AutoPtr<DOM::FrameElement::ITextBehaviour> textfieldElementBehaviour;
			textfieldData->GetTextBehaviour(textfieldElementBehaviour.m_Ptr);

			textfieldElementBehaviour->IsSelectable(element.isSelectable);

			FCM::AutoPtr<DOM::FrameElement::IModifiableTextBehaviour> modifiableTextfieldBehaviour = textfieldElementBehaviour;
			if (modifiableTextfieldBehaviour) {
				modifiableTextfieldBehaviour->GetLineMode(element.lineMode);
			}

			writer->Initialize(element);
		}
		
		FCM::FCMListPtr paragraphs;
		uint32_t paragraphsCount = 0;
		textfieldData->GetParagraphs(paragraphs.m_Ptr);
		paragraphs->Count(paragraphsCount);

		for (uint32_t i = 0; paragraphsCount > i; i++)
		{
			TextParagraph paragraph;
			FCM::AutoPtr<DOM::FrameElement::IParagraph> paragraphData = paragraphs[i];
			paragraph.style.structSize = sizeof(paragraph.style);
			paragraphData->GetParagraphStyle(paragraph.style);

			FCM::FCMListPtr textRuns;
			uint32_t textRunCount = 0;
			paragraphData->GetTextRuns(textRuns.m_Ptr);
			textRuns->Count(textRunCount);

			for (uint32_t t = 0; textRunCount > i; i++)
			{
				auto& textRun = paragraph.textRuns.emplace_back();
				FCM::AutoPtr<DOM::FrameElement::ITextRun> textRunData = textRuns[t];
				FCM::AutoPtr<DOM::FrameElement::ITextStyle> textStyle;
				textRunData->GetTextStyle(textStyle.m_Ptr);

				textStyle->GetFontColor(textRun.fontColor);
				textStyle->GetFontSize(textRun.fontSize);
				textStyle->IsAutoKernEnabled(textRun.autoKern);

				textRun.fontName = context.falloc->GetString16(
					textStyle.m_Ptr,
					&DOM::FrameElement::ITextStyle::GetFontName
				);

				FCM::StringRep8 fontStylePtr;
				textStyle->GetFontStyle(fontStylePtr);
				textRun.fontStyle = std::string((const char*)fontStylePtr);
				context.falloc->Free(fontStylePtr);
			}

			writer->AddParagraph(paragraph);
		}
	}
}