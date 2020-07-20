#pragma once
#include "Codec/IEncoder.h"
#include "NvEncoder.h"

namespace unity
{
namespace webrtc
{
    class ITexture2D;
    class IGraphicsDevice;
    class NvEncoderProxy : public IEncoder
    {
    public:
        NvEncoderProxy(int width, int height, IGraphicsDevice* device);
        virtual ~NvEncoderProxy();
        void InitV() override;
        void SetRates(const ::webrtc::VideoEncoder::RateControlParameters& parameters) override;
        void UpdateSettings() override;
        bool EncodeFrame(void* frame) override;
        bool IsSupported() const override;
        void SetIdrFrame()  override { isIdrFrame = true; }
        uint64 GetCurrentFrameCount() const override { return frameCount; }
    private:
        void ProcessEncodedFrame(std::vector<uint8_t>& packet);
        void InitEncoderResources(uint32_t encoderBufferCount);
    private:
        NV_ENC_INITIALIZE_PARAMS nvEncInitializeParams = {};
        NV_ENC_CONFIG nvEncConfig = {};
        uint64 frameCount = 0;
        int m_width;
        int m_height;
        uint32_t m_frameRate = 30;
        IGraphicsDevice* m_device;
        bool isIdrFrame = false;
        std::unique_ptr<NvEncoder> m_encoder;
        std::unique_ptr<webrtc::BitrateAdjuster> m_bitrateAdjuster;
    };
} // end namespace webrtc
} // end namespace unity
