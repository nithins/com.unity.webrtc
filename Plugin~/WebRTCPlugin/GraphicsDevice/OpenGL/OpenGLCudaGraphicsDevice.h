#pragma once

#include "GraphicsDevice/IGraphicsDevice.h"
#include "WebRTCConstants.h"
#include "GraphicsDevice/Vulkan/Cuda/CudaContext.h"

namespace unity
{
namespace webrtc
{

namespace webrtc = ::webrtc;

class OpenGLCudaGraphicsDevice : public IGraphicsDevice{
public:
    OpenGLCudaGraphicsDevice();
    virtual ~OpenGLCudaGraphicsDevice();

    virtual bool InitV() override;
    virtual void ShutdownV() override;
    virtual ITexture2D* CreateTextureV(uint32_t width, uint32_t height, void* tex) override;
    inline virtual void* GetEncodeDevicePtrV() override;

    virtual ITexture2D* CreateDefaultTextureV(uint32_t w, uint32_t h) override;
    virtual ITexture2D* CreateCPUReadTextureV(uint32_t width, uint32_t height) override;
    virtual bool CopyResourceV(ITexture2D* dest, ITexture2D* src) override;
    virtual bool CopyTextureV(const NvEncInputFrame* dst, ITexture2D* src) override;

    virtual rtc::scoped_refptr<webrtc::I420Buffer> ConvertRGBToI420(ITexture2D* tex) override;
    virtual bool CopyResourceFromNativeV(ITexture2D* dest, void* nativeTexturePtr) override;
    inline virtual GraphicsDeviceType GetDeviceType() const override;

private:
    bool CopyResource(GLuint dstName, GLuint srcName, uint32 width, uint32 height);
    CudaContext m_cudaContext;
    GLuint m_pbo = 0;
};

void* OpenGLCudaGraphicsDevice::GetEncodeDevicePtrV() { return reinterpret_cast<void*>(m_cudaContext.GetContext()); }
GraphicsDeviceType OpenGLCudaGraphicsDevice::GetDeviceType() const { return GRAPHICS_DEVICE_OPENGL; }

//---------------------------------------------------------------------------------------------------------------------
} // end namespace webrtc
} // end namespace unity
