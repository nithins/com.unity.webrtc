#include "pch.h"
#include "nvEncodeAPI.h"
#include "NvEncoderD3D12.h"

#include "GraphicsDevice/D3D12/D3D12Constants.h"

ID3D11Device5* CreateD3D11Device5()
{
    ID3D11Device* legacyDevice;
    if (D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        0, nullptr, 0, D3D11_SDK_VERSION,
        &legacyDevice, nullptr, nullptr) != S_OK)
    {
        NVENC_THROW_ERROR("D3D11CreateDevice failed", NV_ENC_ERR_INVALID_DEVICE);
    }

    ID3D11Device5* pD3D11Device = nullptr;
    if (legacyDevice->QueryInterface(IID_PPV_ARGS(&pD3D11Device)) != S_OK)
    {
        NVENC_THROW_ERROR("QueryInterface failed", NV_ENC_ERR_INVALID_DEVICE);
    }
    return pD3D11Device;
}

NvEncoderD3D12::NvEncoderD3D12(ID3D12Device* pD3D12Device, uint32_t nWidth, uint32_t nHeight,
                               NV_ENC_BUFFER_FORMAT eBufferFormat, uint32_t nExtraOutputDelay, bool bMotionEstimationOnly, bool bOutputInVideoMemory) :
    NvEncoderD3D11(CreateD3D11Device5(), nWidth, nHeight, eBufferFormat, nExtraOutputDelay, bMotionEstimationOnly, bOutputInVideoMemory),
    m_pD3D12Device(pD3D12Device), m_pD3D11Device5(static_cast<ID3D11Device5*>(m_pD3D11Device))
{
    if (!m_pD3D12Device)
    {
        NVENC_THROW_ERROR("Bad d3d12device ptr", NV_ENC_ERR_INVALID_PARAM);
    }
    m_pD3D12Device->AddRef();
}

NvEncoderD3D12::~NvEncoderD3D12()
{
    ReleaseD3D12Resources();
}

ID3D12Resource* NvEncoderD3D12::GetD3D12Resource(void* inputPtr)
{
    ID3D11Texture2D* pD3DTexture = static_cast<ID3D11Texture2D*>(inputPtr);
    return m_mapD3D12Resource.at(pD3DTexture);
}


void NvEncoderD3D12::AllocateInputBuffers(int32_t numInputBuffers)
{
    if (!IsHWEncoderInitialized())
    {
        NVENC_THROW_ERROR("Encoder intialization failed", NV_ENC_ERR_ENCODER_NOT_INITIALIZED);
    }

    // for MEOnly mode we need to allocate seperate set of buffers for reference frame
    int numCount = m_bMotionEstimationOnly ? 2 : 1;
    for (int count = 0; count < numCount; count++)
    {
        std::vector<void*> inputFrames;
        for (int i = 0; i < numInputBuffers; i++)
        {
            ID3D11Texture2D* sharedTex = nullptr;
            HANDLE handle = nullptr;

            D3D12_RESOURCE_DESC desc{};
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            desc.Alignment = 0;
            desc.Width = GetMaxEncodeWidth();
            desc.Height = GetMaxEncodeHeight();
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //We only support this format which has 4 bytes -> DX12_BYTES_PER_PIXEL
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;

            const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_SHARED;
            const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COPY_DEST;

            ID3D12Resource* nativeTex = nullptr;
            if (m_pD3D12Device->CreateCommittedResource(&unity::webrtc::D3D12_DEFAULT_HEAP_PROPS, flags, &desc, initialState,
                nullptr, IID_PPV_ARGS(&nativeTex)) != S_OK)
            {
                NVENC_THROW_ERROR("Failed to create commited resource", NV_ENC_ERR_OUT_OF_MEMORY);
            }

            if (m_pD3D12Device->CreateSharedHandle(nativeTex, nullptr, GENERIC_ALL, nullptr, &handle) != S_OK)
            {
                NVENC_THROW_ERROR("Failed to create shared handle", NV_ENC_ERR_OUT_OF_MEMORY);
            }

            //ID3D11Device::OpenSharedHandle() doesn't accept handles created by d3d12. OpenSharedHandle1() is needed.
            if (m_pD3D11Device5->OpenSharedResource1(handle, IID_PPV_ARGS(&sharedTex)) != S_OK)
            {
                NVENC_THROW_ERROR("Failed to open shared resource", NV_ENC_ERR_OUT_OF_MEMORY);
            }
            inputFrames.push_back(sharedTex);
            m_mapD3D12Resource.insert(std::make_pair(sharedTex, nativeTex));
        }
        RegisterInputResources(inputFrames, NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX,
            GetMaxEncodeWidth(), GetMaxEncodeHeight(), 0, GetPixelFormat(), count == 1 ? true : false);
    }
}

void NvEncoderD3D12::ReleaseInputBuffers()
{
    ReleaseD3D12Resources();
    NvEncoderD3D11::ReleaseInputBuffers();
}

void NvEncoderD3D12::ReleaseD3D12Resources()
{
    if (!m_hEncoder)
    {
        return;
    }

    if (m_pD3D12Device)
    {
        m_pD3D12Device->Release();
        m_pD3D12Device = nullptr;
    }
    m_mapD3D12Resource.clear();
}
