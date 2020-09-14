#pragma once
#include <mutex>
#include "DummyAudioDevice.h"
#include "PeerConnectionObject.h"
#include "GraphicsDevice/IGraphicsDevice.h"

namespace unity
{
namespace webrtc
{
    class Context;
    class IGraphicsDevice;
    class MediaStreamObserver;
    class UnityVideoTrackSource;
    class SetSessionDescriptionObserver;
    class ContextManager
    {
    public:
        static ContextManager* GetInstance() { return &s_instance; }
     
        Context* GetContext(int uid) const;
        Context* CreateContext(int uid, UnityEncoderType encoderType);
        void DestroyContext(int uid);
        void SetCurContext(Context*);
        bool Exists(Context* context);
        using ContextPtr = std::unique_ptr<Context>;
        Context* curContext = nullptr;
        std::mutex mutex;
    private:
        ~ContextManager();
        std::map<int, ContextPtr> m_contexts;
        static ContextManager s_instance;
    };

    enum class CodecInitializationResult
    {
        NotInitialized,
        Success,
        DriverNotInstalled,
        DriverVersionDoesNotSupportAPI,
        APINotFound,
        EncoderInitializationFailed
    };

    class Context
    {
    public:
        
        explicit Context(int uid = -1, UnityEncoderType encoderType = UnityEncoderHardware);
        ~Context();

        // Utility
        UnityEncoderType GetEncoderType() const;
        CodecInitializationResult GetInitializationResult(webrtc::MediaStreamTrackInterface* track);

        // MediaStream
        webrtc::MediaStreamInterface* CreateMediaStream(const std::string& streamId);
        void DeleteMediaStream(webrtc::MediaStreamInterface* stream);
        MediaStreamObserver* GetObserver(const webrtc::MediaStreamInterface* stream);

        // MediaStreamTrack
        webrtc::VideoTrackInterface* CreateVideoTrack(
            const std::string& label, NativeTexPtr frame, uint32_t destMemoryType);
        webrtc::AudioTrackInterface* CreateAudioTrack(const std::string& label);
        void DeleteMediaStreamTrack(webrtc::MediaStreamTrackInterface* track);
        void StopMediaStreamTrack(webrtc::MediaStreamTrackInterface* track);
        void ProcessAudioData(const float* data, int32 size);
        webrtc::VideoTrackInterface* FindVideoTrack(webrtc::MediaStreamTrackInterface* track);
        bool ExistsVideoSource(UnityVideoTrackSource* source);

        // PeerConnection
        PeerConnectionObject* CreatePeerConnection(const webrtc::PeerConnectionInterface::RTCConfiguration& config);
        void AddObserver(const webrtc::PeerConnectionInterface* connection, const rtc::scoped_refptr<SetSessionDescriptionObserver>& observer);
        void RemoveObserver(const webrtc::PeerConnectionInterface* connection);
        SetSessionDescriptionObserver* GetObserver(webrtc::PeerConnectionInterface* connection);
        void DeletePeerConnection(PeerConnectionObject* obj) { m_mapClients.erase(obj); }

        // StatsReport
        void AddStatsReport(const rtc::scoped_refptr<const webrtc::RTCStatsReport>& report);
        void DeleteStatsReport(const webrtc::RTCStatsReport* report);
    
        // DataChannel
        DataChannelObject* CreateDataChannel(PeerConnectionObject* obj, const char* label, const RTCDataChannelInit& options);
        void AddDataChannel(std::unique_ptr<DataChannelObject>& channel);
        void DeleteDataChannel(DataChannelObject* obj);

    private:
        int m_uid;
        UnityEncoderType m_encoderType;
        std::unique_ptr<rtc::Thread> m_workerThread;
        std::unique_ptr<rtc::Thread> m_signalingThread;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;
        rtc::scoped_refptr<DummyAudioDevice> m_audioDevice;
        rtc::scoped_refptr<webrtc::AudioTrackInterface> m_audioTrack;
        std::list<rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> m_mediaSteamTrackList;
        std::vector<rtc::scoped_refptr<const webrtc::RTCStatsReport>> m_listStatsReport;
        std::map<const PeerConnectionObject*, rtc::scoped_refptr<PeerConnectionObject>> m_mapClients;
        std::map<const webrtc::MediaStreamInterface*, std::unique_ptr<MediaStreamObserver>> m_mapMediaStreamObserver;
        std::map<const webrtc::PeerConnectionInterface*, rtc::scoped_refptr<SetSessionDescriptionObserver>> m_mapSetSessionDescriptionObserver;
        std::map<const DataChannelObject*, std::unique_ptr<DataChannelObject>> m_mapDataChannels;
        std::map<const std::string, webrtc::VideoTrackInterface*> m_mapVideoTrack;
        std::vector<const webrtc::VideoTrackSourceInterface*> m_listVideoSource;
    };

    extern bool Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config);
    extern webrtc::SdpType ConvertSdpType(RTCSdpType type);
    extern RTCSdpType ConvertSdpType(webrtc::SdpType type);

} // end namespace webrtc
} // end namespace unity
