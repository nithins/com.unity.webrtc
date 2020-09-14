#pragma once

#include <vulkan/vulkan.h>
#include <cuda.h>

namespace unity
{
namespace webrtc
{

class CudaContext {
public:
    CudaContext();
    ~CudaContext() = default;

    CUresult Init(const VkInstance instance, VkPhysicalDevice physicalDevice);
#if defined(_WIN32)
    CUresult Init(ID3D11Device* device);
#endif
    void Shutdown();
    CUcontext GetContextOnThread() const;
private:
    CUcontext m_context;

};

} // end namespace webrtc
} // end namespace unity
