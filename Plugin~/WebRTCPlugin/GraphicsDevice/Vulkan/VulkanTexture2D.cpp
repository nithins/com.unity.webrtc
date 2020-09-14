#include "pch.h"
#include "VulkanTexture2D.h"


#include "WebRTCMacros.h"
#include "GraphicsDevice/Vulkan/VulkanUtility.h"

namespace unity
{
namespace webrtc
{

//---------------------------------------------------------------------------------------------------------------------

VulkanTexture2D::VulkanTexture2D(const uint32_t w, const uint32_t h) : ITexture2D(w,h),
    m_textureImage(VK_NULL_HANDLE), m_textureImageMemory(VK_NULL_HANDLE),
    m_textureImageMemorySize(0), m_device(VK_NULL_HANDLE),
    m_textureFormat(VK_FORMAT_B8G8R8A8_UNORM)
{
}

//---------------------------------------------------------------------------------------------------------------------

VulkanTexture2D::~VulkanTexture2D() {
    Shutdown();

}

//---------------------------------------------------------------------------------------------------------------------

void VulkanTexture2D::Shutdown()
{
    //[TODO-sin: 2019-11-20] Create an explicit Shutdown(device) function
    VULKAN_SAFE_DESTROY_IMAGE(m_device, m_textureImage, m_allocator);
    VULKAN_SAFE_FREE_MEMORY(m_device, m_textureImageMemory, m_allocator);
    m_textureImageMemorySize = 0;
    m_device = VK_NULL_HANDLE;

    m_cudaImage.Shutdown();    
}

//---------------------------------------------------------------------------------------------------------------------

bool VulkanTexture2D::Init(const VkPhysicalDevice physicalDevice, const VkDevice device) {
    m_physicalDevice = physicalDevice;
    m_device = device;

    const bool EXPORT_HANDLE = true;
    m_textureImageMemorySize = VulkanUtility::CreateImage(physicalDevice,device,m_allocator, m_width, m_height,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureFormat, &m_textureImage, &m_textureImageMemory,
        EXPORT_HANDLE
    );

    if (m_textureImageMemorySize <= 0) {
        return false;
    }

    if(CUDA_SUCCESS != m_cudaImage.Init(m_device, this))
    {
        return false;
    }
    UnityVulkanMemory unityVulkanMemory;
    unityVulkanMemory.memory = m_textureImageMemory;
    unityVulkanMemory.offset = 0;
    unityVulkanMemory.size = m_textureImageMemorySize;
    unityVulkanMemory.mapped = nullptr;
    unityVulkanMemory.flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    //    unityVulkanMemory.memoryTypeIndex = todo::(kazuki)

    m_unityVulkanImage.memory = unityVulkanMemory;
    m_unityVulkanImage.image = m_textureImage;
    m_unityVulkanImage.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_unityVulkanImage.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    m_unityVulkanImage.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    m_unityVulkanImage.format = VK_FORMAT_B8G8R8A8_UNORM;
    m_unityVulkanImage.extent.width = m_width;
    m_unityVulkanImage.extent.height = m_height;
    m_unityVulkanImage.tiling = VK_IMAGE_TILING_OPTIMAL;
    m_unityVulkanImage.type = VK_IMAGE_TYPE_2D;
    m_unityVulkanImage.samples = VK_SAMPLE_COUNT_1_BIT;

    return true;
}

} // end namespace webrtc
} // end namespace unity
