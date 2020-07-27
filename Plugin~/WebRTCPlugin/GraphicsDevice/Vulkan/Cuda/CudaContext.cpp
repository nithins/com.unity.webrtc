#include "pch.h"

#include "CudaContext.h"
#include <cudaGL.h>

#include <array>
#include "GraphicsDevice/Vulkan/VulkanUtility.h"

namespace unity
{
namespace webrtc
{

static void* s_hModule = nullptr;

CudaContext::CudaContext() : m_context(nullptr) {
}

//---------------------------------------------------------------------------------------------------------------------
CUresult CudaContext::Init() {
    // dll check
    if (s_hModule == nullptr)
    {
        // dll delay load
#if defined(_WIN32)
        HMODULE module = LoadLibrary(TEXT("nvcuda.dll"));
        if (module == nullptr)
        {
            LogPrint("nvcuda.dll is not found. Please be sure the environment supports CUDA API.");
            return CUDA_ERROR_NOT_FOUND;
        }
        s_hModule = module;
#else
#endif
    }

    return cuInit(0);
}

CUresult CudaContext::InitGL() {

    CUresult result = Init();
    if (result != CUDA_SUCCESS) {
        return result;
    }

    int numDevices;
    result = cuDeviceGetCount(&numDevices);
    if (CUDA_SUCCESS != result) {
        return result;
    }
    if(numDevices == 0) {
        return CUDA_ERROR_NO_DEVICE;
    }

    // TODO:: check GPU capability 
    int cuDevId = 0;
    CUdevice device;
    result = cuDeviceGet(&device, cuDevId);
    if (CUDA_SUCCESS != result) {
        return result;
    }

    result = cuCtxCreate(&m_context, 0, device);
    if (CUDA_SUCCESS != result) {
        return result;
    }
}

CUresult CudaContext::Init(const VkInstance instance, VkPhysicalDevice physicalDevice) {

    CUresult result = Init();
    if (result != CUDA_SUCCESS) {
        return result;
    }

    int numDevices = 0;
    result = cuDeviceGetCount(&numDevices);
    if (result != CUDA_SUCCESS) {
        return result;
    }

    CUuuid id = {};
    std::array<uint8_t, VK_UUID_SIZE> deviceUUID;
    if (!VulkanUtility::GetPhysicalDeviceUUIDInto(instance, physicalDevice, &deviceUUID)) {
        return CUDA_ERROR_INVALID_DEVICE;
    }

    //Loop over the available devices and identify the CUdevice corresponding to the physical device in use by
    //this Vulkan instance. This is required because there is no other way to match GPUs across API boundaries.
    CUdevice dev;
    bool foundDevice = true;

    for (int i = 0; i < numDevices; i++) {
        cuDeviceGet(&dev, i);

        cuDeviceGetUuid(&id, dev);

        if (!std::memcmp(static_cast<const void *>(&id),
                static_cast<const void *>(deviceUUID.data()),
                sizeof(CUuuid))) {
            foundDevice = true;
            break;
        }
    }

    if (!foundDevice) {
        return CUDA_ERROR_NO_DEVICE;
 
    }

    result = cuCtxCreate(&m_context, 0, dev);
    return result;
}

//---------------------------------------------------------------------------------------------------------------------

void CudaContext::Shutdown() {
    if (nullptr != m_context) {
        cuCtxDestroy(m_context);
        m_context = nullptr;
    }
    if (s_hModule)
    {
#if _WIN32
        FreeLibrary((HMODULE)s_hModule);
#else
#endif
        s_hModule = nullptr;
    }
}

} // end namespace webrtc
} // end namespace unity
