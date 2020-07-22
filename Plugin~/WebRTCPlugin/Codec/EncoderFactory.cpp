#include "pch.h"
#include "IEncoder.h"
#include "EncoderFactory.h"


#include "UnityRenderEvent.h"
#include "SoftwareCodec/SoftwareEncoder.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "NvCodec/NvEncoderProxy.h"
#include "NvCodec/Util.h"

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
        UnityGfxRenderer renderer = GetGfxRenderer();
        return IsSupportedGraphicsDevice(renderer);
    }

    //Can throw exception. The caller is expected to catch it.
    std::unique_ptr<IEncoder> EncoderFactory::Init(int width, int height, IGraphicsDevice* device, UnityEncoderType encoderType)
    {
        std::unique_ptr<IEncoder> encoder;
#if defined(SUPPORT_METAL) && defined(SUPPORT_SOFTWARE_ENCODER)
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
