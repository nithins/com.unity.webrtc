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

} // end namespace webrtc
} // end namespace unity
