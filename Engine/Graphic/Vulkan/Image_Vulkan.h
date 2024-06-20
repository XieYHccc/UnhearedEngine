#pragma once
#include "Graphic/Vulkan/Common_Vulkan.h"
#include "Graphic/Image.h"

namespace graphic {

constexpr VkImageLayout ConvertImageLayout(ImageLayout layout)
{
    switch (layout) {
    case ImageLayout::UNDEFINED:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    case ImageLayout::GENERAL:
        return VK_IMAGE_LAYOUT_GENERAL;
    case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case ImageLayout::TRANSFER_SRC_OPTIMAL:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case ImageLayout::TRANSFER_DST_OPTIMAL:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case ImageLayout::PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
}

constexpr VkImageType ConvertImageType(ImageType type)
{
    switch (type) 
    {
    case ImageType::TYPE_2D:
        return VK_IMAGE_TYPE_2D;
    case ImageType::TYPE_CUBE:
        return VK_IMAGE_TYPE_2D;
    case ImageType::TYPE_3D:
        return VK_IMAGE_TYPE_3D;
    }
}
constexpr VkSampleCountFlagBits ConvertSampleBits(ImageSamples sample)
{
    switch (sample) 
    {
    case ImageSamples::SAMPLES_1:
        return VK_SAMPLE_COUNT_1_BIT;
    case ImageSamples::SAMPLES_2:
        return VK_SAMPLE_COUNT_2_BIT;
    case ImageSamples::SAMPLES_4:
        return VK_SAMPLE_COUNT_4_BIT;
    case ImageSamples::SAMPLES_8:
        return VK_SAMPLE_COUNT_8_BIT;
    case ImageSamples::SAMPLES_16:
        return VK_SAMPLE_COUNT_16_BIT;
    case ImageSamples::SAMPLES_32:
        return VK_SAMPLE_COUNT_32_BIT;
    case ImageSamples::SAMPLES_64:
        return VK_SAMPLE_COUNT_64_BIT;
    default:
        return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
}

class Image_Vulkan : public Image {
    friend class Device_Vulkan;
    friend class CommandList_Vulkan;
public:
    Image_Vulkan(const ImageDesc& desc); // used for fill swapchain image infomation
    Image_Vulkan(Device_Vulkan* device, const ImageDesc& desc, const ImageInitData* init_data);
    virtual ~Image_Vulkan();
    
private:
    Device_Vulkan* device_;
    VkImage handle_;
    VmaAllocation allocation_;
    VkImageView view_;
    bool isSwapChainImage_;
};

CONVERT_TO_VULKAN_INTERNAL(Image)
}