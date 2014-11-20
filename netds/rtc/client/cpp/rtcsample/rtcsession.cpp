// rtcsession.cpp
//

#include "stdafx.h"

/////////////////////////////////////////////
//
// CRTCSession::CRTCSession
// 

CRTCSession::CRTCSession()
{
    m_hWnd = NULL;
    m_pSession = NULL;
    m_pWin = NULL;
    m_enState = RTCSS_IDLE;
}

/////////////////////////////////////////////
//
// CRTCSession::~CRTCSession
// 

CRTCSession::~CRTCSession()
{
}

/////////////////////////////////////////////
//
// CRTCSession::ShowMessageBox
//

void CRTCSession::ShowMessageBox(PWSTR szText)
{
    MessageBoxW(m_hWnd, szText, APP_TITLE, MB_OK);
}

/////////////////////////////////////////////
//
// CRTCSession::SetStatusText
//

void CRTCSession::SetStatusText(PWSTR szText)
{
    SetWindowTextW(m_hStatusBar, szText);
    InvalidateRect(m_hStatusBar, NULL, FALSE);
}

/////////////////////////////////////////////
//
// CRTCSession::OnCreate
// 

LRESULT CRTCSession::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Create the status bar
    m_hStatusBar = CreateStatusWindow(
        WS_CHILD | WS_VISIBLE,
        NULL,
        m_hWnd,
        IDC_STATUSBAR
        );

    if ( !m_hStatusBar )
    {
        // CreateStatusWindow failed
        return -1;
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCSession::OnDestroy
// 

LRESULT CRTCSession::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
    if (m_pSession)
    {
        // Terminate and release the session
        m_pSession->Terminate(RTCTR_NORMAL);

        SAFE_RELEASE(m_pSession);
    }

    if (m_pWin)
    {
        // Remove the session from the session list
        m_pWin->RemoveSession(this);
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCSession::OnClose
// 

LRESULT CRTCSession::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DestroyWindow(m_hWnd);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCSession::OnSize
// 

LRESULT CRTCSession::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Resize the status bar
    SendMessage(m_hStatusBar, uMsg, wParam, lParam);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverSessionState
//

HRESULT CRTCSession::DeliverSessionState(RTC_SESSION_STATE enState)
{
    m_enState = enState;

    // Display the session state in the status bar
    switch(m_enState)
    {
    case RTCSS_IDLE:
        SetStatusText(L"Idle");
        break;

    case RTCSS_INCOMING:
        SetStatusText(L"Incoming");
        break;

    case RTCSS_ANSWERING:
        SetStatusText(L"Answering");
        break;

    case RTCSS_INPROGRESS:
        SetStatusText(L"Inprogress");
        break;

    case RTCSS_CONNECTED:
        SetStatusText(L"Connected");
        break;

    case RTCSS_DISCONNECTED:
        SetStatusText(L"Disconnected");
        break;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverParticipantState
//

HRESULT CRTCSession::DeliverParticipantState(IRTCParticipant * pParticipant, RTC_PARTICIPANT_STATE enState)
{
    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverMessage
//

HRESULT CRTCSession::DeliverMessage(IRTCParticipant * pParticipant, BSTR bstrContentType, BSTR bstrMessage)
{
    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverUserStatus
//

HRESULT CRTCSession::DeliverUserStatus(IRTCParticipant * pParticipant, RTC_MESSAGING_USER_STATUS enStatus)
{
    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverMedia
//

HRESULT CRTCSession::DeliverMedia(long lMediaType, RTC_MEDIA_EVENT_TYPE enType, RTC_MEDIA_EVENT_REASON enReason)
{
    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverIntensity
//

HRESULT CRTCSession::DeliverIntensity(RTC_AUDIO_DEVICE enDevice, long lLevel)
{
    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCSession::DeliverClient
//

HRESULT CRTCSession::DeliverClient(RTC_CLIENT_EVENT_TYPE enEventType)
{
    return S_OK;
}