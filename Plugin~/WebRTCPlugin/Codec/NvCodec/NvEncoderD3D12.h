#pragma once
#include "NvEncoder.h"
#include "NvEncoderD3D11.h"

class NvEncoderD3D12 : public NvEncoderD3D11
{
public:
    NvEncoderD3D12(ID3D12Device* pD3D12Device, uint32_t nWidth, uint32_t nHeight, NV_ENC_BUFFER_FORMAT eBufferFormat,
        uint32_t nExtraOutputDelay = 3, bool bMotionEstimationOnly = false, bool bOPInVideoMemory = false);
    virtual ~NvEncoderD3D12();

    ID3D12Resource* GetD3D12Resource(void* inputPtr);
protected:
    virtual void AllocateInputBuffers(int32_t numInputBuffers) override;
    virtual void ReleaseInputBuffers() override;
private:
    void ReleaseD3D12Resources();

protected:
    ID3D12Device* m_pD3D12Device = nullptr;
    ID3D11Device5* m_pD3D11Device5 = nullptr;
    std::map<ID3D11Texture2D*, ID3D12Resource*> m_mapD3D12Resource;
};
