#pragma once
#include <IUnityProfiler.h>
#include "IGraphicsDevice.h"


namespace unity
{
namespace webrtc
{

class GraphicsUtility {
public:
    static rtc::scoped_refptr<::webrtc::I420Buffer> ConvertRGBToI420Buffer(
        const uint32_t width, const uint32_t height,
        const uint32_t rowToRowInBytes, const uint8_t* srcData);
    static IGraphicsDevice* GetGraphicsDevice();
    static bool IsHWCodecSupportedDevice();
};

//class IUnityProfiler;
IUnityProfiler* GetProfiler();

} // end namespace webrtc
} // end namespace unity
