#include "pch.h"
#include "nvEncodeAPI.h"
#include "NvEncoderD3D12.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"

namespace unity
{
namespace webrtc
{

    NvEncoderD3D12::NvEncoderD3D12(ID3D11Device* pD3D11Device, uint32_t nWidth, uint32_t nHeight,
        NV_ENC_BUFFER_FORMAT eBufferFormat, uint32_t nExtraOutputDelay, bool bMotionEstimationOnly, bool bOutputInVideoMemory) :
        NvEncoder(NV_ENC_DEVICE_TYPE_DIRECTX, pD3D11Device, nWidth, nHeight, eBufferFormat, nExtraOutputDelay, bMotionEstimationOnly, bOutputInVideoMemory)
    {
    }

    NvEncoderD3D12::~NvEncoderD3D12()
    {
    }

    void NvEncoderD3D12::AllocateInputBuffers(int32_t numInputBuffers) {};

    void NvEncoderD3D12::ReleaseInputBuffers() {};

} // end namespace webrtc
} // end namespace unity
