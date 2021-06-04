using System;
using System.Collections;
using System.Linq;
using UnityEngine;
using Unity.WebRTC;
using Unity.WebRTC.Samples;
using UnityEngine.UI;

class VideoSendReceiveSample : MonoBehaviour
{
#pragma warning disable 0649
    [SerializeField] private Button callButton;
    [SerializeField] private Button addTrackButton;
    [SerializeField] private Button removeTrackButton;
    [SerializeField] private Button hangUpButton;
    [SerializeField] private Camera cam1;
    [SerializeField] private Camera cam2;
    [SerializeField] private RawImage sourceImage1;
    [SerializeField] private RawImage sourceImage2;
    [SerializeField] private RawImage receiveImage1;
    [SerializeField] private RawImage receiveImage2;
    [SerializeField] private Vector2Int streamingSize;
    [SerializeField] private Transform rotateObject;
#pragma warning restore 0649

    private RTCPeerConnection _pc1, _pc2;
    private VideoStreamTrack videoStreamTrack1;
    private VideoStreamTrack videoStreamTrack2;
    private RTCRtpSender rtpSender1;
    private RTCRtpSender rtpSender2;
    private bool videoUpdateStarted;

    private void Awake()
    {
        //WebRTC.Initialize(WebRTCSettings.EncoderType, WebRTCSettings.LimitTextureSize);
        WebRTC.Initialize();
    }

    private void OnDestroy()
    {
        WebRTC.Dispose();
    }

    private void Start()
    {
        callButton.onClick.AddListener(Call);
        addTrackButton.onClick.AddListener(AddTrack);
        removeTrackButton.onClick.AddListener(RemoveTrack);
        hangUpButton.onClick.AddListener(HangUp);

        callButton.interactable = true;
        addTrackButton.interactable = false;
        removeTrackButton.interactable = false;
        hangUpButton.interactable = false;
    }

    private void Update()
    {
        if (rotateObject != null)
        {
            rotateObject.Rotate(1, 2, 3);
        }
    }

    private void OnIceConnectionChange(RTCPeerConnection pc, RTCIceConnectionState state)
    {
        switch (state)
        {
            case RTCIceConnectionState.New:
                Debug.Log($"{GetName(pc)} IceConnectionState: New");
                break;
            case RTCIceConnectionState.Checking:
                Debug.Log($"{GetName(pc)} IceConnectionState: Checking");
                break;
            case RTCIceConnectionState.Closed:
                Debug.Log($"{GetName(pc)} IceConnectionState: Closed");
                break;
            case RTCIceConnectionState.Completed:
                Debug.Log($"{GetName(pc)} IceConnectionState: Completed");
                break;
            case RTCIceConnectionState.Connected:
                Debug.Log($"{GetName(pc)} IceConnectionState: Connected");
                break;
            case RTCIceConnectionState.Disconnected:
                Debug.Log($"{GetName(pc)} IceConnectionState: Disconnected");
                break;
            case RTCIceConnectionState.Failed:
                Debug.Log($"{GetName(pc)} IceConnectionState: Failed");
                break;
            case RTCIceConnectionState.Max:
                Debug.Log($"{GetName(pc)} IceConnectionState: Max");
                break;
            default:
                throw new ArgumentOutOfRangeException(nameof(state), state, null);
        }
    }

    IEnumerator PeerNegotiationNeeded(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} createOffer start");
        var op = pc.CreateOffer();
        yield return op;

        if (!op.IsError)
        {
            if (pc.SignalingState != RTCSignalingState.Stable)
            {
                Debug.LogError($"{GetName(pc)} signaling state is not stable.");
                yield break;
            }

            yield return StartCoroutine(OnCreateOfferSuccess(pc, op.Desc));
        }
        else
        {
            OnCreateSessionDescriptionError(op.Error);
        }
    }

    private void Call()
    {
        callButton.interactable = false;
        hangUpButton.interactable = true;

        Debug.Log("GetSelectedSdpSemantics");
        var configuration = WebRTCSettings.GetSelectedSdpSemantics();
        _pc1 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created local peer connection object pc1");

        _pc1.OnIceConnectionChange = state => { OnIceConnectionChange(_pc1, state); };
        _pc1.OnIceCandidate = candidate => { OnIceCandidate(_pc1, candidate); };
        _pc1.OnTrack = e => { OnTrack(e, receiveImage1); };
        _pc1.OnNegotiationNeeded = () => { StartCoroutine(PeerNegotiationNeeded(_pc1)); };

        _pc2 = new RTCPeerConnection(ref configuration);
        Debug.Log("Created remote peer connection object pc2");
        _pc2.OnIceConnectionChange = state => { OnIceConnectionChange(_pc2, state); };
        _pc2.OnIceCandidate = candidate => { OnIceCandidate(_pc2, candidate); };
        _pc2.OnTrack = e => { OnTrack(e, receiveImage2); };


        Debug.Log("capture cameras");
        videoStreamTrack1 = cam1.CaptureStreamTrack(streamingSize.x, streamingSize.y, 0);
        sourceImage1.texture = cam1.targetTexture;

        videoStreamTrack2 = cam2.CaptureStreamTrack(streamingSize.x, streamingSize.y, 0);
        sourceImage2.texture = cam2.targetTexture;

        if (!videoUpdateStarted)
        {
            StartCoroutine(WebRTC.Update());
            videoUpdateStarted = true;
        }

        addTrackButton.interactable = true;
        removeTrackButton.interactable = false;
    }

    private void AddTrack()
    {
        Debug.Log("add video tracks");
        addTrackButton.interactable = false;
        removeTrackButton.interactable = true;

        //negotiation
        rtpSender1 = _pc1.AddTrack(videoStreamTrack1);
        rtpSender2 = _pc2.AddTrack(videoStreamTrack2);
    }

    private void RemoveTrack()
    {
        Debug.Log("remove video tracks");
        addTrackButton.interactable = true;
        removeTrackButton.interactable = false;
        _pc1.RemoveTrack(rtpSender1);
        _pc2.RemoveTrack(rtpSender2);
        rtpSender1.Dispose();
        rtpSender1 = null;
        rtpSender2.Dispose();
        rtpSender2 = null;
    }

    private void HangUp()
    {
        videoStreamTrack1?.Dispose();
        videoStreamTrack1 = null;
        videoStreamTrack2?.Dispose();
        videoStreamTrack2 = null;

        rtpSender1?.Dispose();
        rtpSender1 = null;
        rtpSender2?.Dispose();
        rtpSender2 = null;

        _pc1.Close();
        _pc2.Close();
        Debug.Log("Close local/remote peer connection");
        _pc1.Dispose();
        _pc2.Dispose();
        _pc1 = null;
        _pc2 = null;
        sourceImage1.texture = null;
        sourceImage2.texture = null;
        receiveImage1.texture = null;
        receiveImage2.texture = null;
        callButton.interactable = true;
        addTrackButton.interactable = false;
        removeTrackButton.interactable = false;
        hangUpButton.interactable = false;
    }

    private void OnIceCandidate(RTCPeerConnection pc, RTCIceCandidate candidate)
    {
        GetOtherPc(pc).AddIceCandidate(candidate);
        Debug.Log($"{GetName(pc)} ICE candidate:\n {candidate.Candidate}");
    }

    private void OnTrack(RTCTrackEvent trackEvent, RawImage target)
    {
        if (trackEvent.Track is VideoStreamTrack video)
        {
            target.texture = video.InitializeReceiver(1280, 720);
            trackEvent.Streams.First().OnRemoveTrack = e =>
            {
                if (e.Track.Id == video.Id)
                {
                    target.texture = null;
                }
            };
        }
    }

    private string GetName(RTCPeerConnection pc)
    {
        return (pc == _pc1) ? "pc1" : "pc2";
    }

    private RTCPeerConnection GetOtherPc(RTCPeerConnection pc)
    {
        return (pc == _pc1) ? _pc2 : _pc1;
    }

    private IEnumerator OnCreateOfferSuccess(RTCPeerConnection pc, RTCSessionDescription desc)
    {
        Debug.Log($"Offer from {GetName(pc)}\n{desc.sdp}");
        Debug.Log($"{GetName(pc)} setLocalDescription start");
        var op = pc.SetLocalDescription(ref desc);
        yield return op;

        if (!op.IsError)
        {
            OnSetLocalSuccess(pc);
        }
        else
        {
            var error = op.Error;
            OnSetSessionDescriptionError(ref error);
        }

        var otherPc = GetOtherPc(pc);
        Debug.Log($"{GetName(otherPc)} setRemoteDescription start");
        var op2 = otherPc.SetRemoteDescription(ref desc);
        yield return op2;
        if (!op2.IsError)
        {
            OnSetRemoteSuccess(otherPc);
        }
        else
        {
            var error = op2.Error;
            OnSetSessionDescriptionError(ref error);
        }

        Debug.Log($"{GetName(otherPc)} createAnswer start");
        // Since the 'remote' side has no media stream we need
        // to pass in the right constraints in order for it to
        // accept the incoming offer of audio and video.

        var op3 = otherPc.CreateAnswer();
        yield return op3;
        if (!op3.IsError)
        {
            yield return OnCreateAnswerSuccess(otherPc, op3.Desc);
        }
        else
        {
            OnCreateSessionDescriptionError(op3.Error);
        }
    }

    private void OnSetLocalSuccess(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} SetLocalDescription complete");
    }

    static void OnSetSessionDescriptionError(ref RTCError error)
    {
        Debug.LogError($"Error Detail Type: {error.message}");
    }

    private void OnSetRemoteSuccess(RTCPeerConnection pc)
    {
        Debug.Log($"{GetName(pc)} SetRemoteDescription complete");
    }

    IEnumerator OnCreateAnswerSuccess(RTCPeerConnection pc, RTCSessionDescription desc)
    {
        Debug.Log($"Answer from {GetName(pc)}:\n{desc.sdp}");
        Debug.Log($"{GetName(pc)} setLocalDescription start");
        var op = pc.SetLocalDescription(ref desc);
        yield return op;

        if (!op.IsError)
        {
            OnSetLocalSuccess(pc);
        }
        else
        {
            var error = op.Error;
            OnSetSessionDescriptionError(ref error);
        }

        var otherPc = GetOtherPc(pc);
        Debug.Log($"{GetName(otherPc)} setRemoteDescription start");

        var op2 = otherPc.SetRemoteDescription(ref desc);
        yield return op2;
        if (!op2.IsError)
        {
            OnSetRemoteSuccess(otherPc);
        }
        else
        {
            var error = op2.Error;
            OnSetSessionDescriptionError(ref error);
        }
    }

    private static void OnCreateSessionDescriptionError(RTCError error)
    {
        Debug.LogError($"Error Detail Type: {error.message}");
    }
}
