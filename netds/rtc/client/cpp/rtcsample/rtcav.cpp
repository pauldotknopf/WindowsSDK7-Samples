// rtcav.cpp
//

#include "stdafx.h"

CRTCAVSession * CRTCAVSession::m_Singleton = NULL;
HWND CRTCAVSession::m_hRecvVideoParent = NULL;
HWND CRTCAVSession::m_hPrevVideoParent = NULL;

/////////////////////////////////////////////
//
// CRTCAVSession::CRTCAVSession
// 

CRTCAVSession::CRTCAVSession() : CRTCSession()
{
    m_Singleton = this;

    m_fShowRecv = FALSE;
    m_fShowPrev = FALSE;
    m_lRecvWidth = 0;
    m_lRecvHeight = 0;
    m_lPrevWidth = 0;
    m_lPrevHeight = 0;

    m_hMicText = NULL;
    m_hSpkText = NULL;
    m_hMicLevel = NULL;
    m_hSpkLevel = NULL;
    m_hMicVol = NULL;
    m_hSpkVol = NULL;
    m_hMicMute = NULL;
    m_hSpkMute = NULL;
}

/////////////////////////////////////////////
//
// CRTCAVSession::~CRTCAVSession
// 

CRTCAVSession::~CRTCAVSession()
{
    m_Singleton = NULL;
}

/////////////////////////////////////////////
//
// CRTCAVSession::RegisterClass
//

HRESULT CRTCAVSession::RegisterClass()
{
    // Register the window class
    WNDCLASS wc;
    ATOM atom;

    ZeroMemory(&wc, sizeof(WNDCLASS));

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)CRTCAVSession::WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = NULL;
    wc.hCursor       =
        LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 
        GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU_AV);
    wc.lpszClassName = AV_CLASS;

    atom = ::RegisterClass( &wc );

    if ( !atom )
    {
        // RegisterClass failed
        return E_FAIL;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::WindowProc
// 

LRESULT CALLBACK CRTCAVSession::WindowProc(
  HWND hWnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
    CRTCAVSession * me = NULL;
    LRESULT  lr = 0;

    if ( uMsg == WM_CREATE )
    {
        // Create an instance of the class
        me = new CRTCAVSession;
        me->m_hWnd = hWnd;

        // Store the class instance pointer in the
        // window's user data for later retrieval
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)me);
       
        lr = me->OnCreate(uMsg, wParam, lParam);
    }
    else
    {
        // Retrieve the class instance pointer from the
        // window's user data
        me = (CRTCAVSession *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        switch( uMsg )
        {        
        case WM_DESTROY:
            lr = me->OnDestroy(uMsg, wParam, lParam);

            // Delete the object instance
            delete me;

            break;

        case WM_CLOSE:
            lr = me->OnClose(uMsg, wParam, lParam);
            break;

        case WM_SIZE:
            lr = me->OnSize(uMsg, wParam, lParam);
            break;

        case WM_COMMAND:
            lr = me->OnCommand(uMsg, wParam, lParam);
            break;

        case WM_HSCROLL:
            lr = me->OnHScroll(uMsg, wParam, lParam);
            break;

        default:
            lr = DefWindowProc( hWnd, uMsg, wParam, lParam );
        }
    }

    return lr;
}

/////////////////////////////////////////////
//
// CRTCAVSession::DoMute
// 

HRESULT CRTCAVSession::DoMute(RTC_AUDIO_DEVICE enDevice)
{
    IRTCClient * pClient = NULL;
    BOOL fMute = FALSE;
    HWND hWnd = NULL;
    HRESULT hr;

    hr = m_pSession->get_Client(&pClient);

    if (FAILED(hr))
    {
        // get_Client failed
        return hr;
    }

    if (enDevice == RTCAD_SPEAKER)
    {
        hWnd = m_hSpkMute;
    }
    else
    {
        hWnd = m_hMicMute;
    }

    // Get current check state
    fMute = (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Toggle the check state
    fMute = !fMute;

    // Set the current check state
    SendMessage(hWnd, BM_SETCHECK, (WPARAM)(fMute ? BST_CHECKED : BST_UNCHECKED), 0);

    // Set the mute
    hr = pClient->put_AudioMuted(enDevice, fMute ? VARIANT_TRUE : VARIANT_FALSE);

    if (FAILED(hr))
    {
        // put_AudioMute failed
        SAFE_RELEASE(pClient);
        return hr;
    }

    SAFE_RELEASE(pClient);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::DoVolume
// 

HRESULT CRTCAVSession::DoVolume(RTC_AUDIO_DEVICE enDevice, long lVolume)
{
    IRTCClient * pClient = NULL;
    HRESULT hr;

    hr = m_pSession->get_Client(&pClient);

    if (FAILED(hr))
    {
        // get_Client failed
        return hr;
    }

    // Set the volume
    hr = pClient->put_Volume(enDevice, lVolume);

    if (FAILED(hr))
    {
        // put_Volume failed
        SAFE_RELEASE(pClient);
        return hr;
    }

    SAFE_RELEASE(pClient);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::ShowAudio
// 

HRESULT CRTCAVSession::ShowAudio()
{
    IRTCClient * pClient = NULL;
    VARIANT_BOOL fMute = FALSE;
    long lVolume = 0;
    long lMediaCaps = 0;
    HRESULT hr;

    hr = m_pSession->get_Client(&pClient);

    if (FAILED(hr))
    {
        // get_Client failed
        return hr;
    }

    // Get the media capabilities
    hr = pClient->get_MediaCapabilities(&lMediaCaps);

    if (FAILED(hr))
    {
        // get_MediaCapabilities failed
        SAFE_RELEASE(pClient);
        return hr;
    }

    if (lMediaCaps & RTCMT_AUDIO_RECEIVE)
    {
        // Enable the speaker windows
        EnableWindow(m_hSpkText, TRUE);
        EnableWindow(m_hSpkLevel, TRUE);
        EnableWindow(m_hSpkVol, TRUE);
        EnableWindow(m_hSpkMute, TRUE);

        // Show the speaker mute state
        hr = pClient->get_AudioMuted(RTCAD_SPEAKER, &fMute);

        if (FAILED(hr))
        {
            // get_AudioMuted failed
            SAFE_RELEASE(pClient);
            return hr;
        }

        SendMessage(m_hSpkMute, BM_SETCHECK, fMute ? BST_CHECKED : BST_UNCHECKED, 0);

        // Show the speaker volume
        hr = pClient->get_Volume(RTCAD_SPEAKER, &lVolume);

        if (FAILED(hr))
        {
            // get_Volume failed
            SAFE_RELEASE(pClient);
            return hr;
        }

        SendMessage(m_hSpkVol, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)lVolume);
    }
    else
    {
        // Disable the speaker windows
        EnableWindow(m_hSpkText, FALSE);
        EnableWindow(m_hSpkLevel, FALSE);
        EnableWindow(m_hSpkVol, FALSE);
        EnableWindow(m_hSpkMute, FALSE);
    }

    if (lMediaCaps & RTCMT_AUDIO_SEND)
    {
        // Enable the microphone windows
        EnableWindow(m_hMicText, TRUE);
        EnableWindow(m_hMicLevel, TRUE);
        EnableWindow(m_hMicVol, TRUE);
        EnableWindow(m_hMicMute, TRUE);        

        // Show the microphone mute state
        hr = pClient->get_AudioMuted(RTCAD_MICROPHONE, &fMute);

        if (FAILED(hr))
        {
            // get_AudioMuted failed
            SAFE_RELEASE(pClient);
            return hr;
        }

        SendMessage(m_hMicMute, BM_SETCHECK, fMute ? BST_CHECKED : BST_UNCHECKED, 0);
   
        // Show the microphone volume
        hr = pClient->get_Volume(RTCAD_MICROPHONE, &lVolume);

        if (FAILED(hr))
        {
            // get_Volume failed
            SAFE_RELEASE(pClient);
            return hr;
        }

        SendMessage(m_hMicVol, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)lVolume);
    }
    else
    {
        // Disable the microphone windows
        EnableWindow(m_hMicText, FALSE);
        EnableWindow(m_hMicLevel, FALSE);
        EnableWindow(m_hMicVol, FALSE);
        EnableWindow(m_hMicMute, FALSE);
    }

    SAFE_RELEASE(pClient);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::ShowVideo
// 

HRESULT CRTCAVSession::ShowVideo(RTC_VIDEO_DEVICE enDevice, BOOL fShow)
{
    IVideoWindow * pVid = NULL;
    IRTCClient * pClient = NULL;
    long lMediaCaps = 0;
    HWND hWnd = NULL;
    HRESULT hr;

    hr = m_pSession->get_Client(&pClient);

    if (FAILED(hr))
    {
        // get_Client failed
        return hr;
    }

    // Get the media capabilities
    hr = pClient->get_MediaCapabilities(&lMediaCaps);

    if (FAILED(hr))
    {
        // get_MediaCapabilities failed
        SAFE_RELEASE(pClient);
        return hr;
    }

    hr = pClient->get_IVideoWindow(
        enDevice, &pVid);

    SAFE_RELEASE(pClient);

    if (FAILED(hr))
    {
        // get_IVideoWindow failed
        return hr;
    }

    if (enDevice == RTCVD_PREVIEW)
    {          
        // Determine whether to show the preview video
        fShow = fShow && (lMediaCaps & RTCMT_VIDEO_SEND);
        m_fShowPrev = fShow;
        hWnd = m_hPrevVideoParent;

        // Show or hide the preview video preview parent window
        ShowWindow(m_hPrevVideoParent, fShow ? SW_SHOW : SW_HIDE);

        // Set window region for receive video parent window        
        HRGN hRegion;

        if (fShow)
        {
            // Set the receive video region to make room for the preview
            // video picture-in-picture
            POINT rgPoints[6] =
                    { 0, 0,
                      0, m_lRecvHeight,
                      m_lRecvWidth - m_lPrevWidth, m_lRecvHeight,
                      m_lRecvWidth - m_lPrevWidth, m_lRecvHeight - m_lPrevHeight,
                      m_lRecvWidth, m_lRecvHeight - m_lPrevHeight,
                      m_lRecvWidth, 0
                    };

            hRegion = CreatePolygonRgn(rgPoints, 6, ALTERNATE);
        }
        else
        {
            // Set the receive video region to be rectangular
            hRegion = CreateRectRgn(0, 0, m_lRecvWidth, m_lRecvHeight);
        }

        SetWindowRgn(m_hRecvVideoParent, hRegion, TRUE);
    }
    else
    {
        // Determine whether to show the receive video        
        fShow = fShow && (lMediaCaps & RTCMT_VIDEO_RECEIVE);
        m_fShowRecv = fShow;
        hWnd = m_hRecvVideoParent;

        // Always show the video receive parent window
        ShowWindow(m_hRecvVideoParent, SW_SHOW);
    }

    if ( fShow == TRUE )
    {
        // Set the video window style
        pVid->put_WindowStyle( WS_CHILD |
                               WS_CLIPCHILDREN |
                               WS_CLIPSIBLINGS );
        
        // Set the parent window for the video window
        pVid->put_Owner( (OAHWND)hWnd );

        RECT rc;
        GetClientRect(hWnd, &rc );  
        
        // Position the video window
        pVid->SetWindowPosition(
            rc.left,
            rc.top,
            rc.right,
            rc.bottom
            );

        // Make the video window visible
        pVid->put_Visible(-1);        
    }       

    SAFE_RELEASE(pVid);    

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::OnCreate
// 

LRESULT CRTCAVSession::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Call the base class
    CRTCSession::OnCreate(uMsg, wParam, lParam);

    if (m_hRecvVideoParent == NULL)
    {
        // Create the receive video parent window
        m_hRecvVideoParent = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            NULL,
            WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0,
            0, 0,
            m_hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL);
    
        if ( !m_hRecvVideoParent )
        {
            // CreateWindowExW failed
            return -1;
        }
    }
    else
    {
        // Take ownership of the receive video parent window
        if (!SetParent(m_hRecvVideoParent, m_hWnd))
        {
            // SetParent failed
            return -1;
        }
    }

    if (m_hPrevVideoParent == NULL)
    {
        // Create the preview video parent window
        m_hPrevVideoParent = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            NULL,
            WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0,
            0, 0,
            m_hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL);
    
        if ( !m_hPrevVideoParent )
        {
            // CreateWindowExW failed
            return -1;
        }
    }
    else
    {
        // Take ownership of the preview video parent window
        if (!SetParent(m_hPrevVideoParent, m_hWnd))
        {
            // SetParent failed
            return -1;
        }
    }

    // Create the microphone text window
    m_hMicText = CreateWindowExW(
        0,
        L"STATIC",
        L"Microphone",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hMicText )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the speaker text window
    m_hSpkText = CreateWindowExW(
        0,
        L"STATIC",
        L"Speaker",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hSpkText )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the microphone level window
    m_hMicLevel = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hMicLevel )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the speaker level window
    m_hSpkLevel = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hSpkLevel )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the microphone volume window
    m_hMicVol = CreateWindowExW(
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_MIC_VOL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hMicVol )
    {
        // CreateWindowExW failed
        return -1;
    }

    SendMessage(m_hMicVol, TBM_SETRANGEMAX, (WPARAM)FALSE, (LPARAM)0xFFFF);

    // Create the speaker volume window
    m_hSpkVol = CreateWindowExW(
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_SPK_VOL,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hSpkVol )
    {
        // CreateWindowExW failed
        return -1;
    }

    SendMessage(m_hSpkVol, TBM_SETRANGEMAX, (WPARAM)FALSE, (LPARAM)0xFFFF);

    // Create the microphone mute window
    m_hMicMute = CreateWindowExW(
        0,
        L"BUTTON",
        L"Mute",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_CHECKBOX,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_MIC_MUTE,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hMicMute )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the speaker mute window
    m_hSpkMute = CreateWindowExW(
        0,
        L"BUTTON",
        L"Mute",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_CHECKBOX,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_SPK_MUTE,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hSpkMute )
    {
        // CreateWindowExW failed
        return -1;
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCAVSession::OnDestroy
// 

LRESULT CRTCAVSession::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
    // Orphan the receive video parent window
    ShowWindow(m_hRecvVideoParent, SW_HIDE);
    SetParent(m_hRecvVideoParent, NULL);

    // Orphan the preview video parent window
    ShowWindow(m_hPrevVideoParent, SW_HIDE);
    SetParent(m_hPrevVideoParent, NULL);

    // Call the base class
    CRTCSession::OnDestroy(uMsg, wParam, lParam);
   
    return 0;
}

/////////////////////////////////////////////
//
// CRTCAVSession::OnSize
// 

LRESULT CRTCAVSession::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Call the base class
    CRTCSession::OnSize(uMsg, wParam, lParam);

    RECT rcWnd, rcStatusBar;
    const int SIZE_EDGE = 5;
    const int TEXT_HEIGHT = 16;
    const int LEVEL_HEIGHT = 10;
    const int VOL_HEIGHT = 20;   

    GetClientRect(m_hWnd, &rcWnd);    
    GetClientRect(m_hStatusBar, &rcStatusBar);

    // Calculate video window size
    float fAspectRatio = (float)QCIF_CY_SIZE/(float)QCIF_CX_SIZE;
    m_lRecvWidth = rcWnd.right - rcWnd.left - 2*SIZE_EDGE;
    m_lRecvHeight = fAspectRatio * m_lRecvWidth;
    m_lPrevWidth = QQCIF_CX_SIZE;
    m_lPrevHeight = QQCIF_CY_SIZE;

    // Resize the receive video parent window  
    RECT rcRecvVid;

    rcRecvVid.left = rcWnd.left + SIZE_EDGE; 
    rcRecvVid.right = rcWnd.left + m_lRecvWidth + SIZE_EDGE;
    rcRecvVid.top = rcWnd.top + SIZE_EDGE;
    rcRecvVid.bottom = rcWnd.top + m_lRecvHeight + SIZE_EDGE;
    
    MoveWindow(
        m_hRecvVideoParent,
        rcRecvVid.left,
        rcRecvVid.top,
        m_lRecvWidth,
        m_lRecvHeight,
        TRUE);

    // Resize the preview video parent window
    RECT rcPrevVid;

    rcPrevVid.left = rcRecvVid.right - m_lPrevWidth; 
    rcPrevVid.right = rcRecvVid.right;
    rcPrevVid.top = rcRecvVid.bottom - m_lPrevHeight;
    rcPrevVid.bottom = rcRecvVid.bottom;         

    MoveWindow(
        m_hPrevVideoParent,
        rcPrevVid.left,
        rcPrevVid.top,
        m_lPrevWidth,
        m_lPrevHeight,
        TRUE);

    // Update video windows
    ShowVideo(RTCVD_RECEIVE, m_fShowRecv);
    ShowVideo(RTCVD_PREVIEW, m_fShowPrev);

    // Resize the speaker text window
    RECT rcSpkText;

    rcSpkText.left = rcRecvVid.left;
    rcSpkText.right = rcRecvVid.left + m_lRecvWidth/2 - SIZE_EDGE;
    rcSpkText.top = rcRecvVid.bottom + SIZE_EDGE;
    rcSpkText.bottom = rcSpkText.top + TEXT_HEIGHT;

    MoveWindow(
        m_hSpkText,
        rcSpkText.left,
        rcSpkText.top,
        (rcSpkText.right - rcSpkText.left),
        (rcSpkText.bottom - rcSpkText.top),
        TRUE);

    // Resize the microphone text window
    RECT rcMicText;

    rcMicText.left = rcRecvVid.right - m_lRecvWidth/2 + SIZE_EDGE;
    rcMicText.right = rcRecvVid.right;
    rcMicText.top = rcRecvVid.bottom + SIZE_EDGE;
    rcMicText.bottom = rcMicText.top + TEXT_HEIGHT;

    MoveWindow(
        m_hMicText,
        rcMicText.left,
        rcMicText.top,
        (rcMicText.right - rcMicText.left),
        (rcMicText.bottom - rcMicText.top),
        TRUE);

    // Resize the speaker level window
    RECT rcSpkLevel;

    rcSpkLevel.left = rcSpkText.left;
    rcSpkLevel.right = rcSpkText.right;
    rcSpkLevel.top = rcSpkText.bottom + SIZE_EDGE;
    rcSpkLevel.bottom = rcSpkLevel.top + LEVEL_HEIGHT;

    MoveWindow(
        m_hSpkLevel,
        rcSpkLevel.left,
        rcSpkLevel.top,
        (rcSpkLevel.right - rcSpkLevel.left),
        (rcSpkLevel.bottom - rcSpkLevel.top),
        TRUE);

    // Resize the microphone level window
    RECT rcMicLevel;

    rcMicLevel.left = rcMicText.left;
    rcMicLevel.right = rcMicText.right;
    rcMicLevel.top = rcMicText.bottom + SIZE_EDGE;
    rcMicLevel.bottom = rcMicLevel.top + LEVEL_HEIGHT;

    MoveWindow(
        m_hMicLevel,
        rcMicLevel.left,
        rcMicLevel.top,
        (rcMicLevel.right - rcMicLevel.left),
        (rcMicLevel.bottom - rcMicLevel.top),
        TRUE);


    // Resize the speaker volume window
    RECT rcSpkVol;

    rcSpkVol.left = rcSpkLevel.left;
    rcSpkVol.right = rcSpkLevel.right;
    rcSpkVol.top = rcSpkLevel.bottom + SIZE_EDGE;
    rcSpkVol.bottom = rcSpkVol.top + VOL_HEIGHT;

    MoveWindow(
        m_hSpkVol,
        rcSpkVol.left,
        rcSpkVol.top,
        (rcSpkVol.right - rcSpkVol.left),
        (rcSpkVol.bottom - rcSpkVol.top),
        TRUE);

    // Resize the microphone volume window
    RECT rcMicVol;

    rcMicVol.left = rcMicLevel.left;
    rcMicVol.right = rcMicLevel.right;
    rcMicVol.top = rcMicLevel.bottom + SIZE_EDGE;
    rcMicVol.bottom = rcMicVol.top + VOL_HEIGHT;

    MoveWindow(
        m_hMicVol,
        rcMicVol.left,
        rcMicVol.top,
        (rcMicVol.right - rcMicVol.left),
        (rcMicVol.bottom - rcMicVol.top),
        TRUE);

    // Resize the speaker mute window
    RECT rcSpkMute;

    rcSpkMute.left = rcSpkVol.left;
    rcSpkMute.right = rcSpkVol.right;
    rcSpkMute.top = rcSpkVol.bottom + SIZE_EDGE;
    rcSpkMute.bottom = rcSpkMute.top + TEXT_HEIGHT;

    MoveWindow(
        m_hSpkMute,
        rcSpkMute.left,
        rcSpkMute.top,
        (rcSpkMute.right - rcSpkMute.left),
        (rcSpkMute.bottom - rcSpkMute.top),
        TRUE);

    // Resize the microphone mute window
    RECT rcMicMute;

    rcMicMute.left = rcMicVol.left;
    rcMicMute.right = rcMicVol.right;
    rcMicMute.top = rcMicVol.bottom + SIZE_EDGE;
    rcMicMute.bottom = rcMicMute.top + TEXT_HEIGHT;

    MoveWindow(
        m_hMicMute,
        rcMicMute.left,
        rcMicMute.top,
        (rcMicMute.right - rcMicMute.left),
        (rcMicMute.bottom - rcMicMute.top),
        TRUE);

    // Update the audio windows
    ShowAudio();

    return 0;
}

/////////////////////////////////////////////
//
// CRTCAVSession::OnCommand
// 

LRESULT CRTCAVSession::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case ID_FILE_EXIT:
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        break;

    case IDC_MIC_MUTE:
        DoMute(RTCAD_MICROPHONE);        
        break;

    case IDC_SPK_MUTE:
        DoMute(RTCAD_SPEAKER);
        break;
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCAVSession::OnHScroll
// 

LRESULT CRTCAVSession::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ((LOWORD(wParam) == SB_THUMBPOSITION) ||
        (LOWORD(wParam) == SB_THUMBTRACK))
    {
        if ((HWND)lParam == m_hSpkVol)
        {
            // Speaker volume adjusted
            DoVolume(RTCAD_SPEAKER, HIWORD(wParam));
        }
        else if ((HWND)lParam == m_hMicVol)
        {
            // Microphone volume adjusted
            DoVolume(RTCAD_MICROPHONE, HIWORD(wParam));
        }
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCAVSession::DeliverMedia
//

HRESULT CRTCAVSession::DeliverMedia(long lMediaType, RTC_MEDIA_EVENT_TYPE enType, RTC_MEDIA_EVENT_REASON enReason)
{
    RTC_VIDEO_DEVICE enVideo;
    BOOL fShow;
    HRESULT hr;

    // Which media type changed?
    switch (lMediaType)
    {
    case RTCMT_VIDEO_SEND:   
        enVideo = RTCVD_PREVIEW;
        break;

    case RTCMT_VIDEO_RECEIVE:
        enVideo = RTCVD_RECEIVE;
        break;

    default:
        return S_OK;
    }

    // Did the media start or stop?
    switch (enType) 
    {
    case RTCMET_STARTED:
        fShow = TRUE;
        break;

    case RTCMET_STOPPED:
    case RTCMET_FAILED:
        fShow = FALSE;
        break;

    default:
        return S_OK;
    }

    // Show or hide the video as appropriate
    hr = ShowVideo(enVideo, fShow);

    if (FAILED(hr))
    {
        // ShowVideo failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::DeliverIntensity
//

HRESULT CRTCAVSession::DeliverIntensity(RTC_AUDIO_DEVICE enDevice, long lLevel)
{
    switch(enDevice)
    {
    case RTCAD_SPEAKER:
        // Set the position of the speaker progress bar
        SendMessage(m_hSpkLevel, PBM_SETPOS, (WPARAM)lLevel, 0);
        break;

    case RTCAD_MICROPHONE:
        // Set the position of the microphone progress bar
        SendMessage(m_hMicLevel, PBM_SETPOS, (WPARAM)lLevel, 0);
        break;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCAVSession::DeliverClient
//

HRESULT CRTCAVSession::DeliverClient(RTC_CLIENT_EVENT_TYPE enEventType)
{
    switch(enEventType)
    {
    case RTCCET_VOLUME_CHANGE:
        // On volume change refresh the audio controls
        // so volume controls can be updated
        ShowAudio();
        break;

    case RTCCET_DEVICE_CHANGE:
        // On device change refresh the audio and video
        // controls as available devices may have changed
        ShowAudio();
        ShowVideo(RTCVD_PREVIEW, m_fShowPrev);
        ShowVideo(RTCVD_RECEIVE, m_fShowRecv);
        break;
    }

    return S_OK;
}