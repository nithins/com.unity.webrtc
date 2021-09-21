// todo::
// #if UNITY_WEBGL && !UNITY_EDITOR
#if UNITY_WEBGL

using System;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{

    //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    //internal delegate void DelegateDebugLog([MarshalAs(UnmanagedType.LPStr)] string level, [MarshalAs(UnmanagedType.LPStr)] string msg);

    //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    //internal delegate void DelegateNativeOnIceCandidate(IntPtr ptr, IntPtr iceCandidatePtr, [MarshalAs(UnmanagedType.LPStr)] string candidate, [MarshalAs(UnmanagedType.LPStr)] string sdpMid, int sdpMlineIndex);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateNativeOnTextMessage(IntPtr ptr, IntPtr msg);

    internal static partial class NativeMethods
    {
        public static IntPtr[] ptrToIntPtrArray(IntPtr ptr)
        {
            int len = Marshal.ReadInt32(ptr);
            int[] arr = new int[len];
            Marshal.Copy(IntPtr.Add(ptr, 4), arr, 0, len);
            IntPtr[] ret = new IntPtr[len];
            for (var i = 0; i < len; i++)
                ret[i] = new IntPtr(arr[i]);
            return ret;
        }

        //public static void RegisterDebugLog(Action<string> func) { }
        //[DllImport(WebRTC.Lib)]
        //public static extern void RegisterDebugLog(DebugLogLevel logLevel, DelegateDebugLog func);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr ContextCreateDataChannel(IntPtr ptr, IntPtr ptrPeer, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string label, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string optionsJson);

        [DllImport(WebRTC.Lib)]
        public static extern IntPtr ContextCreateVideoTrack(IntPtr self, IntPtr srcTexturePtr, IntPtr dstTexturePtr, int width, int height);

        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionCreateOffer(IntPtr ptr, string options);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionCreateAnswer(IntPtr ptr, string options);

        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType PeerConnectionSetLocalDescription(IntPtr context, IntPtr ptr, RTCSdpType type, string sdp);
        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType PeerConnectionSetRemoteDescription(IntPtr context, IntPtr ptr, RTCSdpType type, string sdp);

        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionTrackGetStats(IntPtr ptr, IntPtr track);

        [DllImport(WebRTC.Lib)]
        public static extern string ContextGetSenderCapabilities(IntPtr context, TrackKind kind);

        [DllImport(WebRTC.Lib)]
        public static extern string ContextGetReceiverCapabilities(IntPtr context, TrackKind kind);

        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetLocalDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetRemoteDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetCurrentLocalDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetCurrentRemoteDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetPendingLocalDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetPendingRemoteDescription(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionAddTrack(IntPtr pc, IntPtr track, IntPtr stream);
        [DllImport(WebRTC.Lib)]
        public static extern string IceCandidateGetCandidate(IntPtr candidate);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetReceivers(IntPtr context, IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetSenders(IntPtr context, IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionGetTransceivers(IntPtr context, IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern string TransceiverGetCurrentDirection(IntPtr transceiver);
        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType TransceiverSetCodecPreferences(IntPtr transceiver, string capabilities);

        [DllImport(WebRTC.Lib)]
        public static extern string SenderGetParameters(IntPtr sender);
        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType SenderSetParameters(IntPtr sender, string parameters);

        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnTextMessage(IntPtr ptr, DelegateNativeOnTextMessage callback);

        [DllImport(WebRTC.Lib)]
        public static extern void MediaStreamAddUserMedia(IntPtr streamPtr, string constraints);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamGetVideoTracks(IntPtr stream);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamGetAudioTracks(IntPtr stream);
        [DllImport(WebRTC.Lib)]
        public static extern string StatsReportGetStatsList(IntPtr report);

// todo::        
#if !UNITY_EDITOR
        public static IntPtr StatsGetJson(IntPtr stats) { return default; }
        public static IntPtr StatsGetId(IntPtr stats){ return default; }
        public static RTCStatsType StatsGetType(IntPtr stats){ return default; }
        public static long StatsGetTimestamp(IntPtr stats){ return default; }
        public static IntPtr StatsGetMembers(IntPtr stats, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetName(IntPtr member){ return default; }
        public static StatsMemberType StatsMemberGetType(IntPtr member){ return default; }
        public static bool StatsMemberIsDefined(IntPtr member){ return default; }
        public static bool StatsMemberGetBool(IntPtr member){ return default; }
        public static int StatsMemberGetInt(IntPtr member){ return default; }
        public static uint StatsMemberGetUnsignedInt(IntPtr member){ return default; }
        public static long StatsMemberGetLong(IntPtr member){ return default; }
        public static ulong StatsMemberGetUnsignedLong(IntPtr member){ return default; }
        public static double StatsMemberGetDouble(IntPtr member){ return default; }
        public static IntPtr StatsMemberGetString(IntPtr member){ return default; }
        public static IntPtr StatsMemberGetBoolArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetIntArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetUnsignedIntArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetLongArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetUnsignedLongArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetDoubleArray(IntPtr member, out ulong length){ length = default; return default; }
        public static IntPtr StatsMemberGetStringArray(IntPtr member, out ulong length){ length = default; return default; }
#endif
        [DllImport(WebRTC.Lib)]
        public static extern void UnityWebRTCInit(int logLebel);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreateAudioTrack();
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreateVideoTrack();
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreateMediaStream();
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreatePeerConnection();
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreatePeerConnectionWithConfig(string confJson);
        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType PeerConnectionSetDescription(IntPtr peerPtr, RTCSdpType type, string sdp, string side);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreateDataChannel();
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DeleteMediaStream(IntPtr streamPtr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DeleteReceiver(IntPtr receiverPtr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DeleteSender(IntPtr senderPtr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DeleteTransceiver(IntPtr transceiverPtr);

        [DllImport(WebRTC.Lib)]
        public static extern void RenderLocalVideotrack(IntPtr trackPtr, bool needFlip);
        [DllImport(WebRTC.Lib)]
        public static extern void UpdateRendererTexture(IntPtr trackPtr, IntPtr renderTexturePtr, bool needFlip);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CreateNativeTexture();
    }
}
#endif
