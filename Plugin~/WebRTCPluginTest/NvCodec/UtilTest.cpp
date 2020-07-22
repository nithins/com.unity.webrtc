#include "pch.h"
#include "Codec/NvCodec/Util.h"

namespace unity
{
namespace webrtc
{
    
TEST(UtilTest, LoadModule)
{
    EXPECT_TRUE(LoadModule());
    UnloadModule();
}

TEST(UtilTest, DriverVersion)
{
    EXPECT_TRUE(LoadModule());
    EXPECT_TRUE(CheckDriverVersion());
    UnloadModule();
}

TEST(UtilTest, IsSupportedGraphicsDevice)
{
#ifdef UNITY_WIN
    EXPECT_TRUE(IsSupportedGraphicsDevice(kUnityGfxRendererD3D11));
    EXPECT_TRUE(IsSupportedGraphicsDevice(kUnityGfxRendererD3D12));
    EXPECT_FALSE(IsSupportedGraphicsDevice(kUnityGfxRendererVulkan));
    EXPECT_FALSE(IsSupportedGraphicsDevice(kUnityGfxRendererOpenGLCore));
#endif
#ifdef UNITY_LINUX
    EXPECT_TRUE(IsSupportedGraphicsDevice(kUnityGfxRendererOpenGLCore));
    EXPECT_FALSE(IsSupportedGraphicsDevice(kUnityGfxRendererVulkan));
#endif
#ifdef UNITY_MAC
    EXPECT_FALSE(IsSupportedGraphicsDevice(kUnityGfxRendererOpenGLCore));
    EXPECT_FALSE(IsSupportedGraphicsDevice(kUnityGfxRendererMetal));
#endif
}

} // end namespace webrtc
} // end namespace unity
