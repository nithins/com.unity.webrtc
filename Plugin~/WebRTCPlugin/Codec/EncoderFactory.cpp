#include "pch.h"
#include "IEncoder.h"
#include "EncoderFactory.h"


#include "UnityRenderEvent.h"
#include "SoftwareCodec/SoftwareEncoder.h"
#include "GraphicsDevice/IGraphicsDevice.h"

#if defined(UNITY_WIN) || defined(UNITY_LINUX)
#include "NvCodec/NvEncoderProxy.h"
#include "NvCodec/Util.h"
#endif

namespace unity
{
namespace webrtc
{

    EncoderFactory& EncoderFactory::GetInstance() {
        static EncoderFactory factory;
        return factory;
    }

    bool EncoderFactory::GetHardwareEncoderSupport()
    {
#if defined(UNITY_OSX)
        return false;
#else
        UnityGfxRenderer renderer = GetGfxRenderer();
        return IsSupportedGraphicsDevice(renderer);
#endif
    }

    //Can throw exception. The caller is expected to catch it.
    std::unique_ptr<IEncoder> EncoderFactory::Init(int width, int height, IGraphicsDevice* device, UnityEncoderType encoderType)
    {
        std::unique_ptr<IEncoder> encoder;
#if defined(UNITY_OSX)
        encoder = std::make_unique<SoftwareEncoder>(width, height, device);
#else
        GraphicsDeviceType deviceType = device->GetDeviceType();
        if (encoderType == UnityEncoderType::UnityEncoderHardware &&
            GetHardwareEncoderSupport())
        {
            encoder = std::make_unique<NvEncoderProxy>(width, height, device, deviceType);
        }
        else
        {
            encoder = std::make_unique<SoftwareEncoder>(width, height, device);
        }
#endif            
        encoder->InitV();
        return encoder;
    }
    
} // end namespace webrtc
} // end namespace unity
