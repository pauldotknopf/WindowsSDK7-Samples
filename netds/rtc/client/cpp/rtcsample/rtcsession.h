// rtcsession.h
//

#ifndef _RTCSESSION_H_
#define _RTCSESSION_H_

// Base class for session windows
class CRTCSession
{
public:
    CRTCSession();
    ~CRTCSession();

protected:

    friend class CRTCWin;

    HWND m_hWnd;
    HWND m_hStatusBar;

    IRTCSession * m_pSession;
    CRTCWin * m_pWin;
    RTC_SESSION_STATE m_enState;

    // Helpers
    void ShowMessageBox(PWSTR szText);
    void SetStatusText(PWSTR szText);

    // Window message handlers
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Event handlers
    virtual HRESULT DeliverSessionState(RTC_SESSION_STATE enState);
    virtual HRESULT DeliverParticipantState(IRTCParticipant * pParticipant, RTC_PARTICIPANT_STATE enState);
    virtual HRESULT DeliverMessage(IRTCParticipant * pParticipant, BSTR bstrContentType, BSTR bstrMessage);
    virtual HRESULT DeliverUserStatus(IRTCParticipant * pParticipant, RTC_MESSAGING_USER_STATUS enStatus);
    virtual HRESULT DeliverMedia(long lMediaType, RTC_MEDIA_EVENT_TYPE enType, RTC_MEDIA_EVENT_REASON enReason);
    virtual HRESULT DeliverIntensity(RTC_AUDIO_DEVICE enDevice, long lLevel);
    virtual HRESULT DeliverClient(RTC_CLIENT_EVENT_TYPE enEventType);
};

#endif //_RTCSESSION_H_