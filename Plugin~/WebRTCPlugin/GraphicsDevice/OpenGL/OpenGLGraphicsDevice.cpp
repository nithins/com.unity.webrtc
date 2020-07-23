#include "pch.h"
#include "OpenGLGraphicsDevice.h"
#include "OpenGLTexture2D.h"
#include "GraphicsDevice/GraphicsUtility.h"

namespace unity
{
namespace webrtc
{

OpenGLGraphicsDevice::OpenGLGraphicsDevice()
{
}

//---------------------------------------------------------------------------------------------------------------------
OpenGLGraphicsDevice::~OpenGLGraphicsDevice() {

}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLGraphicsDevice::InitV()
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
    return true;
}

//---------------------------------------------------------------------------------------------------------------------

void OpenGLGraphicsDevice::ShutdownV() {

}

//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLGraphicsDevice::CreateTextureV(uint32_t width, uint32_t height, void* tex)
{
    return new OpenGLTexture2D(width, height, reinterpret_cast<uintptr_t>(tex));
}
//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLGraphicsDevice::CreateDefaultTextureV(uint32_t w, uint32_t h) {

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
    glBindTexture(GL_TEXTURE_2D, 0);
    return new OpenGLTexture2D(w, h, tex);
}

//---------------------------------------------------------------------------------------------------------------------
ITexture2D* OpenGLGraphicsDevice::CreateCPUReadTextureV(uint32_t w, uint32_t h)
{
    OpenGLTexture2D* tex = static_cast<OpenGLTexture2D*>(CreateDefaultTextureV(w, h));
    tex->CreatePBO();
    return tex;
}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLGraphicsDevice::CopyResourceV(ITexture2D* dest, ITexture2D* src) {
    const GLuint dstName = reinterpret_cast<uintptr_t>(dest->GetNativeTexturePtrV());
    const GLuint srcName = reinterpret_cast<uintptr_t>(src->GetNativeTexturePtrV());
    const uint32_t width = dest->GetWidth();
    const uint32_t height  = dest->GetHeight();
    return CopyResource(dstName, srcName, width, height);

}

//---------------------------------------------------------------------------------------------------------------------
bool OpenGLGraphicsDevice::CopyResourceFromNativeV(ITexture2D* dest, void* nativeTexturePtr) {
    const GLuint dstName = reinterpret_cast<uintptr_t>(dest->GetNativeTexturePtrV());
    const uint32_t width = dest->GetWidth();
    const uint32_t height  = dest->GetHeight();

    GLuint srcName = reinterpret_cast<uintptr_t>(nativeTexturePtr);
    return CopyResource(dstName, srcName, width, height);
}

bool OpenGLGraphicsDevice::CopyResource(GLuint dstName, GLuint srcName, uint32 width, uint32 height) {
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

rtc::scoped_refptr<webrtc::I420Buffer> OpenGLGraphicsDevice::ConvertRGBToI420(ITexture2D* tex)
{
    OpenGLTexture2D* sourceTex = static_cast<OpenGLTexture2D*>(tex);
    const GLuint sourceId = reinterpret_cast<uintptr_t>(tex->GetNativeTexturePtrV());
    const GLuint pbo = sourceTex->GetPBO();

    // Send normal texture data to the PBO
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBindTexture(GL_TEXTURE_2D, sourceId);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);

    uint32_t buffSize = sourceTex->GetBufferSize();
    byte* data = sourceTex->GetBuffer();

    // Send PBO to main memory
    GLubyte* pboPtr = static_cast<GLubyte*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
    if (pboPtr)
    {
        memcpy(data, pboPtr, buffSize);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    const uint32_t width = tex->GetWidth();
    const uint32_t height = tex->GetHeight();
    const uint32_t pitch = sourceTex->GetPitch();

    rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = GraphicsUtility::ConvertRGBToI420Buffer(
        width, height, pitch, static_cast<uint8_t*>(data)
    );
    return i420_buffer;
}

} // end namespace webrtc
} // end namespace unity
