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
    EXPECT_TRUE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_D3D11));
    EXPECT_TRUE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_D3D12));
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_VULKAN));
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_OPENGL));
#endif
#ifdef UNITY_LINUX
    EXPECT_TRUE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_OPENGL));
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_VULKAN));
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_METAL));
#endif
#ifdef UNITY_MAC
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_OPENGL));
    EXPECT_FALSE(IsSupportedGraphicsDevice(GRAPHICS_DEVICE_METAL));
#endif
}

} // end namespace webrtc
} // end namespace unity
