#include "pch.h"
#include "OpenGLCudaTexture2D.h"

#include <cuda.h>

namespace unity
{
namespace webrtc
{

//---------------------------------------------------------------------------------------------------------------------

OpenGLCudaTexture2D::OpenGLCudaTexture2D(
    uint32_t width, uint32_t height, GLuint tex)
    : ITexture2D(width,height) , m_texture(tex), m_pbo(0)
{
}

OpenGLCudaTexture2D::~OpenGLCudaTexture2D()
{
    Shutdown();
}

CUresult OpenGLCudaTexture2D::Init()
{
    m_pbo = CreatePBO();
    return m_cudaImage.Init(m_pbo);
}

void OpenGLCudaTexture2D::Shutdown()
{
    m_cudaImage.Shutdown();
}

GLuint OpenGLCudaTexture2D::CreatePBO()
{
    GLuint pbo = 0;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);

    const size_t bufferSize = GetBufferSize();
    glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    return pbo;

    //if(m_buffer != nullptr)
    //{
    //    free(m_buffer);
    //}
    //m_buffer = static_cast<byte*>(malloc(bufferSize));
}


} // end namespace webrtc
} // end namespace unity
