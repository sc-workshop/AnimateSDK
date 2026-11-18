#pragma once

#include "AnimateDOM.h"
#include "AnimateWriter.h"
#include "animate/publisher/symbol/SymbolContext.h"

namespace Animate::Publisher {
    class ResourcePublisher;

    class TextFieldGenerator {
    public:
        static void Generate(wk::Ref<SharedTextFieldWriter> writer, const FCM::AutoPtr<DOM::FrameElement::IClassicText>& textFieldData);
    };
}