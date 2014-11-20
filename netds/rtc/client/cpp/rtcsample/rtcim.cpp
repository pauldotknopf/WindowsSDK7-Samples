// rtcim.cpp
//

#include "stdafx.h"

// Font definition
static CHARFORMAT cfDefault =
{
	sizeof(CHARFORMAT),
	CFM_EFFECTS | CFM_PROTECTED | CFM_SIZE | CFM_OFFSET | CFM_COLOR | CFM_CHARSET | CFM_FACE,
	CFE_AUTOCOLOR,		// effects
	200,				// height, 200 twips == 10 points
	0,					// offset
	0,					// color (not used since CFE_AUTOCOLOR is specified)
	DEFAULT_CHARSET,
	FF_SWISS,			// pitch and family
	_T("Microsoft Sans Serif") // face name
};

/////////////////////////////////////////////
//
// CRTCIMSession::CRTCIMSession
// 

CRTCIMSession::CRTCIMSession() : CRTCSession()
{
    m_hDisplay = NULL;
    m_hEdit = NULL;
    m_hSend = NULL;
    m_fTyping = FALSE;
    m_wpOrigEditProc = NULL;
}

/////////////////////////////////////////////
//
// CRTCIMSession::~CRTCIMSession
// 

CRTCIMSession::~CRTCIMSession()
{
}

/////////////////////////////////////////////
//
// CRTCIMSession::RegisterClass
//

HRESULT CRTCIMSession::RegisterClass()
{
    // Register the window class
    WNDCLASS wc;
    ATOM atom;

    ZeroMemory(&wc, sizeof(WNDCLASS));

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)CRTCIMSession::WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = NULL;
    wc.hCursor       =
        LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 
        GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU_IM);
    wc.lpszClassName = IM_CLASS;

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
// CRTCIMSession::WindowProc
// 

LRESULT CALLBACK CRTCIMSession::WindowProc(
  HWND hWnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
    CRTCIMSession * me = NULL;
    LRESULT  lr = 0;

    if ( uMsg == WM_CREATE )
    {
        // Create an instance of the class
        me = new CRTCIMSession;
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
        me = (CRTCIMSession *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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

        default:
            lr = DefWindowProc( hWnd, uMsg, wParam, lParam );
        }
    }

    return lr;
}

/////////////////////////////////////////////
//
// CRTCIMSession::EditSubclassProc
// 

LRESULT CALLBACK CRTCIMSession::EditSubclassProc(
  HWND hWnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
    CRTCIMSession * me = NULL;
    me = (CRTCIMSession *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if ( uMsg == WM_KEYDOWN )
    {
        // Intercept the return key from the edit box
        // and press the send button
        if ( (wParam == VK_RETURN) &&
             !(GetKeyState(VK_CONTROL) & 0x8000))
        {
            SendMessage(me->m_hWnd, WM_COMMAND,
                MAKEWPARAM(IDC_IM_SEND, BN_CLICKED),
                (LPARAM)me->m_hSend);

            return 0;
        }
    }

    // Call the original edit window procedure
    return CallWindowProc(
        me->m_wpOrigEditProc,
        hWnd, uMsg, wParam, lParam);
}

/////////////////////////////////////////////
//
// CRTCIMSession::DoSendMessage
//

HRESULT CRTCIMSession::DoSendMessage()
{
    LONG cch;
    BSTR bstrMessage = NULL;
    HRESULT hr;
    
    // Determine the length of the outgoing message
    // in the edit box
    cch = SendMessageW(m_hEdit, WM_GETTEXTLENGTH, 0, 0);

    if (cch > 0)
    {
        // Allocate a buffer for the outgoing message
        bstrMessage = SysAllocStringLen(NULL, cch);

        if (bstrMessage == NULL)
        {
            // SysAllocStringLen failed
            return E_OUTOFMEMORY;
        }

        // Get the outgoing message from the edit box
        if (cch != SendMessageW(m_hEdit, WM_GETTEXT, cch+1, (LPARAM)bstrMessage))
        {
            // SendMessageW failed
            SAFE_FREE_STRING(bstrMessage);
            return E_FAIL;
        }       

        // Get the local user URI
        BSTR bstrURI = NULL;

        hr = m_pWin->GetUserURI(&bstrURI);

        if (FAILED(hr))
        {
            // GetUserURI failed
            SAFE_FREE_STRING(bstrMessage);
            return hr;
        }

        // Display the outgoing message
        DoDisplayMessage(bstrURI, bstrMessage);

        SAFE_FREE_STRING(bstrURI);

        // Clear the edit box
        SetWindowText(m_hEdit, L"");

        // Send the outgoing message
        hr = m_pSession->SendMessage(NULL, bstrMessage, 0);

        SAFE_FREE_STRING(bstrMessage);

        if (FAILED(hr))
        {
            // SendMessage failed
            return hr;
        }     
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCIMSession::DoDisplayMessage
//

HRESULT CRTCIMSession::DoDisplayMessage(PWSTR szUser, PWSTR szMessage)
{
    // Select the end of the text in the edit control
    SendMessage(m_hDisplay, EM_SETSEL, -1, -1);

    // Set format for the user text
    CHARFORMAT cf;
    PARAFORMAT pf;

    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = 0;
    cf.crTextColor = RGB(0,128,128);

    SendMessage(m_hDisplay, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

    pf.cbSize = sizeof(PARAFORMAT);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 50;

    SendMessage(m_hDisplay, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

    // Paste the user text in the edit control
    SendMessageW(m_hDisplay, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)szUser);
    SendMessageW(m_hDisplay, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)L" says:\n");

    // Set format for the message text
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = CFE_AUTOCOLOR;
    cf.crTextColor = 0;

    SendMessage(m_hDisplay, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

    pf.cbSize = sizeof(PARAFORMAT);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 200;

    SendMessage(m_hDisplay, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

    // Paste the message text in the edit control
    SendMessageW(m_hDisplay, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)szMessage);

    // Paste a new line in the edit control
    SendMessageW(m_hDisplay, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)L"\n");

    // Scroll to the bottom of the edit control
    SendMessage(m_hDisplay, WM_VSCROLL, SB_BOTTOM, 0);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCIMSession::DoSendTyping
//

HRESULT CRTCIMSession::DoSendTyping()
{
    BOOL fTyping;
    HRESULT hr;

    // Is the edit box empty?
    fTyping = (SendMessage(m_hEdit, WM_GETTEXTLENGTH, 0, 0) > 0);

    if (m_fTyping != fTyping)
    {
        // Typing status has changed
        m_fTyping = fTyping;

        hr = m_pSession->SendMessageStatus(
            m_fTyping ? RTCMUS_TYPING : RTCMUS_IDLE,
            0);

        if (FAILED(hr))
        {
            // SendMessageStatus failed
            return hr;
        }
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCIMSession::DoDisplayTyping
//

HRESULT CRTCIMSession::DoDisplayTyping(PWSTR szUser, BOOL fTyping)
{
    if (fTyping)
    {
        // Display typing status in the status bar.
        WCHAR szTyping[MAX_STRING];
        _snwprintf_s(szTyping, _countof(szTyping), _TRUNCATE, L"%s is typing.", szUser);

        SetStatusText(szTyping);
    }
    else
    {
        // User is not longer typing. Display the session
        // state in the status bar.
        DeliverSessionState(m_enState);
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCIMSession::OnCreate
// 

LRESULT CRTCIMSession::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)    
{
    // Call the base class
    CRTCSession::OnCreate(uMsg, wParam, lParam);

    // Create the display control
    m_hDisplay = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        RICHEDIT_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_IM_DISPLAY,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hDisplay )
    {
        // CreateWindowEx failed
        return -1;
    }

    SendMessage(m_hDisplay, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cfDefault); 

    // Create the edit control
    m_hEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        RICHEDIT_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_IM_EDIT,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hEdit )
    {
        // CreateWindowEx failed
        return -1;
    }

    SendMessage(m_hEdit, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendMessage(m_hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cfDefault); 
    
    // Subclass the edit window
    m_wpOrigEditProc = (WNDPROC)SetWindowLongPtr(m_hEdit,
        GWLP_WNDPROC, (LONG_PTR)CRTCIMSession::EditSubclassProc);
    SetWindowLongPtr(m_hEdit, GWLP_USERDATA, (LONG_PTR)this);

    // Create the send button
    m_hSend = CreateWindowExW(
        0,
        L"BUTTON",
        L"Send",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_IM_SEND,
        GetModuleHandle(NULL),
        NULL);
    
    if ( !m_hSend )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Set focus to the edit window
    SetFocus(m_hEdit);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCIMSession::OnDestroy
// 

LRESULT CRTCIMSession::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    // Call the base class
    CRTCSession::OnDestroy(uMsg, wParam, lParam);

    // Unsubclass the edit window
    SetWindowLongPtr(m_hEdit, GWLP_WNDPROC, (LONG_PTR)m_wpOrigEditProc);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCIMSession::OnSize
// 

LRESULT CRTCIMSession::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)    
{
    // Call the base class
    CRTCSession::OnSize(uMsg, wParam, lParam);
        
    RECT rcWnd, rcStatusBar, rcDisplay, rcEdit, rcSend;    
    const int SIZE_EDGE = 5;
    const int SIZE_BUTTON = 60;

    GetClientRect(m_hWnd, &rcWnd);    
    GetClientRect(m_hStatusBar, &rcStatusBar);

    // Resize the send button
    rcSend.bottom = rcWnd.bottom -
        (rcStatusBar.bottom - rcStatusBar.top) -
        SIZE_EDGE;
    rcSend.top = rcSend.bottom - SIZE_BUTTON;
    rcSend.right = rcWnd.right - SIZE_EDGE;
    rcSend.left = rcSend.right - SIZE_BUTTON; 

    MoveWindow(
        m_hSend,
        rcSend.left,
        rcSend.top,
        (rcSend.right - rcSend.left),
        (rcSend.bottom - rcSend.top),
        TRUE);

    // Resize the edit control
    rcEdit.bottom = rcSend.bottom;
    rcEdit.top = rcSend.top;
    rcEdit.right = rcSend.left - SIZE_EDGE;
    rcEdit.left = rcWnd.left + SIZE_EDGE;

    MoveWindow(
        m_hEdit,
        rcEdit.left,
        rcEdit.top,
        (rcEdit.right - rcEdit.left),
        (rcEdit.bottom - rcEdit.top),
        TRUE);

    // Resize the display control
    rcDisplay.bottom = rcEdit.top - SIZE_EDGE;
    rcDisplay.top = rcWnd.top + SIZE_EDGE;
    rcDisplay.right = rcWnd.right - SIZE_EDGE;
    rcDisplay.left = rcWnd.left + SIZE_EDGE;
    
    MoveWindow(
        m_hDisplay,
        rcDisplay.left,
        rcDisplay.top,
        (rcDisplay.right - rcDisplay.left),
        (rcDisplay.bottom - rcDisplay.top),
        TRUE);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCIMSession::OnCommand
// 

LRESULT CRTCIMSession::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (HIWORD(wParam) == EN_CHANGE)
    {
        switch(LOWORD(wParam))
        {
        case IDC_IM_EDIT:
            DoSendTyping();
            break;
        }
    }
    else
    {
        switch(LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            PostMessage(m_hWnd, WM_CLOSE, 0, 0);
            break;

        case IDC_IM_SEND:
            DoSendMessage();
            break;
        }
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCIMSession::DeliverMessage
//

HRESULT CRTCIMSession::DeliverMessage(IRTCParticipant * pParticipant, BSTR bstrContentType, BSTR bstrMessage)
{
    HRESULT hr;
    BSTR bstrURI = NULL;

    // Get the URI of the incoming user
    hr = pParticipant->get_UserURI(&bstrURI);

    if (FAILED(hr))
    {
        // get_UserURI failed
        return hr;
    }

    // Display the incoming message
    hr = DoDisplayMessage(bstrURI, bstrMessage);

    if (FAILED(hr))
    {
        // DoDisplayMessage failed
        return hr;
    }

    // Set the incoming user's typing status to idle
    hr = DoDisplayTyping(bstrURI, RTCMUS_IDLE);

    if (FAILED(hr))
    {
        // DoDisplayTyping failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCIMSession::DeliverUserStatus
//

HRESULT CRTCIMSession::DeliverUserStatus(IRTCParticipant * pParticipant, RTC_MESSAGING_USER_STATUS enStatus)
{
    HRESULT hr;
    BSTR bstrURI = NULL;

    // Get the URI of the incoming user
    hr = pParticipant->get_UserURI(&bstrURI);

    if (FAILED(hr))
    {
        // get_UserURI failed
        return hr;
    }

    // Display the typing status of the incoming user
    hr = DoDisplayTyping(bstrURI, (enStatus == RTCMUS_TYPING));

    if (FAILED(hr))
    {
        // DoDisplayTyping failed
        return hr;
    }

    return S_OK;
}
