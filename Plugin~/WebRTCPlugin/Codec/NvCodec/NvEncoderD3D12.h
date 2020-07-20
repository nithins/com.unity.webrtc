#pragma once
#include "NvEncoder.h"

namespace unity
{
namespace webrtc
{
    class ITexture2D;
    class IGraphicsDevice;

    class NvEncoderD3D12 : public NvEncoder
    {
    public:
        NvEncoderD3D12(ID3D11Device* pD3D11Device, uint32_t nWidth, uint32_t nHeight,
            NV_ENC_BUFFER_FORMAT eBufferFormat, uint32_t nExtraOutputDelay, bool bMotionEstimationOnly, bool bOutputInVideoMemory);
        virtual ~NvEncoderD3D12();
    protected:

        void AllocateInputBuffers(int32_t numInputBuffers) override;
        void ReleaseInputBuffers() override;
    };

} // end namespace webrtc
} // end namespace unity
