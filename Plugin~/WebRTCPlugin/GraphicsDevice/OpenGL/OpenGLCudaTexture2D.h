#pragma once

#include "GraphicsDevice/ITexture2D.h"
#include "GraphicsDevice/Vulkan/Cuda/CudaImage.h"

namespace unity
{
namespace webrtc
{

struct OpenGLCudaTexture2D : ITexture2D {
public:
    OpenGLCudaTexture2D(uint32_t width, uint32_t height, GLuint tex);
    virtual ~OpenGLCudaTexture2D();

    CUresult Init();
    void Shutdown();

    inline virtual void* GetNativeTexturePtrV() override;
    inline virtual const void* GetNativeTexturePtrV() const  override;
    inline virtual void* GetEncodeTexturePtrV()  override;
    inline virtual const void* GetEncodeTexturePtrV() const override;

    size_t GetBufferSize() const { return m_width * m_height * 4; }
    size_t GetPitch() const { return m_width * 4; }
    GLuint GetPBO() const { return m_pbo; }
private:
    GLuint CreatePBO();
    GLuint m_texture;
    CudaImage m_cudaImage;

    GLuint m_pbo;
};

//---------------------------------------------------------------------------------------------------------------------

void* OpenGLCudaTexture2D::GetNativeTexturePtrV() {
    return reinterpret_cast<void*>(m_texture);
}
const void* OpenGLCudaTexture2D::GetNativeTexturePtrV() const {
    return reinterpret_cast<const void*>(m_texture);
};
void* OpenGLCudaTexture2D::GetEncodeTexturePtrV() {
    return reinterpret_cast<void*>(m_cudaImage.GetDevicePtr());
}
const void* OpenGLCudaTexture2D::GetEncodeTexturePtrV() const {
    return reinterpret_cast<const void*>(m_cudaImage.GetDevicePtr());
}

} // end namespace webrtd
} // end namespace unity
