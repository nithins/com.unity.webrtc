#include "pch.h"
#include "NvEncoderProxy.h"
#include "Codec/NvCodec/Util.h"
#include "GraphicsDevice/IGraphicsDevice.h"

#if defined(SUPPORT_OPENGL_CORE)
#include "NvEncoderGL.h"
#endif

#if defined(SUPPORT_D3D11)
#include "NvEncoderD3D11.h"
#endif

#if defined(SUPPORT_D3D12)
#include "NvEncoderD3D12.h"
#endif
#include "NvEncoderCuda.h"
#include "NvVideoCapturer.h"

#if defined(SUPPORT_METAL)
#include "VideoToolbox/VTEncoderMetal.h"
#endif

namespace unity
{
namespace webrtc
{

    NvEncoderProxy::NvEncoderProxy(const int width, const int height, IGraphicsDevice* device) :
        m_width(width), m_height(height), m_device(device)
    {
        ID3D11Device* pDevice = static_cast<ID3D11Device*>(device->GetEncodeDevicePtrV());
        m_encoder = std::make_unique<NvEncoderD3D11>(pDevice, width, height, NV_ENC_BUFFER_FORMAT_ARGB);
        m_bitrateAdjuster = std::make_unique<::webrtc::BitrateAdjuster>(0.5f, 0.95f);
    }

    NvEncoderProxy::~NvEncoderProxy()
    {
        m_encoder->DestroyEncoder();
    }

    void NvEncoderProxy::InitV()
    {
        if (m_initializationResult == CodecInitializationResult::NotInitialized)
        {
            if(!LoadModule())
                m_initializationResult = CodecInitializationResult::DriverNotInstalled;
            if (!CheckDriverVersion())
                m_initializationResult = CodecInitializationResult::DriverVersionDoesNotSupportAPI;
        }
        else if (m_initializationResult != CodecInitializationResult::Success)
        {
            return;
        }

        nvEncInitializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
        nvEncConfig = { NV_ENC_CONFIG_VER };
        nvEncInitializeParams.encodeConfig = &nvEncConfig;
        m_encoder->CreateDefaultEncoderParams(&nvEncInitializeParams,
            NV_ENC_CODEC_H264_GUID,
            NV_ENC_PRESET_P1_GUID,
            NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY);

#pragma region get preset ocnfig and set it
        nvEncConfig.gopLength = NVENC_INFINITE_GOPLENGTH;
        nvEncConfig.frameIntervalP = 1;
        nvEncConfig.encodeCodecConfig.h264Config.idrPeriod = NVENC_INFINITE_GOPLENGTH;
        // nvEncConfig.encodeCodecConfig.hevcConfig.idrPeriod = NVENC_INFINITE_GOPLENGTH;

        nvEncConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR;
        nvEncConfig.rcParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;
        nvEncConfig.rcParams.averageBitRate = m_bitrateAdjuster->GetAdjustedBitrateBps();
        nvEncConfig.encodeCodecConfig.h264Config.idrPeriod = nvEncConfig.gopLength;

        nvEncConfig.rcParams.averageBitRate =
            (static_cast<unsigned int>(5.0f *
                nvEncInitializeParams.encodeWidth *
                nvEncInitializeParams.encodeHeight) / (1280 * 720)) * 100000;
        nvEncConfig.rcParams.vbvBufferSize =
            (nvEncConfig.rcParams.averageBitRate *
                nvEncInitializeParams.frameRateDen / nvEncInitializeParams.frameRateNum) * 5;
        nvEncConfig.rcParams.maxBitRate = nvEncConfig.rcParams.averageBitRate;
        nvEncConfig.rcParams.vbvInitialDelay = nvEncConfig.rcParams.vbvBufferSize;
#pragma endregion

        try
        {
            m_encoder->CreateEncoder(&nvEncInitializeParams);
        }
        catch(NVENCException& ex)
        {
            m_initializationResult = CodecInitializationResult::EncoderInitializationFailed;
        }
    }

    bool NvEncoderProxy::IsSupported() const
    {
        return CheckDriverVersion();
    }


    void NvEncoderProxy::UpdateSettings()
    {
        bool settingChanged = false;
        const uint32_t bitRate = m_bitrateAdjuster->GetAdjustedBitrateBps();
        if (nvEncConfig.rcParams.averageBitRate != bitRate)
        {
            nvEncConfig.rcParams.averageBitRate = bitRate;
            settingChanged = true;
        }
        if (nvEncInitializeParams.frameRateNum != m_frameRate)
        {
            nvEncInitializeParams.frameRateNum = m_frameRate;
            settingChanged = true;
        }

        if (settingChanged)
        {
            NV_ENC_RECONFIGURE_PARAMS nvEncReconfigureParams;
            std::memcpy(&nvEncReconfigureParams.reInitEncodeParams, &nvEncInitializeParams, sizeof(nvEncInitializeParams));
            nvEncReconfigureParams.version = NV_ENC_RECONFIGURE_PARAMS_VER;
            m_encoder->Reconfigure(&nvEncReconfigureParams);
        }
    }

    void NvEncoderProxy::SetRates(const webrtc::VideoEncoder::RateControlParameters& parameters)
    {
        const uint32_t bitrate = parameters.bitrate.get_sum_bps();
        m_bitrateAdjuster->SetTargetBitrateBps(bitrate);
        m_frameRate = static_cast<uint32_t>(parameters.framerate_fps);
    }

    //entry for encoding a frame
    bool NvEncoderProxy::EncodeFrame(void* frame)
    {
        UpdateSettings();

        const NvEncInputFrame* inputFrame = m_encoder->GetNextInputFrame();
        ITexture2D* inputTex = m_device->CreateTextureV(inputFrame->inputPtr);
        m_device->CopyResourceFromNativeV(inputTex, frame);

#pragma region start encoding
        NV_ENC_PIC_PARAMS picParams = {};
        if (isIdrFrame)
        {
            picParams.encodePicFlags |= NV_ENC_PIC_FLAG_FORCEIDR;
        }
        isIdrFrame = false;

        std::vector<std::vector<uint8_t>> vPacket;
        m_encoder->EncodeFrame(vPacket, &picParams);
#pragma endregion

        for (std::vector<uint8_t>& packet : vPacket)
        {
            ProcessEncodedFrame(packet);
            m_bitrateAdjuster->Update(packet.size());
            frameCount++;
        }
        return true;
    }

    //get encoded frame
    void NvEncoderProxy::ProcessEncodedFrame(std::vector<uint8_t>& packet)
    {
        const rtc::scoped_refptr<FrameBuffer> buffer = new rtc::RefCountedObject<FrameBuffer>(m_width, m_height, packet, m_encoderId);
        const int64 timestamp = rtc::TimeMillis();
        webrtc::VideoFrame videoFrame{ buffer, webrtc::VideoRotation::kVideoRotation_0, timestamp };
        videoFrame.set_ntp_time_ms(timestamp);
        CaptureFrame(videoFrame);
    }

} // end namespace webrtc
} // end namespace unity
