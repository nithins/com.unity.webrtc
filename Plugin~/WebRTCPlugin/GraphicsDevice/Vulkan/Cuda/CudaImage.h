#pragma once

#include "cuda.h"
#include <vulkan/vulkan.h>

namespace unity
{
namespace webrtc
{

class VulkanTexture2D;

// Maps a 2D CUDA array on the device memory object referred to by deviceMem. deviceMem should have been created with a
// device memory object backing a 2D VkImage. This mapping makes use of Vulkan's
// export of device memory followed by import of this external memory by CUDA.

class CudaImage
{
public:
    CudaImage();
    ~CudaImage() = default;
    CUresult Init(const VkDevice device, const VulkanTexture2D* texture);
    CUresult Init(GLuint texture);
    void Shutdown();
    inline CUarray GetArray() const;
    inline CUdeviceptr GetDevicePtr() const;

private:
    CUarray m_array;
    CUdeviceptr m_devicePtr;
    CUgraphicsResource m_graphicsResource;
    CUmipmappedArray m_mipmapArray;
    CUexternalMemory m_extMemory;
};

//---------------------------------------------------------------------------------------------------------------------
    inline CUarray CudaImage::GetArray() const { return m_array; }
    inline CUdeviceptr CudaImage::GetDevicePtr() const { return m_devicePtr; }

} // end namespace webrtc
} // end namespace unity
