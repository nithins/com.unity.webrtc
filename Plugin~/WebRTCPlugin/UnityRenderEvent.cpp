#include "pch.h"

#include <IUnityGraphics.h>
#include <IUnityProfiler.h>

#include "Context.h"
#include "GraphicsDevice/GraphicsDevice.h"
#include "GraphicsDevice/GraphicsUtility.h"
#include "UnityVideoTrackSource.h"

enum class VideoStreamRenderEventID
{
    Initialize = 0,
    Encode = 1,
    Finalize = 2
};

using namespace unity::webrtc;
using namespace ::webrtc;

namespace unity
{
namespace webrtc
{
    IUnityInterfaces* s_UnityInterfaces = nullptr;
    IUnityGraphics* s_Graphics = nullptr;
    IUnityProfiler* s_UnityProfiler = nullptr;
    Clock* s_clock = Clock::GetRealTimeClock();
    Context* s_context = nullptr;
    IGraphicsDevice* s_device;

    const UnityProfilerMarkerDesc* s_MarkerEncode = nullptr;
    bool s_IsDevelopmentBuild = false;

    IGraphicsDevice* GraphicsUtility::GetGraphicsDevice()
    {
        GraphicsDevice& instance = GraphicsDevice::GetInstance();
        if (!instance.IsInitialized())
        {
            instance.Init(s_UnityInterfaces);
        }
        return instance.GetDevice();
    }

    IUnityProfiler* GetProfiler()
    {
        return s_UnityProfiler;
    }

    bool GraphicsUtility::IsHWCodecSupportedDevice()
    {
        // todo::(kazuki)
        return true;
    }

} // end namespace webrtc
} // end namespace unity

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
    case kUnityGfxDeviceEventInitialize:
    {
        break;
    }
    case kUnityGfxDeviceEventShutdown:
    {
        //UnityPluginUnload not called normally
        s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
        break;
    }
    case kUnityGfxDeviceEventBeforeReset:
    {
        break;
    }
    case kUnityGfxDeviceEventAfterReset:
    {
        break;
    }
    };
}
// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = unityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    s_UnityProfiler = unityInterfaces->Get<IUnityProfiler>();
    if (s_UnityProfiler != nullptr)
    {
        s_IsDevelopmentBuild = s_UnityProfiler->IsAvailable() != 0;
        s_UnityProfiler->CreateMarker(&s_MarkerEncode, "Encode", kUnityProfilerCategoryRender, kUnityProfilerMarkerFlagDefault, 0);
    }

    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID, void* data)
{
    if (s_context == nullptr)
        return;
    std::lock_guard<std::mutex> lock(ContextManager::GetInstance()->mutex);
    if(!ContextManager::GetInstance()->Exists(s_context))
        return;
    UnityVideoTrackSource* source =
        static_cast<UnityVideoTrackSource*>(data);
    VideoStreamRenderEventID event =
        static_cast<VideoStreamRenderEventID>(eventID);

    if(!s_context->ExistsVideoSource(source))
    {
        return;
    }
    switch(event)
    {
        case VideoStreamRenderEventID::Initialize:
        {
            if (!GraphicsDevice::GetInstance().IsInitialized())
            {
                GraphicsDevice::GetInstance().Init(s_UnityInterfaces);
            }
            s_device = GraphicsDevice::GetInstance().GetDevice();

            source->Init();
            return;
        }
        case VideoStreamRenderEventID::Encode:
        {
            if (s_IsDevelopmentBuild)
                s_UnityProfiler->BeginSample(s_MarkerEncode);

            const int64_t timestamp_us = s_clock->TimeInMicroseconds();
            source->OnFrameCaptured(timestamp_us);

            if (s_IsDevelopmentBuild)
                s_UnityProfiler->EndSample(s_MarkerEncode);

            return;
        }
        case VideoStreamRenderEventID::Finalize:
        {
            return;
        }
        default: {
            LogPrint("Unknown event id %d", eventID);
            return;
        }
    }
}

extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc(Context* context)
{
    s_context = context;
    return OnRenderEvent;
}
