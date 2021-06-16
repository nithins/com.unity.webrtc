using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class TestWebCam : MonoBehaviour
{
    [SerializeField] private Button callButton;
    [SerializeField] private Button hangUpButton;
    [SerializeField] private Dropdown webCamLListDropdown;
    [SerializeField] private RawImage sourceImage;

    private WebCamTexture webCamTexture;

    private void Awake()
    {
        callButton.onClick.AddListener(Call);
        hangUpButton.onClick.AddListener(HangUp);
        webCamLListDropdown.options = WebCamTexture.devices.Select(x => new Dropdown.OptionData(x.name)).ToList();
    }

    void Call()
    {
        StartCoroutine(CaptureVideoStart());
    }

    IEnumerator CaptureVideoStart()
    {
        if (WebCamTexture.devices.Length == 0)
        {
            Debug.LogFormat("WebCam device not found");
            yield break;
        }

        yield return Application.RequestUserAuthorization(UserAuthorization.WebCam);
        if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
        {
            Debug.LogFormat("authorization for using the device is denied");
            yield break;
        }

        WebCamDevice userCameraDevice = WebCamTexture.devices[webCamLListDropdown.value];
        webCamTexture = new WebCamTexture(userCameraDevice.name, 1280, 720, 30);
        webCamTexture.Play();
        yield return new WaitUntil(() => webCamTexture.didUpdateThisFrame);
        sourceImage.texture = webCamTexture;
    }

    void HangUp()
    {
        if (webCamTexture != null)
        {
            webCamTexture.Stop();
            webCamTexture = null;
        }

        sourceImage.texture = null;
    }
}
