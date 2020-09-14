#pragma once

#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/Cuda/CudaContext.h"
#include "WebRTCConstants.h"

namespace unity
{
namespace webrtc
{

namespace webrtc = ::webrtc;

class VulkanGraphicsDevice : public IGraphicsDevice
{
public:
    VulkanGraphicsDevice( IUnityGraphicsVulkan* unityVulkan, const VkInstance instance,
        const VkPhysicalDevice physicalDevice, const VkDevice device,
        const VkQueue graphicsQueue, const uint32_t queueFamilyIndex);

    virtual ~VulkanGraphicsDevice() = default;
    virtual bool InitV() override;
    virtual void ShutdownV() override;
    inline virtual void* GetEncodeDevicePtrV() override;
    virtual ITexture2D* CreateDefaultTextureV(const uint32_t w, const uint32_t h) override;
    virtual ITexture2D* CreateCPUReadTextureV(uint32_t width, uint32_t height) override;

    virtual bool CopyResourceV(ITexture2D* dest, ITexture2D* src) override;

    virtual NativeTexPtr ConvertNativeFromUnityPtr(void* tex) override;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="dest"></param>
    /// <param name="nativeTexturePtr"> a pointer of UnityVulkanImage </param>
    /// <returns></returns>
    virtual bool CopyResourceFromNativeV(ITexture2D* dest, void* nativeTexturePtr) override;
    inline virtual GraphicsDeviceType GetDeviceType() const override;
    inline virtual UnityGfxRenderer GetGfxRenderer() const override;

    virtual rtc::scoped_refptr<webrtc::I420Buffer> ConvertRGBToI420(ITexture2D* tex) override;

    virtual bool IsNvCodecSupport() override { return m_nvCodecSupport; }
    virtual CUcontext GetCuContext() override { return m_cudaContext.GetContextOnThread(); }
    virtual NV_ENC_BUFFER_FORMAT GetEncodeBufferFormat() override { return NV_ENC_BUFFER_FORMAT_ARGB; }


private:

    VkResult CreateCommandPool();

    IUnityGraphicsVulkan*   m_unityVulkan;
    VkInstance              m_instance;
    VkPhysicalDevice        m_physicalDevice;
    VkDevice                m_device;
    VkQueue                 m_graphicsQueue;
    VkCommandPool           m_commandPool;

    bool m_nvCodecSupport;
    CudaContext m_cudaContext;
    uint32_t m_queueFamilyIndex;

    const VkAllocationCallbacks* m_allocator = nullptr;

};

//---------------------------------------------------------------------------------------------------------------------

void* VulkanGraphicsDevice::GetEncodeDevicePtrV() { return reinterpret_cast<void*>(m_cudaContext.GetContextOnThread()); }
GraphicsDeviceType VulkanGraphicsDevice::GetDeviceType() const { return GRAPHICS_DEVICE_VULKAN; }
UnityGfxRenderer VulkanGraphicsDevice::GetGfxRenderer() const { return kUnityGfxRendererVulkan; }


} // end namespace webrtc
} // end namespace unity
