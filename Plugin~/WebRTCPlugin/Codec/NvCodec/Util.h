#pragma once
#include "GraphicsDevice/GraphicsDeviceType.h"

namespace unity
{
namespace webrtc
{
    bool LoadModule();
    void UnloadModule();
    bool CheckDriverVersion();
    bool IsSupportedGraphicsDevice(GraphicsDeviceType deviceType);
} // end namespace webrtc
} // end namespace unity
