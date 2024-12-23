#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher
{
	class ResourcePublisher;

	class TextFieldGenerator {
	public:
		static void Generate(SharedTextFieldWriter* writer, FCM::AutoPtr<DOM::FrameElement::IClassicText> textFieldData);
	};
}