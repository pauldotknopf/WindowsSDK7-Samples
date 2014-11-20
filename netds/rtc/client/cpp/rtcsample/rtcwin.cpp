// rtcwin.h
//

#include "stdafx.h"

/////////////////////////////////////////////
//
// CRTCWin::CRTCWin
// 

CRTCWin::CRTCWin()
{
    m_hWnd = NULL;
    m_hStatusBar = NULL;
    m_hBuddyList = NULL;
    m_pClient = NULL;
    m_pProfile = NULL;
    m_pEvents = NULL;
    m_enState = RTCRS_NOT_REGISTERED;
    m_fPresenceEnabled = FALSE;
}

/////////////////////////////////////////////
//
// CRTCWin::~CRTCWin
// 

CRTCWin::~CRTCWin()
{
}

/////////////////////////////////////////////
//
// CRTCWin::RegisterClass
//

HRESULT CRTCWin::RegisterClass()
{
    // Register the window class
    WNDCLASS wc;
    ATOM atom;

    ZeroMemory(&wc, sizeof(WNDCLASS));

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)CRTCWin::WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = NULL;
    wc.hCursor       =
        LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 
        GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU_APP);
    wc.lpszClassName = APP_CLASS;

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
// CRTCWin::AddSession
//

HRESULT CRTCWin::AddSession(IRTCSession * pSession, RTC_SESSION_TYPE enType)
{
    // Is this an audio/video session?
    BOOL fAVSession = (enType != RTCST_IM);

    // Create the session window
    HWND hWnd;

    hWnd = CreateWindowExW(
                0,
                (fAVSession) ? AV_CLASS : IM_CLASS,
                (fAVSession) ? AV_TITLE : IM_TITLE,
                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                CW_USEDEFAULT, CW_USEDEFAULT,
                (fAVSession) ? AV_WIDTH : IM_WIDTH,
                (fAVSession) ? AV_HEIGHT : IM_HEIGHT,
                NULL,
                NULL,
                GetModuleHandle(NULL),
                NULL);    

    if ( !hWnd )
    {  
        // CreateWindowExW failed
        return E_FAIL;
    }

    // Initialize the window
    CRTCSession * pSessWindow = 
        (CRTCSession *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    pSessWindow->m_pSession = pSession;
    pSessWindow->m_pSession->AddRef();
    pSessWindow->m_pWin = this;

    // Make the main window visible
    ShowWindow( hWnd, SW_SHOW );
    UpdateWindow( hWnd );

    // Add window to the list
    m_SessionList.push_back(pSessWindow);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::RemoveSession
//

HRESULT CRTCWin::RemoveSession(CRTCSession * pSessWindow)
{
    std::vector<CRTCSession *>::iterator it;
    
    if (!m_SessionList.empty())
    {
        // Find the window
        for(it=m_SessionList.begin();
            it < m_SessionList.end();
            it++)
        {
            if (*it == pSessWindow)
            {
                // Remove the window
                m_SessionList.erase(it);
                break;
            }
        }
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::FindSession
//

HRESULT CRTCWin::FindSession(IRTCSession * pSession,
                    CRTCSession ** ppSessWindow)
{
    std::vector<CRTCSession *>::iterator it;     

    if (!m_SessionList.empty())
    {
        for(it=m_SessionList.begin();
            it < m_SessionList.end();
            it++)
        {
            if ((*it)->m_pSession == pSession)
            {
                *ppSessWindow = (*it);         
                return S_OK;
            }
        }
    }

    *ppSessWindow = NULL;
    return E_FAIL;
}

/////////////////////////////////////////////
//
// CRTCWin::CleanupSessions
//

HRESULT CRTCWin::CleanupSessions()
{
    std::vector<CRTCSession *>::iterator it;     

    if (!m_SessionList.empty())
    {
        for(it=m_SessionList.begin();
            it < m_SessionList.end();
            it++)
        {
            PostMessage((*it)->m_hWnd, WM_CLOSE, 0, 0);
        }
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::ShowMessageBox
//

void CRTCWin::ShowMessageBox(PWSTR szText)
{
    MessageBoxW(m_hWnd, szText, APP_TITLE, MB_OK);
}

/////////////////////////////////////////////
//
// CRTCWin::SetStatusText
//

void CRTCWin::SetStatusText(PWSTR szText)
{
    SetWindowTextW(m_hStatusBar, szText);
    InvalidateRect(m_hStatusBar, NULL, FALSE);
}

/////////////////////////////////////////////
//
// CRTCWin::GetUserURI
//

HRESULT CRTCWin::GetUserURI(BSTR *pbstrURI)
{
    HRESULT hr;

    if (m_pProfile != NULL)
    {
        // Get the user URI from the profile
        hr = m_pProfile->get_UserURI(pbstrURI);

        if (FAILED(hr))
        {
            // get_UserURI failed
            return hr;
        }
    }
    else
    {
        // Get the user URI from the client
        hr = m_pClient->get_LocalUserURI(pbstrURI);

        if (FAILED(hr))
        {
            // get_LocalUserURI failed
            return hr;
        }
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::UpdateBuddyList
//

HRESULT CRTCWin::UpdateBuddyList(IRTCBuddy * pBuddy)
{
    HRESULT hr;

    // Get the buddy status
    RTC_PRESENCE_STATUS enStatus;

    hr = pBuddy->get_Status(&enStatus);

    if (FAILED(hr))
    {
        // get_Status failed
        return hr;
    }

    // Get the buddy name
    BSTR bstrName = NULL;

    hr = pBuddy->get_Name(&bstrName);

    if (SUCCEEDED(hr) && !wcscmp(bstrName, L""))
    {
        // Treat an emptry string as a failure
        SAFE_FREE_STRING(bstrName);
        hr = E_FAIL;
    }

    if (FAILED(hr))
    {
        // get_Name failed, get the URI instead
        hr = pBuddy->get_PresentityURI(&bstrName);

        if (SUCCEEDED(hr) && !wcscmp(bstrName, L""))
        {
            // Treat an emptry string as a failure
            SAFE_FREE_STRING(bstrName);
            hr = E_FAIL;
        }

        if (FAILED(hr))
        {
            // get_PresentityURI failed
            return hr;
        }
    }

    // Build a string for the listbox
    WCHAR szBuddy[MAX_STRING];

    switch(enStatus)
    {
    case RTCXS_PRESENCE_OFFLINE:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Offline)", bstrName);
        break;
    case RTCXS_PRESENCE_ONLINE:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Online)", bstrName);
        break;
    case RTCXS_PRESENCE_AWAY:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Away)", bstrName);
        break;
    case RTCXS_PRESENCE_IDLE:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Idle)", bstrName);
        break;
    case RTCXS_PRESENCE_BUSY:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Busy)", bstrName);
        break;
    case RTCXS_PRESENCE_BE_RIGHT_BACK:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Be right back)", bstrName);
        break;
    case RTCXS_PRESENCE_ON_THE_PHONE:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (On the phone)", bstrName);
        break;
    case RTCXS_PRESENCE_OUT_TO_LUNCH:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws (Out to lunch)", bstrName);
        break;
    default:
        _snwprintf_s(szBuddy, _countof(szBuddy), _TRUNCATE, L"%ws", bstrName);
        break;
    }

    szBuddy[MAX_STRING-1] = L'\0';

    SAFE_FREE_STRING(bstrName);
    
    // Is the buddy in the list?
    LVFINDINFO lvfi;    
    ZeroMemory(&lvfi, sizeof(LVFINDINFO));

    lvfi.flags = LVFI_PARAM;
    lvfi.lParam = (LPARAM)pBuddy;

    int iRes = ListView_FindItem(m_hBuddyList, -1, &lvfi);

    if (iRes == -1)
    {
        // Buddy is not in the list
        // Add the buddy to the list  
        LVITEM lvi;
        ZeroMemory(&lvi, sizeof(LVITEM));

        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.pszText = szBuddy;
        lvi.lParam = (LPARAM)pBuddy;

        iRes = ListView_InsertItem(m_hBuddyList, &lvi);

        if (iRes == -1)
        {
            // ListView_InsertItem failed
            return E_FAIL;
        }

        // An RTC buddy reference will be kept by the list
        // Add the reference here
        pBuddy->AddRef();
    }
    else
    {
        // Buddy is in the list
        // Update the list entry
        LVITEM lvi;
        ZeroMemory(&lvi, sizeof(LVITEM));

        lvi.mask = LVIF_TEXT;
        lvi.pszText = szBuddy;
        lvi.iItem = iRes;

        iRes = ListView_SetItem(m_hBuddyList, &lvi);

        if (iRes == -1)
        {
            // ListView_SetItem failed         
            return E_FAIL;
        }
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::ClearBuddyList
// 

HRESULT CRTCWin::ClearBuddyList()
{
    LVITEM lvi;
    ZeroMemory(&lvi, sizeof(LVITEM));
    
    lvi.mask = LVIF_PARAM;

    // Iterate through the list and remove each item
    while (ListView_GetItem(m_hBuddyList, &lvi))
    {
        IRTCBuddy * pBuddy = (IRTCBuddy *)(lvi.lParam);

        // Release the buddy reference
        SAFE_RELEASE(pBuddy);

        ListView_DeleteItem(m_hBuddyList, 0);
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoLogOn
// 

HRESULT CRTCWin::DoLogOn(BSTR bstrURI, BSTR bstrServer, BSTR bstrTransport)
{
    HRESULT hr;

    if (m_pProfile)
    {
        // Already logged on
        ShowMessageBox(L"Already logged on!");

        return S_FALSE;
    }

    // Build an XML provisioning document
    BSTR bstrXML = NULL; 

    hr = DoGenerateProvisioning(
        bstrURI, bstrServer, bstrTransport, &bstrXML);

    if (FAILED(hr))
    {
        // DoGenerateProvisioning failed
        ShowMessageBox(L"Logon failed!");

        return hr;
    }

    // Get the RTC client provisioning interface
    IRTCClientProvisioning * pProv = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientProvisioning,
            (void **)&pProv);

    if (FAILED(hr))
    {
        // QueryInterface failed
        ShowMessageBox(L"Logon failed!");

        return hr;
    }

    // Create a RTC profile object from the XML
    // provisioning document
    hr = pProv->CreateProfile(bstrXML, &m_pProfile);

    SAFE_FREE_STRING(bstrXML);

    if (FAILED(hr))
    {
        // CreateProfile failed
        ShowMessageBox(L"Logon failed!");

        SAFE_RELEASE(pProv);
        return hr;
    }

    // Enable the RTC profile object
    hr = pProv->EnableProfile(m_pProfile, RTCRF_REGISTER_ALL);

    SAFE_RELEASE(pProv);

    if (FAILED(hr))
    {
        // EnableProfile failed
        SAFE_RELEASE(m_pProfile);

        ShowMessageBox(L"Logon failed!");        
        return hr;    
    }

    // Enable presence
    // It is best to enable presence immediately after enabling the
    // profile so that incoming watchers will not be lost.
    hr = DoEnablePresence(TRUE);

    if (FAILED(hr))
    {
        // DoEnablePresence failed
        DoLogOff();

        ShowMessageBox(L"Logon failed!");
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::OnLoggingOn
// 

HRESULT CRTCWin::OnLoggingOn()
{
    // Enable/disable menu items
    HMENU hMenu = GetMenu(m_hWnd);

    EnableMenuItem(hMenu, ID_FILE_LOGON, MF_GRAYED);
    EnableMenuItem(hMenu, ID_FILE_LOGOFF, MF_ENABLED);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::OnLoggedOn
// 

HRESULT CRTCWin::OnLoggedOn()
{
    HRESULT hr;

    // Get the RTC client presence interface
    IRTCClientPresence * pPresence = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientPresence,
            (void **)&pPresence);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Enumerate buddies and populate list
    IRTCEnumBuddies * pEnum = NULL;
    IRTCBuddy * pBuddy = NULL;

    hr = pPresence->EnumerateBuddies(&pEnum);

    SAFE_RELEASE(pPresence);

    if (FAILED(hr))
    {
        // Enumerate buddies failed
        return hr;
    }

    while (pEnum->Next(1, &pBuddy, NULL) == S_OK)
    {
        // Update the buddy list entry
        UpdateBuddyList(pBuddy);

        SAFE_RELEASE(pBuddy);
    }

    SAFE_RELEASE(pEnum);

    // Enable/disable menu items
    HMENU hMenu = GetMenu(m_hWnd);

    EnableMenuItem(hMenu, ID_FILE_LOGON, MF_GRAYED);
    EnableMenuItem(hMenu, ID_FILE_LOGOFF, MF_ENABLED);
    EnableMenuItem(hMenu, ID_ACTION_ADDBUDDY, MF_ENABLED);    

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoLogOff
// 

HRESULT CRTCWin::DoLogOff()
{
    HRESULT hr;

    if (!m_pProfile)
    {
        // Already logged off
        return S_OK;
    }

    // Get the RTC client provisioning interface
    IRTCClientProvisioning * pProv = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientProvisioning,
            (void **)&pProv);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Disable the profile
    pProv->DisableProfile(m_pProfile);

    SAFE_RELEASE(m_pProfile);
    SAFE_RELEASE(pProv);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::OnLoggingOff
// 

HRESULT CRTCWin::OnLoggingOff()
{
    // Enable/disable menu items
    HMENU hMenu = GetMenu(m_hWnd);

    EnableMenuItem(hMenu, ID_FILE_LOGON, MF_GRAYED);
    EnableMenuItem(hMenu, ID_FILE_LOGOFF, MF_GRAYED);
    EnableMenuItem(hMenu, ID_ACTION_ADDBUDDY, MF_GRAYED);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::OnLoggedOff
// 

HRESULT CRTCWin::OnLoggedOff()
{
    // Disable presence
    DoEnablePresence(FALSE);    

    // Enable/disable menu items
    HMENU hMenu = GetMenu(m_hWnd);

    EnableMenuItem(hMenu, ID_FILE_LOGON, MF_ENABLED);
    EnableMenuItem(hMenu, ID_FILE_LOGOFF, MF_GRAYED);
    EnableMenuItem(hMenu, ID_ACTION_ADDBUDDY, MF_GRAYED);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoGenerateProvisioning
// 

HRESULT CRTCWin::DoGenerateProvisioning(BSTR bstrURI, BSTR bstrServer,
                                        BSTR bstrTransport, BSTR *pbstrXML)
{
    WCHAR szXML[MAX_XML_STRING];
    WCHAR szRealm[MAX_STRING];

    // Allow basic authentication if the protocol is TLS
    BOOL fBasicAuth = !_wcsicmp(bstrTransport, L"TLS");

    // Use the domain of the user uri as the realm        
    WCHAR *ch = wcschr(bstrURI, L'@');

    if (ch == NULL)
    {
        // User URI has no domain, use empty string
        wcsncpy_s(szRealm, _countof(szRealm), L"", _TRUNCATE);
    }
    else
    {   
        // Found the domain, copy the string
        ch++;
        wcsncpy_s(szRealm, _countof(szRealm), ch, _TRUNCATE);
    }

    // Generate the XML provisioning document
    _snwprintf_s(
        szXML,
		_countof(szXML),
        _TRUNCATE,
        L"<provision key=\"RTC Sample\" name=\"RTC Sample\">"
        L"<user uri=\"%s\" account=\"\" password=\"\" realm=\"%s\" />"
        L"<sipsrv addr=\"%s\" protocol=\"%s\" %s role=\"proxy\">"
        L"<session party=\"first\" type=\"pc2pc\" />"
        L"<session party=\"first\" type=\"pc2ph\" />"
        L"<session party=\"first\" type=\"im\" />"
        L"</sipsrv>"
        L"<sipsrv addr=\"%s\" protocol=\"%s\" %s role=\"registrar\" />"
        L"</provision>",
        bstrURI, szRealm,
        bstrServer, bstrTransport, fBasicAuth ? L"auth=\"basic\"" : L"",
        bstrServer, bstrTransport, fBasicAuth ? L"auth=\"basic\"" : L""
        );

    *pbstrXML = SysAllocString(szXML);

    if (!(*pbstrXML))
    {
        // Out of memory
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoAuth
// 

HRESULT CRTCWin::DoAuth(BSTR bstrURI, BSTR bstrAccount, BSTR bstrPassword)
{
    HRESULT hr;

    // Update the credentials in the profile
    hr = m_pProfile->SetCredentials(bstrURI, bstrAccount, bstrPassword);

    if (FAILED(hr))
    {
        // SetCredentials failed
        return hr;
    }

    // Get the RTC client provisioning interface
    IRTCClientProvisioning * pProv = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientProvisioning,
            (void **)&pProv);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Re-enable the RTC profile object
    hr = pProv->EnableProfile(m_pProfile, RTCRF_REGISTER_ALL);

    SAFE_RELEASE(pProv);

    if (FAILED(hr))
    {
        // EnableProfile failed
        return hr;    
    }

    // Re-enable presence
    hr = DoEnablePresence(TRUE);

    if (FAILED(hr))
    {
        // DoEnablePresence failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoEnablePresence
// 

HRESULT CRTCWin::DoEnablePresence(BOOL fEnable)
{    
    IRTCClientPresence * pPresence = NULL;
    HRESULT hr;

    // Cleanup the buddy list
    ClearBuddyList();

    // Get the RTC client presence interface
    hr = m_pClient->QueryInterface(
            IID_IRTCClientPresence,
            (void **)&pPresence);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Get the location of the presence storage
    VARIANT varStorage;
    VariantInit(&varStorage);
    varStorage.vt = VT_BSTR;
    varStorage.bstrVal = SysAllocString(L"presence.xml");

    // If we are disabling presence, save the latest
    // copy of the presence data
    if (!fEnable && m_fPresenceEnabled)
    {
        hr = pPresence->Export(varStorage);

        if (FAILED(hr))
        {
            // Export failed
            SAFE_RELEASE(pPresence);
            VariantClear(&varStorage);
            return hr;
        }
    }

    // Enable presence
    hr = pPresence->EnablePresence(
        fEnable ? VARIANT_TRUE : VARIANT_FALSE, varStorage);
    
    VariantClear(&varStorage);

    if (FAILED(hr))
    {
        // EnablePresence failed
        SAFE_RELEASE(pPresence);
        return hr;
    }

    // Set the enabled flag
    m_fPresenceEnabled = fEnable;

    // If we are disabling presence, cleanup the
    // presence data
    if (!fEnable)
    {
        // Cleanup buddies
        IRTCEnumBuddies * pEnumBuddy = NULL;
        IRTCBuddy * pBuddy = NULL;

        hr = pPresence->EnumerateBuddies(&pEnumBuddy);
   
        if (FAILED(hr))
        {
            // Enumerate buddies failed
            SAFE_RELEASE(pPresence);
            return hr;
        }

        while (pEnumBuddy->Next(1, &pBuddy, NULL) == S_OK)
        {
            pPresence->RemoveBuddy(pBuddy);

            SAFE_RELEASE(pBuddy);
        }

        SAFE_RELEASE(pEnumBuddy);

        // Cleanup watchers
        IRTCEnumWatchers * pEnumWatcher = NULL;
        IRTCWatcher * pWatcher = NULL;

        hr = pPresence->EnumerateWatchers(&pEnumWatcher);
   
        if (FAILED(hr))
        {
            // Enumerate watchers failed
            SAFE_RELEASE(pPresence);
            return hr;
        }

        while (pEnumWatcher->Next(1, &pWatcher, NULL) == S_OK)
        {
            pPresence->RemoveWatcher(pWatcher);

            SAFE_RELEASE(pWatcher);
        }

        SAFE_RELEASE(pEnumWatcher);
    }

    SAFE_RELEASE(pPresence);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoAddBuddy
// 

HRESULT CRTCWin::DoAddBuddy(BSTR bstrURI, BSTR bstrName)
{
    HRESULT hr;

    // Get the RTC client presence interface
    IRTCClientPresence * pPresence = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientPresence,
            (void **)&pPresence);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Add the buddy
    IRTCBuddy * pBuddy = NULL;

    hr = pPresence->AddBuddy(
            bstrURI,
            bstrName,
            NULL,
            VARIANT_TRUE,
            NULL,
            0,
            &pBuddy);

    SAFE_RELEASE(pPresence);

    if (FAILED(hr))
    {
        // Addbuddy failed          
        SAFE_RELEASE(pBuddy);
        return hr;
    }

    // Update the buddy list entry
    UpdateBuddyList(pBuddy);
    SAFE_RELEASE(pBuddy);

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoRemoveBuddy
// 

HRESULT CRTCWin::DoRemoveBuddy(IRTCBuddy *pBuddy)
{
    HRESULT hr;

    // Get the RTC client presence interface
    IRTCClientPresence * pPresence = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientPresence,
            (void **)&pPresence);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Remove the buddy
    hr = pPresence->RemoveBuddy(pBuddy);

    SAFE_RELEASE(pPresence);

    if (FAILED(hr))
    {
        // RemoveBuddy failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoCall
// 

HRESULT CRTCWin::DoCall(RTC_SESSION_TYPE enType, BSTR bstrURI)
{
    HRESULT hr;

    if (enType != RTCST_IM)
    {
        // Is there already an AV session? We can only
        // allow one at a time
        if (CRTCAVSession::m_Singleton != NULL)
        {
            ShowMessageBox(L"An audio/video call is in progress!");

            return S_FALSE;
        }
    }

    // Create the session
    IRTCSession * pSession = NULL;

    hr = m_pClient->CreateSession(
        enType,
        NULL,
        NULL,
        0,
        &pSession
        );

    if (FAILED(hr))
    {
        // CreateSession failed
        return hr;
    }

    // Add the participant to the session
    hr = pSession->AddParticipant(
        bstrURI,
        NULL,
        NULL
        );

    if (FAILED(hr))
    {
        // AddParticipant failed
        SAFE_RELEASE(pSession);
        return hr;
    }

    // Add the session to the session list
    // This will create the session window
    hr = AddSession(pSession, enType);
    
    SAFE_RELEASE(pSession);

    if (FAILED(hr))
    {  
        // AddSession failed        
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoTuningWizard
// 

HRESULT CRTCWin::DoTuningWizard()
{
    HRESULT hr;

    // Is there already an AV session? We can only
    // allow one at a time
    if (CRTCAVSession::m_Singleton != NULL)
    {
        ShowMessageBox(L"An audio/video call is in progress!");

        return S_FALSE;
    }

    // Show the tuning wizard
    hr = m_pClient->InvokeTuningWizard((OAHWND)m_hWnd);

    if (FAILED(hr))
    {
        // InvokeTuningWizard failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::DoSetPresence
// 

HRESULT CRTCWin::DoSetPresence(RTC_PRESENCE_STATUS enStatus)
{
    HRESULT hr;
    UINT id = 0;

    // Check the appropriate menu item
    switch(enStatus)
    {
    case RTCXS_PRESENCE_OFFLINE:
        id = ID_FILE_PRESENCESTATUS_OFFLINE;
        break;

    case RTCXS_PRESENCE_ONLINE:
        id = ID_FILE_PRESENCESTATUS_ONLINE;
        break;

    case RTCXS_PRESENCE_AWAY:
        id = ID_FILE_PRESENCESTATUS_AWAY;
        break;

    case RTCXS_PRESENCE_IDLE:
        id = ID_FILE_PRESENCESTATUS_IDLE;
        break;

    case RTCXS_PRESENCE_BUSY:
        id = ID_FILE_PRESENCESTATUS_BUSY;
        break;

    case RTCXS_PRESENCE_BE_RIGHT_BACK:
        id = ID_FILE_PRESENCESTATUS_BERIGHTBACK;
        break;

    case RTCXS_PRESENCE_ON_THE_PHONE:
        id = ID_FILE_PRESENCESTATUS_ONTHEPHONE;
        break;

    case RTCXS_PRESENCE_OUT_TO_LUNCH:
        id = ID_FILE_PRESENCESTATUS_OUTTOLUNCH;
        break;
    }

    CheckMenuRadioItem(
        GetMenu(m_hWnd),   // handle to menu
        ID_FILE_PRESENCESTATUS_OFFLINE,  // identifier or position of first item
        ID_FILE_PRESENCESTATUS_OUTTOLUNCH,   // identifier or position of last item
        id,  // identifier or position of menu item
        MF_BYCOMMAND    // function options
        );

    // Get the RTC client presence interface
    IRTCClientPresence * pPresence = NULL;

    hr = m_pClient->QueryInterface(
            IID_IRTCClientPresence,
            (void **)&pPresence);

    if (FAILED(hr))
    {
        // QueryInterface failed
        return hr;
    }

    // Set the local presence status
    hr = pPresence->SetLocalPresenceInfo(enStatus, NULL);

    SAFE_RELEASE(pPresence);

    if (FAILED(hr))
    {
        // SetLocalPresenceInfo failed
        return hr;
    }

    return S_OK;
}

/////////////////////////////////////////////
//
// CRTCWin::WindowProc
// 

LRESULT CALLBACK CRTCWin::WindowProc(
  HWND hWnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
    CRTCWin * me = NULL;
    LRESULT  lr = 0;

    if ( uMsg == WM_CREATE )
    {
        // Create an instance of the class
        me = new CRTCWin;
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
        me = (CRTCWin *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        switch( uMsg )
        {        
        case WM_DESTROY:
            lr = me->OnDestroy(uMsg, wParam, lParam);

            // Delete the object instance
            delete me;

            // Quit the application
            PostQuitMessage(0);
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

        case WM_NOTIFY:
            lr = me->OnNotify(uMsg, wParam, lParam);
            break;

        case WM_RTC_EVENT:
            lr = me->OnRTCEvent(uMsg, wParam, lParam);
            break;
      
        default:
            lr = DefWindowProc( hWnd, uMsg, wParam, lParam );
        }
    }

    return lr;
}

/////////////////////////////////////////////
//
// CRTCWin::OnCreate
// 

LRESULT CRTCWin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

    // Create the buddy list
    m_hBuddyList = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"SysListView32",
        NULL,
        WS_CHILD | WS_VISIBLE |
        LVS_SINGLESEL | LVS_SORTASCENDING |
        LVS_AUTOARRANGE | LVS_SMALLICON,
        0, 0,
        0, 0,
        m_hWnd,
        (HMENU)IDC_BUDDYLIST,
        GetModuleHandle(NULL),
        NULL);

    if ( !m_hBuddyList )
    {
        // CreateWindowExW failed
        return -1;
    }

    // Create the RTC client
    HRESULT hr;

    hr = CoCreateInstance(
        CLSID_RTCClient,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IRTCClient,
        (LPVOID *)&m_pClient
        );

    if (FAILED(hr))
    {
        // CoCreateInstance failed
        return -1;
    }

    // Initialize the RTC client
    hr = m_pClient->Initialize();

    if (FAILED(hr))
    {
        // Initialize failed
        SAFE_RELEASE(m_pClient);
        return -1;
    }

    // Set the event filter for the RTC client
    hr = m_pClient->put_EventFilter( 
                 RTCEF_REGISTRATION_STATE_CHANGE |
                 RTCEF_SESSION_STATE_CHANGE |
                 RTCEF_PARTICIPANT_STATE_CHANGE |
                 RTCEF_MESSAGING |
                 RTCEF_MEDIA |
                 RTCEF_INTENSITY |
                 RTCEF_CLIENT |
                 RTCEF_BUDDY |
                 RTCEF_WATCHER
                 );

    if ( FAILED(hr) )
    {
        // put_EventFilter failed
        SAFE_RELEASE(m_pClient);
        return -1;
    }

    // Set the listen mode for RTC client
    // RTCLM_BOTH opens the standard SIP port 5060, as well as
    // a dynamic port.
    hr = m_pClient->put_ListenForIncomingSessions(RTCLM_BOTH); 

    if ( FAILED(hr) )
    {
        // put_ListenMode failed
        SAFE_RELEASE(m_pClient);
        return -1;
    }

    // Create the event sink object
    m_pEvents = new CRTCEvents;

    if (!m_pEvents)
    {
        // Out of memory
        SAFE_RELEASE(m_pClient);
        return -1;
    }

    // Advise for events from the RTC client
    hr = m_pEvents->Advise( m_pClient, m_hWnd );

    if ( FAILED(hr) )
    {
        // Advise failed
        SAFE_RELEASE(m_pClient);
        return -1;
    }

    // Initialize presence status
    DoSetPresence(RTCXS_PRESENCE_ONLINE);

    // Show the logon dialog
    PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_LOGON, 0), 0);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnDestroy
// 

LRESULT CRTCWin::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
    // Release the RTC profile
    SAFE_RELEASE(m_pProfile);

    if (m_pClient)
    {
        if (m_pEvents)
        {
            // Unadvise for events from the RTC client
            m_pEvents->Unadvise(m_pClient);
            m_pEvents = NULL;
        }

        // Shutdown the RTC client
        m_pClient->Shutdown();

        // Release the RTC client
        SAFE_RELEASE(m_pClient);
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnClose
// 

LRESULT CRTCWin::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr = S_OK;

    // Cleanup the existing session windows
    CleanupSessions();

    if (m_pClient)
    {
        // Prepare the RTC client object for shutdown
        hr = m_pClient->PrepareForShutdown();
    }

    if (!m_pClient || FAILED(hr))
    {
        // The RTC client object either doesn't exist, or
        // failed to prepare for shutdown. Destroy the
        // window now
        DestroyWindow(m_hWnd);
    }
    else
    {
        // The RTC client object is preparing to shutdown.
        // We should wait for the RTCCET_ASYNC_CLEANUP_DONE
        // event before we shutdown the RTC client. For now
        // just hide the window
        ShowWindow(m_hWnd, SW_HIDE);
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnSize
// 

LRESULT CRTCWin::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rcWnd, rcStatusBar, rcBuddyList;

    const int SIZE_EDGE = 5;

    GetClientRect(m_hWnd, &rcWnd);

    // Resize the status bar
    SendMessage(m_hStatusBar, uMsg, wParam, lParam);

    GetClientRect(m_hStatusBar, &rcStatusBar);

    // Resize the buddy list
    rcBuddyList.bottom = rcWnd.bottom -
        (rcStatusBar.bottom - rcStatusBar.top) -
        SIZE_EDGE;
    rcBuddyList.top = rcWnd.top + SIZE_EDGE;
    rcBuddyList.right = rcWnd.right - SIZE_EDGE;
    rcBuddyList.left = rcWnd.left + SIZE_EDGE;
    
    MoveWindow(
        m_hBuddyList,
        rcBuddyList.left,
        rcBuddyList.top,
        (rcBuddyList.right - rcBuddyList.left),
        (rcBuddyList.bottom - rcBuddyList.top),
        TRUE);

    SendMessage(m_hBuddyList, LVM_SETCOLUMNWIDTH,
        (WPARAM)0, MAKELPARAM(rcBuddyList.right - rcBuddyList.left,0));

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnCommand
// 

LRESULT CRTCWin::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr;

    switch(LOWORD(wParam))
    {
    case ID_FILE_LOGON:
        {
            BSTR bstrURI = NULL;
            BSTR bstrServer = NULL;
            BSTR bstrTransport = NULL;

            hr = ShowLogonDialog(
                m_hWnd, &bstrURI, &bstrServer, &bstrTransport);

            if (FAILED(hr))
            {
                // ShowLogonDialog failed
                break;
            }

            DoLogOn(bstrURI, bstrServer, bstrTransport);

            SAFE_FREE_STRING(bstrURI);
            SAFE_FREE_STRING(bstrServer);
            SAFE_FREE_STRING(bstrTransport);
        }
        break;

    case ID_FILE_LOGOFF:
        DoLogOff();
        break;

    case ID_FILE_EXIT:
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        break;

    case ID_ACTION_ADDBUDDY:
        {
            BSTR bstrURI = NULL;
            BSTR bstrName = NULL;

            // Show the add buddy dialog
            hr = ShowAddressDialog(m_hWnd, L"Add Buddy", TRUE, &bstrURI, &bstrName);

            if (FAILED(hr))
            {
                // ShowAddressDialog failed
                break;
            }
        
            // Add the buddy
            DoAddBuddy(bstrURI, bstrName);

            SAFE_FREE_STRING(bstrURI);
            SAFE_FREE_STRING(bstrName);
        }
        break;

    case ID_ACTION_CALL:
        {
            BSTR bstrURI = NULL;

            // Show the call dialog
            hr = ShowAddressDialog(m_hWnd, L"Call To", FALSE, &bstrURI, NULL);

            if (FAILED(hr))
            {
                // ShowAddressDialog failed
                break;
            }

            // Call the user
            DoCall(RTCST_PC_TO_PC, bstrURI);

            SAFE_FREE_STRING(bstrURI);
        }
        break;

    case ID_ACTION_MESSAGE:
        {
            BSTR bstrURI = NULL;

            // Show the call dialog
            hr = ShowAddressDialog(m_hWnd, L"Message To", FALSE, &bstrURI, NULL);

            if (FAILED(hr))
            {
                // ShowAddressDialog failed
                break;
            }

            // Send a message to the user
            DoCall(RTCST_IM, bstrURI);

            SAFE_FREE_STRING(bstrURI);
        }
        break;

    case ID_TOOLS_TUNINGWIZARD:
        DoTuningWizard();
        break;

    case ID_FILE_PRESENCESTATUS_OFFLINE:
        DoSetPresence(RTCXS_PRESENCE_OFFLINE);
        break;

    case ID_FILE_PRESENCESTATUS_ONLINE:
        DoSetPresence(RTCXS_PRESENCE_ONLINE);
        break;

    case ID_FILE_PRESENCESTATUS_AWAY:
        DoSetPresence(RTCXS_PRESENCE_AWAY);
        break;

    case ID_FILE_PRESENCESTATUS_IDLE:
        DoSetPresence(RTCXS_PRESENCE_IDLE);
        break;

    case ID_FILE_PRESENCESTATUS_BUSY:
        DoSetPresence(RTCXS_PRESENCE_BUSY);
        break;

    case ID_FILE_PRESENCESTATUS_BERIGHTBACK:
        DoSetPresence(RTCXS_PRESENCE_BE_RIGHT_BACK);
        break;

    case ID_FILE_PRESENCESTATUS_ONTHEPHONE:
        DoSetPresence(RTCXS_PRESENCE_ON_THE_PHONE);
        break;

    case ID_FILE_PRESENCESTATUS_OUTTOLUNCH:
        DoSetPresence(RTCXS_PRESENCE_OUT_TO_LUNCH);
        break;
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnNotify
// 

LRESULT CRTCWin::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
    int idCtrl = (int)wParam;
    LPNMHDR pnmh = (LPNMHDR)lParam;
    HRESULT hr;

    if ( (pnmh->code == NM_CLICK) ||
         (pnmh->code == NM_RCLICK) )
    {
        if ( idCtrl == IDC_BUDDYLIST )
        {
            // The user clicked on the buddy list
            LVHITTESTINFO lvht;
            ZeroMemory(&lvht, sizeof(LVHITTESTINFO));

            POINT pt;
            GetCursorPos(&pt);

            lvht.pt = pt;
            MapWindowPoints(NULL, pnmh->hwndFrom, &lvht.pt, 1 );
            
            int iRes = ListView_HitTest(pnmh->hwndFrom, &lvht);

            if (iRes == -1)
            {
                // ListView_HitTest failed
                return 0;
            }

            if (lvht.flags & LVHT_ONITEM)
            {
                // The user clicked on a buddy
                LVITEM lvi;
                ZeroMemory(&lvi, sizeof(LVITEM));
    
                lvi.mask = LVIF_PARAM;
                lvi.iItem = iRes;

                // Get the buddy item
                if (!ListView_GetItem(m_hBuddyList, &lvi))
                {
                    // ListView_GetItem failed;
                    return 0;
                }
                
                IRTCBuddy * pBuddy = (IRTCBuddy *)(lvi.lParam);
                BSTR bstrURI = NULL;

                // Get the buddy URI
                hr = pBuddy->get_PresentityURI(&bstrURI);

                if (FAILED(hr))
                {
                    // get_UserURI failed
                    return 0;
                }

                // Show the buddy menu
                HMENU hMenuRes = LoadMenu( 
                    GetModuleHandle(NULL),
                    MAKEINTRESOURCE(IDR_MENU_BUDDY));

                HMENU hMenu = GetSubMenu(hMenuRes, 0);

                UINT uID = TrackPopupMenu(hMenu,
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON |
                    TPM_NONOTIFY | TPM_RETURNCMD,
                    pt.x, pt.y, 0, m_hWnd, NULL);

                switch (uID)
                {
                case ID_BUDDY_CALL:
                    // Call the buddy
                    DoCall(RTCST_PC_TO_PC, bstrURI);
                    break;

                case ID_BUDDY_MESSAGE:
                    // Send a message to the buddy
                    DoCall(RTCST_IM, bstrURI);
                    break;

                case ID_BUDDY_REMOVE:
                    // Remove the buddy
                    DoRemoveBuddy(pBuddy);
                    
                    // Release the buddy reference
                    SAFE_RELEASE(pBuddy);

                    // Delete the buddy from the list
                    ListView_DeleteItem(m_hBuddyList, iRes);
                    break;
                }

                SAFE_FREE_STRING(bstrURI);
            }
        }
    }

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCEvent
// 

LRESULT CRTCWin::OnRTCEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IDispatch * pDisp = (IDispatch *)lParam;
    RTC_EVENT enEvent = (RTC_EVENT)wParam;
    HRESULT hr;

    // Based on the RTC_EVENT type, query for the 
    // appropriate event interface and call a helper
    // method to handle the event
    switch ( wParam )
    {
        case RTCE_REGISTRATION_STATE_CHANGE:
            {
                IRTCRegistrationStateChangeEvent * pEvent = NULL;
                
                hr = pDisp->QueryInterface( IID_IRTCRegistrationStateChangeEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCRegistrationStateChangeEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }              
            }
            break;

        case RTCE_SESSION_STATE_CHANGE:
            {
                IRTCSessionStateChangeEvent * pEvent = NULL;
                
                hr = pDisp->QueryInterface( IID_IRTCSessionStateChangeEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCSessionStateChangeEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }              
            }
            break;

        case RTCE_PARTICIPANT_STATE_CHANGE:
            {
                IRTCParticipantStateChangeEvent * pEvent = NULL;
                
                hr = pDisp->QueryInterface( IID_IRTCParticipantStateChangeEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCParticipantStateChangeEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }              
            }
            break;

        case RTCE_MESSAGING:
            {
                IRTCMessagingEvent * pEvent = NULL;
                
                hr = pDisp->QueryInterface( IID_IRTCMessagingEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCMessagingEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }              
            }
            break;

        case RTCE_MEDIA:
            {
                IRTCMediaEvent * pEvent = NULL;

                hr = pDisp->QueryInterface( IID_IRTCMediaEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCMediaEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }
            }
            break;

        case RTCE_INTENSITY:
            {
                IRTCIntensityEvent * pEvent = NULL;

                hr = pDisp->QueryInterface( IID_IRTCIntensityEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCIntensityEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }
            }
            break;

        case RTCE_CLIENT:
            {
                IRTCClientEvent * pEvent = NULL;

                hr = pDisp->QueryInterface( IID_IRTCClientEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCClientEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }
            }
            break;

        case RTCE_BUDDY:
            {
                IRTCBuddyEvent * pEvent = NULL;

                hr = pDisp->QueryInterface( IID_IRTCBuddyEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCBuddyEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }
            }
            break;

        case RTCE_WATCHER:
            {
                IRTCWatcherEvent * pEvent = NULL;

                hr = pDisp->QueryInterface( IID_IRTCWatcherEvent,
                                            (void **)&pEvent );

                if (SUCCEEDED(hr))
                {
                    OnRTCWatcherEvent(pEvent);
                    SAFE_RELEASE(pEvent);
                }
            }
            break;
    }

    // Release the event
    SAFE_RELEASE(pDisp);

    return 0;
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCRegistrationStateChangeEvent
// 

void CRTCWin::OnRTCRegistrationStateChangeEvent(
            IRTCRegistrationStateChangeEvent *pEvent
            )
{
    HRESULT hr;

    // Get the registration state
    RTC_REGISTRATION_STATE enState;
    long lStatusCode;

    hr = pEvent->get_State(&enState);

    if (FAILED(hr))
    {
        // get_State failed
        return;
    }

    // Get the status code
    hr = pEvent->get_StatusCode(&lStatusCode);

    if (FAILED(hr))
    {
        // get_StatusCode failed
        return;
    }

    switch(enState)
    {
    case RTCRS_UNREGISTERING:
        // Logoff in progress
        SetStatusText(L"Logging off");

        OnLoggingOff();
        break;

    case RTCRS_NOT_REGISTERED:
        // Logged off
        SetStatusText(L"Logged off");

        OnLoggedOff();
        break;

    case RTCRS_REGISTERING:
        // Logon in progress
        SetStatusText(L"Logging on");

        OnLoggingOn();
        break;

    case RTCRS_REGISTERED:
        // Logged on
        SetStatusText(L"Logged on");

        OnLoggedOn();
        break;

    case RTCRS_REJECTED:
    case RTCRS_ERROR:
        {
            // Logon failed. Most likely the server could no be found,
            // or the user needs to authenticate.
            SetStatusText(L"Logged off");

            HRESULT hr = lStatusCode;

            // Check if we need to authenticate
            if ((hr == RTC_E_STATUS_CLIENT_UNAUTHORIZED) ||
                (hr == RTC_E_STATUS_CLIENT_PROXY_AUTHENTICATION_REQUIRED))
            {
                BSTR bstrOldURI = NULL;
                BSTR bstrURI = NULL; 
                BSTR bstrAccount = NULL; 
                BSTR bstrPassword = NULL; 

                // Get the old user URI
                hr = GetUserURI(&bstrOldURI);

                if (FAILED(hr))
                {
                    // GetUserURI failed
                    break;
                }

                // Display the authentication dialog  
                hr = ShowAuthDialog(m_hWnd, bstrOldURI,
                    &bstrURI, &bstrAccount, &bstrPassword);

                SAFE_FREE_STRING(bstrOldURI);

                if (FAILED(hr))
                {
                    // ShowAuthDialog failed
                    break;
                }

                // Do the authentication
                hr = DoAuth(bstrURI, bstrAccount, bstrPassword);

                SAFE_FREE_STRING(bstrURI);
                SAFE_FREE_STRING(bstrAccount);
                SAFE_FREE_STRING(bstrPassword);
            }
           
            if (FAILED(hr))
            {
                DoLogOff();
                OnLoggedOff();

                // If we were logging on the show error
                if (m_enState == RTCRS_REGISTERING)
                {
                    ShowMessageBox(L"Logon failed!");
                }
            }
        }
        break;

    case RTCRS_LOGGED_OFF:
        // The user logged on at another client
        // The user is logged off from this client
        SetStatusText(L"Logged off");

        DoLogOff();
        OnLoggedOff();
        break;

    case RTCRS_LOCAL_PA_LOGGED_OFF:
        // The user logged on at another client
        // The user's presence state is no longer sent from this client

        SetStatusText(L"Logged on (Presence disabled)");
        break;

    case RTCRS_REMOTE_PA_LOGGED_OFF:
        // The user logged off on another client that was sending his
        // presence state. We can ignore this.
        break;
    }

    m_enState = enState;
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCSessionStateChangeEvent
// 

void CRTCWin::OnRTCSessionStateChangeEvent(
            IRTCSessionStateChangeEvent *pEvent
            )
{
    IRTCSession * pSession = NULL;
    CRTCSession * pSessWindow = NULL;
    RTC_SESSION_STATE enState;
    HRESULT hr;

    hr = pEvent->get_State(&enState);

    if (FAILED(hr))
    {
        // get_State failed
        return;
    }

    hr = pEvent->get_Session(&pSession);

    if (FAILED(hr))
    {
        // get_Session failed
        return;
    }

    // Is this session in our session list?
    hr = FindSession(pSession, &pSessWindow);    

    if (FAILED(hr))
    {
        // FindSession failed
        if (enState == RTCSS_INCOMING)
        {
            // This is a new session
            RTC_SESSION_TYPE enType;

            hr = pSession->get_Type(&enType);

            if (FAILED(hr))
            {
                // get_Type failed
                SAFE_RELEASE(pSession);
                return;
            }            

            if (enType == RTCST_IM)
            {
                // This is an IM call
                m_pClient->PlayRing(RTCRT_MESSAGE, VARIANT_TRUE);
            }
            else            
            {
                // This is an AV call
                if (CRTCAVSession::m_Singleton != NULL)
                {
                    // If another AV call is in progress, then
                    // we are already busy.
                    pSession->Terminate(RTCTR_BUSY);

                    SAFE_RELEASE(pSession);
                    return;
                }
                    
                // Get the participant object
                IRTCEnumParticipants * pEnum = NULL;
                IRTCParticipant * pParticipant = NULL;

                hr = pSession->EnumerateParticipants(&pEnum);

                if (FAILED(hr))
                {
                    // EnumerateParticipants failed
                    SAFE_RELEASE(pSession);
                    return;
                }

                hr = pEnum->Next(1, &pParticipant, NULL);

                SAFE_RELEASE(pEnum);

                if (hr != S_OK)
                {
                    // Next failed
                    SAFE_RELEASE(pSession);
                    return;
                }

                // Get the participant URI
                BSTR bstrURI = NULL;

                hr = pParticipant->get_UserURI(&bstrURI);

                if (FAILED(hr))
                {
                    // get_UserURI failed
                    SAFE_RELEASE(pSession);
                    SAFE_RELEASE(pParticipant);
                    return;
                }

                // Get the participant name
                BSTR bstrName = NULL;

                hr = pParticipant->get_Name(&bstrName);

                SAFE_RELEASE(pParticipant);

                if (FAILED(hr))
                {
                    // get_PresentityURI failed
                    SAFE_FREE_STRING(bstrURI);
                    SAFE_RELEASE(pSession);
                    return;
                }

                // Ring the bell
                m_pClient->PlayRing(RTCRT_PHONE, VARIANT_TRUE);

                // Show the session dialog
                BOOL fAccept;

                hr = ShowSessionDialog(m_hWnd, bstrName, bstrURI, &fAccept);    

                SAFE_FREE_STRING(bstrURI);
                SAFE_FREE_STRING(bstrName);

                if (FAILED(hr))
                {
                    // ShowSessionDialog failed                    
                    SAFE_RELEASE(pSession);
                    return;
                }

                if (fAccept)
                {
                    // Accept the session
                    hr = pSession->Answer();

                    if (FAILED(hr))
                    {
                        // Answer failed
                        SAFE_RELEASE(pSession);
                        return;
                    }
                }
                else
                {
                    // Reject the session
                    pSession->Terminate(RTCTR_REJECT);

                    SAFE_RELEASE(pSession);
                    return;
                }
            }  
            
            // Add the session to the session list
            // This will create the session window
            hr = AddSession(pSession, enType);

            if (FAILED(hr))
            {
                // AddSession failed
                SAFE_RELEASE(pSession);
                return;
            }
        }        

        SAFE_RELEASE(pSession);

        return;
    }    

    SAFE_RELEASE(pSession);

    // Deliver the session state to the session window
    pSessWindow->DeliverSessionState(enState);
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCParticipantStateChangeEvent
// 

void CRTCWin::OnRTCParticipantStateChangeEvent(
            IRTCParticipantStateChangeEvent *pEvent
            )
{
    IRTCSession * pSession = NULL;
    CRTCSession * pSessWindow = NULL;
    IRTCParticipant * pParticipant = NULL;
    RTC_PARTICIPANT_STATE enState;
    HRESULT hr;

    hr = pEvent->get_Participant(&pParticipant);

    if (FAILED(hr))
    {
        // get_Participant failed
        return;
    }

    hr = pParticipant->get_Session(&pSession);

    if (FAILED(hr))
    {
        // get_Session failed
        SAFE_RELEASE(pParticipant);
        return;
    }
        
    hr = FindSession(pSession, &pSessWindow);

    SAFE_RELEASE(pSession);

    if (FAILED(hr))
    {
        // FindSession failed
        SAFE_RELEASE(pParticipant);
        return;
    }

    // Get the participant state
    hr = pEvent->get_State(&enState);

    if (FAILED(hr))
    {
        // get_State failed
        SAFE_RELEASE(pParticipant);
        return;
    }

    // Deliver the participant state to the session window
    pSessWindow->DeliverParticipantState(pParticipant, enState);

    SAFE_RELEASE(pParticipant);
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCMessagingEvent
// 

void CRTCWin::OnRTCMessagingEvent(
            IRTCMessagingEvent *pEvent
            )
{
    IRTCSession * pSession = NULL;
    CRTCSession * pSessWindow = NULL;
    IRTCParticipant * pParticipant = NULL;
    RTC_MESSAGING_EVENT_TYPE enType;
    RTC_MESSAGING_USER_STATUS enStatus;
    BSTR bstrContentType = NULL;
    BSTR bstrMessage = NULL;
    HRESULT hr;

    hr = pEvent->get_Session(&pSession);

    if (FAILED(hr))
    {
        // get_Session failed
        return;
    }
    
    hr = FindSession(pSession, &pSessWindow);

    SAFE_RELEASE(pSession);

    if (FAILED(hr))
    {
        // FindSession failed
        return;
    }

    hr = pEvent->get_EventType(&enType);

    if (FAILED(hr))
    {
        // get_EventType failed
        return;
    }

    hr = pEvent->get_Participant(&pParticipant);

    if (FAILED(hr))
    {
        // get_Participant failed
        return;
    }

    if (enType == RTCMSET_MESSAGE)
    {
        hr = pEvent->get_MessageHeader(&bstrContentType);

        if (FAILED(hr))
        {
            // get_MessageHeader failed
            SAFE_RELEASE(pParticipant);
            return;
        }

        hr = pEvent->get_Message(&bstrMessage);

        if (FAILED(hr))
        {
            // get_Message failed
            SAFE_RELEASE(pParticipant);
            SAFE_FREE_STRING(bstrContentType);
            return;
        }

        // Deliver the message to the session window
        pSessWindow->DeliverMessage(pParticipant, bstrContentType, bstrMessage);

        SAFE_FREE_STRING(bstrContentType);
        SAFE_FREE_STRING(bstrMessage);
    }
    else if (enType == RTCMSET_STATUS)
    {
        hr = pEvent->get_UserStatus(&enStatus);

        if (FAILED(hr))
        {
            // get_UserStatus failed
            return;
        }

        // Deliver the user status to the session window
        pSessWindow->DeliverUserStatus(pParticipant, enStatus);
    }

    SAFE_RELEASE(pParticipant);    
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCMediaEvent
// 

void CRTCWin::OnRTCMediaEvent(
            IRTCMediaEvent *pEvent
            )
{
    long lMediaType;
    RTC_MEDIA_EVENT_TYPE enType;
    RTC_MEDIA_EVENT_REASON enReason;
    HRESULT hr;

    hr = pEvent->get_MediaType(&lMediaType);

    if (FAILED(hr))
    {
        // get_MediaType failed
        return;
    }

    hr = pEvent->get_EventType(&enType);

    if (FAILED(hr))
    {
        // get_EventType failed
        return;
    }

    hr = pEvent->get_EventReason(&enReason);

    if (FAILED(hr))
    {
        // get_EventReason failed
        return;
    }

    if (CRTCAVSession::m_Singleton != NULL)
    {
        // Deliver the media state to the session window
        (CRTCAVSession::m_Singleton)->DeliverMedia(lMediaType, enType, enReason);
    }
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCIntensityEvent
// 

void CRTCWin::OnRTCIntensityEvent(
            IRTCIntensityEvent *pEvent
            )
{
    RTC_AUDIO_DEVICE enDevice;
    long lLevel, lMin, lMax;
    HRESULT hr;

    hr = pEvent->get_Direction(&enDevice);

    if (FAILED(hr))
    {
        // get_Direction failed
        return;
    }

    hr = pEvent->get_Level(&lLevel);

    if (FAILED(hr))
    {
        // get_Level failed
        return;
    }

    hr = pEvent->get_Min(&lMin);

    if (FAILED(hr))
    {
        // get_Min failed
        return;
    }

    hr = pEvent->get_Max(&lMax);

    if (FAILED(hr))
    {
        // get_Max failed
        return;
    }

    // Normalize level to between zero and 100
    if ((lMax - lMin) == 0)
    {
        lLevel = 0;
    }
    else
    {
        lLevel = (lLevel - lMin) * 100 / (lMax - lMin);
    }

    if (CRTCAVSession::m_Singleton != NULL)
    {
        // Deliver the intensity state to the session window
        (CRTCAVSession::m_Singleton)->DeliverIntensity(enDevice, lLevel);
    }
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCClientEvent
// 

void CRTCWin::OnRTCClientEvent(
            IRTCClientEvent *pEvent
            )
{
    HRESULT hr;

    // Get the client event tyoe
    RTC_CLIENT_EVENT_TYPE enEventType;

    hr = pEvent->get_EventType(&enEventType);

    if (FAILED(hr))
    {
        // get_EventType failed
        return;
    }

    if ( enEventType == RTCCET_ASYNC_CLEANUP_DONE )
    {
        // The RTC client has finished preparing for
        // shutdown. Destroy the window now.
        DestroyWindow(m_hWnd);
    }
    else
    {
        if (CRTCAVSession::m_Singleton != NULL)
        {
            // Deliver the client state to the session window
            (CRTCAVSession::m_Singleton)->DeliverClient(enEventType);
        }
    }
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCBuddyEvent
// 

void CRTCWin::OnRTCBuddyEvent(
            IRTCBuddyEvent *pEvent
            )
{
    HRESULT hr;

    // Get the buddy object
    IRTCBuddy * pBuddy = NULL;

    hr = pEvent->get_Buddy(&pBuddy);

    if (FAILED(hr))
    {
        // get_Buddy failed
        return;
    }

    // Update the buddy list entry
    hr = UpdateBuddyList(pBuddy);

    SAFE_RELEASE(pBuddy);

    if (FAILED(hr))
    {
        // UpdateBuddyState failed
        return;
    }
}

/////////////////////////////////////////////
//
// CRTCWin::OnRTCWatcherEvent
// 

void CRTCWin::OnRTCWatcherEvent(
            IRTCWatcherEvent *pEvent
            )
{
    HRESULT hr;

    // Get the watcher object
    IRTCWatcher * pWatcher = NULL;

    hr = pEvent->get_Watcher(&pWatcher);

    if (FAILED(hr))
    {
        // get_Watcher failed
        return;
    }

    // Get the watcher URI
    BSTR bstrURI = NULL;

    hr = pWatcher->get_PresentityURI(&bstrURI);

    if (FAILED(hr))
    {
        // get_PresentityURI failed
        SAFE_RELEASE(pWatcher);
        return;
    }

    // Get the watcher name
    BSTR bstrName = NULL;

    hr = pWatcher->get_Name(&bstrName);

    if (FAILED(hr))
    {
        // get_PresentityURI failed
        SAFE_FREE_STRING(bstrURI);
        SAFE_RELEASE(pWatcher);
        return;
    }

    // Show the incoming watcher dialog
    BOOL fAllow, fAddBuddy;

    hr = ShowWatcherDialog(m_hWnd, bstrName, bstrURI, &fAllow, &fAddBuddy);    

    if (FAILED(hr))
    {
        // ShowWatcherDialog failed
        SAFE_FREE_STRING(bstrURI);
        SAFE_FREE_STRING(bstrName);
        SAFE_RELEASE(pWatcher);
        return;
    }

    // Set the watcher to be allowed or blocked
    hr = pWatcher->put_State(fAllow ? RTCWS_ALLOWED : RTCWS_BLOCKED);

    SAFE_RELEASE(pWatcher);

    if (FAILED(hr))
    {
        // put_State failed
        SAFE_FREE_STRING(bstrURI);
        SAFE_FREE_STRING(bstrName);
        return;
    }

    // Shall we add the user as a buddy?
    if (fAddBuddy)
    {
        hr = DoAddBuddy(bstrURI, bstrName);

        if (FAILED(hr))
        {
            // DoAddBuddy failed
            SAFE_FREE_STRING(bstrURI);
            SAFE_FREE_STRING(bstrName);
            return;
        }
    }

    SAFE_FREE_STRING(bstrURI);
    SAFE_FREE_STRING(bstrName);
}