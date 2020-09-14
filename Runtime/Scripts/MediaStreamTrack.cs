using System;
using System.Collections.Generic;
using UnityEngine;

namespace Unity.WebRTC
{
    [Flags]
    public enum VideoSourceMemoryType
    {
        GpuMemory = 1,
        CpuMemory = 1 << 1
    }

    public class MediaStreamTrack : IDisposable
    {
        internal IntPtr self;
        protected bool disposed;
        private bool enabled;
        private TrackState readyState;

        /// <summary>
        ///
        /// </summary>
        public bool Enabled
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetEnabled(self);
            }
            set
            {
                NativeMethods.MediaStreamTrackSetEnabled(self, value);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public TrackState ReadyState
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetReadyState(self);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public TrackKind Kind { get; }

        /// <summary>
        ///
        /// </summary>
        public string Id { get; }

        internal MediaStreamTrack(IntPtr ptr)
        {
            self = ptr;
            WebRTC.Table.Add(self, this);
            Kind = NativeMethods.MediaStreamTrackGetKind(self);
            Id = NativeMethods.MediaStreamTrackGetID(self).AsAnsiStringWithFreeMem();
        }

        ~MediaStreamTrack()
        {
            this.Dispose();
            WebRTC.Table.Remove(self);
        }

        public virtual void Dispose()
        {
            if (this.disposed)
            {
                return;
            }
            if (self != IntPtr.Zero && !WebRTC.Context.IsNull)
            {
                WebRTC.Context.DeleteMediaStreamTrack(self);
                self = IntPtr.Zero;
            }
            this.disposed = true;
            GC.SuppressFinalize(this);
        }

        //Disassociate track from its source(video or audio), not for destroying the track
        public void Stop()
        {
            WebRTC.Context.StopMediaStreamTrack(self);
        }
    }

    public class VideoStreamTrack : MediaStreamTrack
    {
        internal static List<VideoStreamTrack> tracks = new List<VideoStreamTrack>();

        private readonly IntPtr m_videoSource;

        readonly bool m_needFlip = false;
        readonly UnityEngine.Texture m_sourceTexture;
        readonly UnityEngine.RenderTexture m_destTexture;

        private static UnityEngine.RenderTexture CreateRenderTexture(int width, int height, UnityEngine.RenderTextureFormat format)
        {
            var tex = new UnityEngine.RenderTexture(width, height, 0, format);
            tex.Create();
            return tex;
        }

        internal VideoStreamTrack(string label, UnityEngine.Texture source, UnityEngine.RenderTexture dest, bool useGpu, bool useCpu)
            : this(label, dest.GetNativeTexturePtr(), useGpu, useCpu)
        {
            m_needFlip = true;
            m_sourceTexture = source;
            m_destTexture = dest;
        }

        /// <summary>
        /// note:
        /// The videotrack cannot be used if the encoder has not been initialized.
        /// Do not use it until the initialization is complete.
        /// </summary>
        public bool IsInitialized
        {
            get
            {
                return WebRTC.Context.GetInitializationResult(self) == CodecInitializationResult.Success;
            }
        }

        internal void Update()
        {
            // [Note-kazuki: 2020-03-09] Flip vertically RenderTexture
            // note: streamed video is flipped vertical if no action was taken:
            //  - duplicate RenderTexture from its source texture
            //  - call Graphics.Blit command with flip material every frame
            //  - it might be better to implement this if possible
            if (m_needFlip)
            {
                UnityEngine.Graphics.Blit(m_sourceTexture, m_destTexture, WebRTC.flipMat);
            }
            WebRTC.Context.Encode(m_videoSource);
        }

        /// <summary>
        /// Creates a new VideoStream object.
        /// The track is created with a `source`.
        /// </summary>
        /// <param name="label"></param>
        /// <param name="source"></param>
        /// <param name="useGpu"></param>
        /// <param name="useCpu"></param>
        public VideoStreamTrack(string label, RenderTexture source, bool useGpu = true, bool useCpu = true)
            : this(label, source, CreateRenderTexture(source.width, source.height, source.format), useGpu, useCpu)
        {
        }

        public VideoStreamTrack(string label, Texture source, bool useGpu, bool useCpu)
            : this(label, source,
                CreateRenderTexture(source.width, source.height, WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType)),
                useGpu , useCpu)
        {
        }


        /// <summary>
        /// Creates a new VideoStream object.
        /// The track is created with a source texture `ptr`.
        /// It is noted that streamed video might be flipped when not action was taken. Almost case it has no problem to use other constructor instead.
        ///
        /// See Also: Texture.GetNativeTexturePtr
        /// </summary>
        /// <param name="label"></param>
        /// <param name="ptr"></param>
        /// <param name="useGpu"></param>
        /// <param name="useCpu"></param>
        public VideoStreamTrack(string label, IntPtr ptr, bool useGpu, bool useCpu)
            : base(WebRTC.Context.CreateVideoTrack(label, ptr, useGpu, useCpu))
        {
            m_videoSource = GetVideoTrackSource();
            WebRTC.Context.InitializeEncoder(m_videoSource);
            tracks.Add(this);
        }

        public override void Dispose()
        {
            if (this.disposed)
            {
                return;
            }
            if (self != IntPtr.Zero && !WebRTC.Context.IsNull)
            {
                WebRTC.Context.FinalizeEncoder(m_videoSource);
                tracks.Remove(this);
                WebRTC.Context.DeleteMediaStreamTrack(self);
                UnityEngine.Object.DestroyImmediate(m_destTexture);
                self = IntPtr.Zero;
            }
            this.disposed = true;
            GC.SuppressFinalize(this);
        }

        internal IntPtr GetVideoTrackSource()
        {
            return NativeMethods.ContextGetVideoSource(WebRTC.Context.self, self);
        }
    }

    public class AudioStreamTrack : MediaStreamTrack
    {
        public AudioStreamTrack(string label) : base(WebRTC.Context.CreateAudioTrack(label))
        {
        }
    }

    public enum TrackKind
    {
        Audio,
        Video
    }
    public enum TrackState
    {
        Live,
        Ended
    }

    public class RTCTrackEvent
    {
        public MediaStreamTrack Track { get; }

        public RTCRtpTransceiver Transceiver { get; }

        public RTCRtpReceiver Receiver
        {
            get
            {
                return Transceiver.Receiver;
            }
        }

        internal RTCTrackEvent(IntPtr ptrTransceiver, RTCPeerConnection peer)
        {
            IntPtr ptrTrack = NativeMethods.TransceiverGetTrack(ptrTransceiver);
            Track = WebRTC.FindOrCreate(ptrTrack, ptr => new MediaStreamTrack(ptr));
            Transceiver = new RTCRtpTransceiver(ptrTransceiver, peer);
        }
    }

    public class MediaStreamTrackEvent
    {
        public MediaStreamTrack Track { get; }

        internal MediaStreamTrackEvent(IntPtr ptrTrack)
        {
            Track = WebRTC.FindOrCreate(ptrTrack, ptr => new MediaStreamTrack(ptr));
        }
    }
}

