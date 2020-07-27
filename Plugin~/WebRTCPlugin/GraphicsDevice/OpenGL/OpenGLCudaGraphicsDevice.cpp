#include "pch.h"
#include <cuda.h>
#include "OpenGLCudaGraphicsDevice.h"

#include "NvEncoderCuda.h"
#include "OpenGLCudaTexture2D.h"
#include "Codec/NvCodec/NvEncoderCudaWithCUarray.h"
#include "GraphicsDevice/GraphicsUtility.h"

namespace unity
{
namespace webrtc
{

OpenGLCudaGraphicsDevice::OpenGLCudaGraphicsDevice()
{
}

//---------------------------------------------------------------------------------------------------------------------
OpenGLCudaGraphicsDevice::~OpenGLCudaGraphicsDevice() {

}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLCudaGraphicsDevice::InitV()
{
    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err != GLEW_OK)
        return false;
#if _DEBUG
    GLuint unusedIds = 0;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OnOpenGLDebugMessage, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
#endif

    if (CUDA_SUCCESS != m_cudaContext.InitGL())
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------------------------------

void OpenGLCudaGraphicsDevice::ShutdownV() {
    m_cudaContext.Shutdown();
}

//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLCudaGraphicsDevice::CreateTextureV(
    uint32_t width, uint32_t height, void* tex)
{
    OpenGLCudaTexture2D* texture = new OpenGLCudaTexture2D(
        width, height, reinterpret_cast<uintptr_t>(tex));
    CUresult result = texture->Init();
    if(result != CUDA_SUCCESS)
    {
        texture->Shutdown();
        delete texture;
        return nullptr;
    }
    return texture;
}
//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLCudaGraphicsDevice::CreateDefaultTextureV(
    uint32_t width, uint32_t height) {

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glBindTexture(GL_TEXTURE_2D, 0);
    return CreateTextureV(width, height, reinterpret_cast<void*>(tex));
}

//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLCudaGraphicsDevice::CreateCPUReadTextureV(uint32_t w, uint32_t h)
{
    OpenGLCudaTexture2D* tex = static_cast<OpenGLCudaTexture2D*>(CreateDefaultTextureV(w, h));
//    tex->CreatePBO();
    return tex;
}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLCudaGraphicsDevice::CopyResourceV(ITexture2D* dest, ITexture2D* src) {
    const GLuint dstName = reinterpret_cast<uintptr_t>(dest->GetNativeTexturePtrV());
    const GLuint srcName = reinterpret_cast<uintptr_t>(src->GetNativeTexturePtrV());
    const uint32_t width = dest->GetWidth();
    const uint32_t height  = dest->GetHeight();
    return CopyResource(dstName, srcName, width, height);

}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLCudaGraphicsDevice::CopyResourceFromNativeV(
    ITexture2D* dest, void* nativeTexturePtr) {
    if (dest == nullptr)
        return false;
    if (nativeTexturePtr == nullptr)
        return false;

    const CUdeviceptr srcDevice = reinterpret_cast<CUdeviceptr>(nativeTexturePtr);
    const CUarray dstArray = static_cast<CUarray>(dest->GetEncodeTexturePtrV());
    if(dstArray == nullptr) {
        return false;
    }
    const uint32_t width = dest->GetWidth();
    const uint32_t height  = dest->GetHeight();
    const size_t byteCount = static_cast<size_t>(width * height * 4);

    /*
    CUcontext context = m_cudaContext.GetContext();
    NvEncoderCuda::CopyToDeviceFrame(
        context, srcDevice, width * 4, dstDevice, width * 4, width, height,
        CU_MEMORYTYPE_DEVICE, 
        )
    */
    CUresult result = cuMemcpyDtoA(dstArray, 0, srcDevice, byteCount);
    if(result != CUDA_SUCCESS) {
        return false;
    }
    return true;
}

bool OpenGLCudaGraphicsDevice::CopyTextureV(
    const NvEncInputFrame* dst, ITexture2D* src) {
    if (dst == nullptr)
        return false;
    if (src == nullptr)
        return false;

    void* pSrcFrame = src->GetEncodeTexturePtrV();
    CUdeviceptr pDstFrame = reinterpret_cast<CUdeviceptr>(dst->inputPtr);
    
    const uint32_t width = src->GetWidth();
    const uint32_t height = src->GetHeight();
    const uint32_t pitch = dst->pitch;

    CUcontext context = m_cudaContext.GetContext();
    NvEncoderCuda::CopyToDeviceFrame(
        context, pSrcFrame, 0, pDstFrame, dst->pitch, width, height,
        CU_MEMORYTYPE_DEVICE, dst->bufferFormat, dst->chromaOffsets,
        dst->numChromaPlanes);
    return true;
}


bool OpenGLCudaGraphicsDevice::CopyResource(GLuint dstName, GLuint srcName, uint32 width, uint32 height) {
    if(srcName == dstName)
    {
        LogPrint("Same texture");
        return false;
    }
    if(glIsTexture(srcName) == GL_FALSE)
    {
        LogPrint("srcName is not texture");
        return false;
    }
    if(glIsTexture(dstName) == GL_FALSE)
    {
        LogPrint("dstName is not texture");
        return false;
    }
    glCopyImageSubData(
            srcName, GL_TEXTURE_2D, 0, 0, 0, 0,
            dstName, GL_TEXTURE_2D, 0, 0, 0, 0,
            width, height, 1);
    return true;
}

rtc::scoped_refptr<webrtc::I420Buffer> OpenGLCudaGraphicsDevice::ConvertRGBToI420(ITexture2D* tex)
{
    //OpenGLCudaTexture2D* sourceTex = static_cast<OpenGLCudaTexture2D*>(tex);
    //const GLuint sourceId = reinterpret_cast<uintptr_t>(tex->GetNativeTexturePtrV());
    //const GLuint pbo = sourceTex->GetPBO();

    //// Send normal texture data to the PBO
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    //glBindTexture(GL_TEXTURE_2D, sourceId);
    //glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);

    //uint32_t buffSize = sourceTex->GetBufferSize();
    //byte* data = sourceTex->GetBuffer();

    //// Send PBO to main memory
    //GLubyte* pboPtr = static_cast<GLubyte*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
    //if (pboPtr)
    //{
    //    memcpy(data, pboPtr, buffSize);
    //    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    //}
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    //glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    //const uint32_t width = tex->GetWidth();
    //const uint32_t height = tex->GetHeight();
    //const uint32_t pitch = sourceTex->GetPitch();

    //rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = GraphicsUtility::ConvertRGBToI420Buffer(
    //    width, height, pitch, static_cast<uint8_t*>(data)
    //);
    //return i420_buffer;
    return nullptr;
}

} // end namespace webrtc
} // end namespace unity
