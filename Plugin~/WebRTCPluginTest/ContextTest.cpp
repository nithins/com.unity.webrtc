#include "pch.h"

#include <cuda.h>

#include "NvCodecUtils.h"
#include "rtc_base/bind.h"
#include "GraphicsDeviceTestBase.h"
#include "GraphicsDevice/ITexture2D.h"
#include "Context.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
namespace webrtc
{

using namespace ::webrtc;

class ContextTest : public GraphicsDeviceTestBase
{
protected:

    ContextTest()
    {
//        EXPECT_TRUE(ck(cuInit(0)));
//        EXPECT_TRUE(ck(cuDeviceGet(&device_, 0)));

        context_ = std::make_unique<Context>();
    }

    virtual ~ContextTest()
    {
    }

    const int width = 256;
    const int height = 256;
    std::unique_ptr<Context> context_;
    CUdevice device_;
};

TEST_P(ContextTest, CreateAndDeleteMediaStream) {
    MediaStreamInterface* stream = context_->CreateMediaStream("test");
    context_->DeleteMediaStream(stream);
}


TEST_P(ContextTest, CreateAndDeleteVideoTrack) {
    const std::unique_ptr<ITexture2D> tex(m_device->CreateDefaultTextureV(width, height));
    EXPECT_NE(nullptr, tex.get());
    void* frame = tex->GetNativeTexturePtrV();
    EXPECT_NE(nullptr, frame);
    VideoTrackInterface* track = context_->CreateVideoTrack("video", frame, GPU_MEMORY | CPU_MEMORY);
    EXPECT_NE(nullptr, track);
}

TEST_P(ContextTest, CreateAndDeleteAudioTrack) {
    const auto track = context_->CreateAudioTrack("audio");
    context_->DeleteMediaStreamTrack(track);
}

TEST_P(ContextTest, AddAndRemoveAudioTrackToMediaStream) {
    const auto stream = context_->CreateMediaStream("audiostream");
    const auto track = context_->CreateAudioTrack("audio");
    const auto audiotrack = reinterpret_cast<webrtc::AudioTrackInterface*>(track);
    stream->AddTrack(audiotrack);
    stream->RemoveTrack(audiotrack);
    context_->DeleteMediaStream(stream);
    context_->DeleteMediaStreamTrack(track);
}

TEST_P(ContextTest, AddAndRemoveVideoTrackToMediaStream) {
    const std::unique_ptr<ITexture2D> tex(m_device->CreateDefaultTextureV(width, height));
    void* frame = tex->GetNativeTexturePtrV();
    const auto stream = context_->CreateMediaStream("videostream");
    const auto track = context_->CreateVideoTrack("video", frame, GPU_MEMORY | CPU_MEMORY);
    const auto videoTrack = reinterpret_cast<webrtc::VideoTrackInterface*>(track);
    stream->AddTrack(videoTrack);
    stream->RemoveTrack(videoTrack);
    context_->DeleteMediaStream(stream);
    context_->DeleteMediaStreamTrack(track);
}

TEST_P(ContextTest, CreateAndDeletePeerConnection) {
    const webrtc::PeerConnectionInterface::RTCConfiguration config;
    const auto connection = context_->CreatePeerConnection(config);
    context_->DeletePeerConnection(connection);
}

TEST_P(ContextTest, CreateAndDeleteDataChannel) {
    const webrtc::PeerConnectionInterface::RTCConfiguration config;
    const auto connection = context_->CreatePeerConnection(config);
    RTCDataChannelInit init;
    init.protocol = "";
    const auto channel = context_->CreateDataChannel(connection, "test", init);
    context_->DeleteDataChannel(channel);
    context_->DeletePeerConnection(connection);
}

TEST_P(ContextTest, Thread) {
    // Create and start the thread.
    auto thread = rtc::Thread::CreateWithSocketServer();
    thread->Start();
    // Try calling functors.
    //int a = thread->Invoke<int>(
    //    RTC_FROM_HERE,
    //    rtc::Bind(&ContextTest::Hello, static_cast<ContextTest*>(this)));
    //EXPECT_EQ(a, 0);

    //bool ret = thread->Invoke<bool>(
    //    RTC_FROM_HERE,
    //    rtc::Bind(&Context::InitializeEncoder, context_, encoder_.get(), track)
    //    )
}

INSTANTIATE_TEST_CASE_P(GraphicsDeviceParameters, ContextTest, testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity
