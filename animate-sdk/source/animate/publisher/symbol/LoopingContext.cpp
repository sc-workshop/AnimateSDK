#include "LoopingContext.h"

#include "animate/publisher/wheelchair/AdobeWheelchair.h"

namespace Animate::Publisher {
    LoopingContext::LoopingContext() {
    }

    LoopingContext::LoopingContext(FCM::AutoPtr<DOM::FrameElement::IGraphic> graphic) {
        AdobeWheelchair& wheelchair = AdobeWheelchair::Instance();
        if (!wheelchair.IsWheelchairMoving() || !wheelchair.CPicGraphic_stageFrameEndOffset)
            return;

        // Let the magic happen
        DOM::FrameElement::IGraphic* wrapper = graphic.m_Ptr;

        uintptr_t* pic_graphic = nullptr;
        // offsets here should be the same across hompilers :hope:
        (*(void(**)(uint8_t*, uintptr_t**))(*((uintptr_t*) wrapper + 4) + 32LL))((uint8_t*) wrapper + 32, &pic_graphic);
        if (pic_graphic == nullptr)
            return;
        
        // Getting object vtable
        uintptr_t* pic_graphic_vtable = *(uintptr_t**) pic_graphic;
        // Calling to stageFrameEndOffset
        int64_t end_offset = (*(int64_t (**)(uintptr_t*))(pic_graphic_vtable + wheelchair.CPicGraphic_stageFrameEndOffset))(pic_graphic);

        // Getting the rest of props
        graphic->GetLoopMode(m_mode);
        graphic->GetFirstFrameIndex(m_start_frame);
        m_end_frame = (uint32_t) end_offset;
    }
}