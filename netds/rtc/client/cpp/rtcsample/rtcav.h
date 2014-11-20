// rtcavsession.h
//

#ifndef _RTCAVSESSION_H_
#define _RTCAVSESSION_H_

// Class for audio/video session windows
class CRTCAVSession : public CRTCSession
{
public:
    CRTCAVSession();
    ~CRTCAVSession();

    // Registers the window class
    static HRESULT RegisterClass();

    // Window procedure
    static LRESULT CALLBACK WindowProc(
      HWND hWnd,      // handle to window
      UINT uMsg,      // message identifier
      WPARAM wParam,  // first message parameter
      LPARAM lParam   // second message parameter
    );   

protected:

    friend class CRTCWin;

    // Only one audio/video session can exist at a time. We will
    // treat this object as a singleton.
    static CRTCAVSession * m_Singleton;

    static HWND m_hRecvVideoParent;
    static HWND m_hPrevVideoParent;

    BOOL m_fShowRecv;
    BOOL m_fShowPrev;
    long m_lRecvWidth;
    long m_lRecvHeight;
    long m_lPrevWidth;
    long m_lPrevHeight;

    HWND m_hMicText;
    HWND m_hSpkText;
    HWND m_hMicLevel;
    HWND m_hSpkLevel;
    HWND m_hMicVol;
    HWND m_hSpkVol;
    HWND m_hMicMute;
    HWND m_hSpkMute;

    // Set the audio mute
    HRESULT DoMute(RTC_AUDIO_DEVICE enDevice);

    // Set the audio volume
    HRESULT DoVolume(RTC_AUDIO_DEVICE enDevice, long lVolume);
    
    // Update the audio controls display
    HRESULT ShowAudio();

    // Update the video controls display
    HRESULT ShowVideo(RTC_VIDEO_DEVICE enDevice, BOOL fShow);

    // Window message handlers
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Event handlers
    virtual HRESULT DeliverMedia(long lMediaType, RTC_MEDIA_EVENT_TYPE enType, RTC_MEDIA_EVENT_REASON enReason);
    virtual HRESULT DeliverIntensity(RTC_AUDIO_DEVICE enDevice, long lLevel);
    virtual HRESULT DeliverClient(RTC_CLIENT_EVENT_TYPE enEventType);
};

#endif //_RTCAVSESSION_H_