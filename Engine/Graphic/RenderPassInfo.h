#pragma once
#include "Graphic/Common.h"

namespace graphic {
union ClearValue
{
    float color[4];
    struct ClearDepthStencil
    {
        float depth;
        uint32_t stencil;
    } depth_stencil;
};

// Do not have Gpu resouce handle.
// We are using vulkan dynamic rendering.
struct RenderPassInfo {
    constexpr static u32 MAX_COLOR_ATTHACHEMNT_NUM = 8;
    enum class AttachmentLoadOp
    {
        LOAD,
        CLEAR,
        DONTCARE,
    };

    enum class AttachmentStoreOp
    {
        STORE,
        DONTCARE,
    };

    // Color images
    Ref<Image> colorAttachments[MAX_COLOR_ATTHACHEMNT_NUM];
    ClearValue clearColors[MAX_COLOR_ATTHACHEMNT_NUM] = {};
    AttachmentLoadOp colorAttatchemtsLoadOp[MAX_COLOR_ATTHACHEMNT_NUM];
    AttachmentStoreOp colorAttatchemtsStoreOp[MAX_COLOR_ATTHACHEMNT_NUM];
    u32 numColorAttachments = 0;

    // Resolve image. The declaration of color 
    // attachments and resolve attachments must match
    // to deduce source color image and the destination resolve image.
    Image* resolveAttatchments[MAX_COLOR_ATTHACHEMNT_NUM];
    u32 numResolveAttachments = 0;

    // Depth images
    Ref<Image> depthAttatchment = nullptr;
    AttachmentLoadOp depthAttachmentLoadOp = AttachmentLoadOp::LOAD;
    AttachmentStoreOp depthAttachmentStoreOp = AttachmentStoreOp::STORE;
    ClearValue ClearDepthStencil = {1.f, 0.f};
};
}