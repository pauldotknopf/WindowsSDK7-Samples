// rtcdialogs.cpp
//

#include "stdafx.h"

/////////////////////////////////////////////
//
// CleanUserURI
//
// Assumes buffer length to be MAX_STRING
//

void CleanUserURI(WCHAR * szURI)
{
    // If the user URI does not start with sip: then
    // add it
    if (wcsncmp(szURI, L"sip:", 4))
    {
        WCHAR szTemp[MAX_STRING];

        wcscpy_s(szTemp, _countof(szTemp), szURI);
        _snwprintf_s(szURI, MAX_STRING, _TRUNCATE, L"sip:%s", szTemp);

    }

    // Also, strip off any parameters to the user URI
    WCHAR *ch = wcschr(szURI, L';');

    if (ch != NULL)
    {
        *ch = L'\0';
    }
}

/////////////////////////////////////////////
//
// LOGON_DATA
//

struct LOGON_DATA
{
    WCHAR szUserURI[MAX_STRING];
    WCHAR szServer[MAX_STRING];
    WCHAR szTransport[MAX_STRING];
};

/////////////////////////////////////////////
//
// LogonDialogProc
// 

INT_PTR CALLBACK LogonDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
    LOGON_DATA *pLD;
    pLD = (LOGON_DATA *)GetWindowLongPtr(hwndDlg, DWLP_USER);

    switch (uMsg) 
    { 
    case WM_INITDIALOG:
        {
            // Store the LOGON_DATA pointer in the
            // window's user data
            SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);

            // Populate the transport combo box
            HWND hwndCombo = GetDlgItem(hwndDlg, IDC_COMBO_LOGON_TRANSPORT);
        
            SendMessageW(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"TCP");
            SendMessageW(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"UDP");
            SendMessageW(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"TLS");
            SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);
        }
        return TRUE;

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDOK: 
            {
                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_LOGON_USERURI, 
                    pLD->szUserURI,
                    MAX_STRING
                    );

                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_LOGON_SERVER, 
                    pLD->szServer,
                    MAX_STRING
                    );

                GetDlgItemTextW(
                    hwndDlg,
                    IDC_COMBO_LOGON_TRANSPORT, 
                    pLD->szTransport,
                    MAX_STRING
                    );

                // Fall through
            }

        case IDCANCEL: 
            EndDialog(hwndDlg, wParam); 
            return TRUE; 
        } 
    } 

    return FALSE; 
}

/////////////////////////////////////////////
//
// ShowLogonDialog
//

HRESULT ShowLogonDialog(HWND hWndParent,
                        BSTR *pbstrURI, BSTR *pbstrServer, BSTR *pbstrTransport)
{
    int iRes;
    LOGON_DATA ld;
    
    // Show the dialog
    iRes = DialogBoxParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_LOGON),
        hWndParent,
        (DLGPROC)LogonDialogProc,
        (LPARAM)&ld
        );

    if (iRes == IDOK)
    {
        // Check for empty strings
        if ((ld.szUserURI[0] == L'\0') ||
            (ld.szServer[0] == L'\0') ||
            (ld.szTransport[0] == L'\0'))
        {
            return E_FAIL;
        }

        // Cleanup the user URI
        CleanUserURI(ld.szUserURI);

        *pbstrURI = SysAllocString(ld.szUserURI);

        if (!(*pbstrURI))
        {
            // Out of memory
            return E_OUTOFMEMORY;
        }

        *pbstrServer = SysAllocString(ld.szServer);

        if (!(*pbstrServer))
        {
            // Out of memory
            SAFE_FREE_STRING(*pbstrURI);
            return E_OUTOFMEMORY;
        }

        *pbstrTransport = SysAllocString(ld.szTransport);

        if (!(*pbstrTransport))
        {
            // Out of memory
            SAFE_FREE_STRING(*pbstrURI);
            SAFE_FREE_STRING(*pbstrServer);
            return E_OUTOFMEMORY;
        }

        return S_OK;
    }

    return E_FAIL;
}

/////////////////////////////////////////////
//
// AUTH_DATA
//

struct AUTH_DATA
{
    BSTR bstrOldURI;
    WCHAR szUserURI[MAX_STRING];
    WCHAR szAccount[MAX_STRING];
    WCHAR szPassword[MAX_STRING];
};

/////////////////////////////////////////////
//
// AuthDialogProc
// 

INT_PTR CALLBACK AuthDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
    AUTH_DATA *pAD;
    pAD = (AUTH_DATA *)GetWindowLongPtr(hwndDlg, DWLP_USER);

    switch (uMsg) 
    { 
    case WM_INITDIALOG:
        {
            // Store the AUTH_DATA pointer in the
            // window's user data
            SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);

            pAD = (AUTH_DATA *)lParam;

            // Populate the user URI edit box
            SetDlgItemTextW(hwndDlg, IDC_EDIT_AUTH_USERURI,
                            pAD->bstrOldURI);
        }
        return TRUE;

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDOK: 
            {
                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_AUTH_USERURI, 
                    pAD->szUserURI,
                    MAX_STRING
                    );

                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_AUTH_ACCOUNT, 
                    pAD->szAccount,
                    MAX_STRING
                    );

                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_AUTH_PASSWORD, 
                    pAD->szPassword,
                    MAX_STRING
                    );

                // Fall through
            }

        case IDCANCEL: 
            EndDialog(hwndDlg, wParam); 
            return TRUE; 
        } 
    } 

    return FALSE; 
}

/////////////////////////////////////////////
//
// ShowAuthDialog
//

HRESULT ShowAuthDialog(HWND hWndParent, BSTR bstrOldURI,
                       BSTR *pbstrURI, BSTR *pbstrAccount, BSTR *pbstrPassword)
{
    int iRes;
    AUTH_DATA ad;

    ad.bstrOldURI = bstrOldURI;
    
    // Show the dialog
    iRes = DialogBoxParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_AUTH),
        hWndParent,
        (DLGPROC)AuthDialogProc,
        (LPARAM)&ad
        );

    if (iRes == IDOK)
    {
        // Check for empty strings
        if ((ad.szUserURI[0] == L'\0') ||
            (ad.szAccount[0] == L'\0') ||
            (ad.szPassword[0] == L'\0'))
        {
            return E_FAIL;
        }

        // Cleanup the user URI
        CleanUserURI(ad.szUserURI);

        *pbstrURI = SysAllocString(ad.szUserURI);

        if (!(*pbstrURI))
        {
            // Out of memory
            return E_OUTOFMEMORY;
        }

        *pbstrAccount = SysAllocString(ad.szAccount);

        if (!(*pbstrAccount))
        {
            // Out of memory
            SAFE_FREE_STRING(*pbstrURI);
            return E_OUTOFMEMORY;
        }

        *pbstrPassword = SysAllocString(ad.szPassword);

        if (!(*pbstrPassword))
        {
            // Out of memory
            SAFE_FREE_STRING(*pbstrURI);
            SAFE_FREE_STRING(*pbstrAccount);
            return E_OUTOFMEMORY;
        }

        return S_OK;
    }

    return E_FAIL;
}

/////////////////////////////////////////////
//
// WATCHER_DATA
// 

struct WATCHER_DATA
{
    BSTR bstrName;
    BSTR bstrURI;
    BOOL fAllow;
    BOOL fAddBuddy;
};

/////////////////////////////////////////////
//
// WatcherDialogProc
// 

INT_PTR CALLBACK WatcherDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
    WATCHER_DATA *pWD;
    pWD = (WATCHER_DATA *)GetWindowLongPtr(hwndDlg, DWLP_USER);

    switch (uMsg) 
    { 
    case WM_INITDIALOG:
        {
            // Store the WATCHER_DATA pointer in the
            // window's user data
            SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
            pWD = (WATCHER_DATA *)lParam;

            // Populate the watcher text
            SendDlgItemMessageW(hwndDlg, IDC_STATIC_WATCHER_NAME,
                WM_SETTEXT, 0, (LPARAM)(pWD->bstrName));

            SendDlgItemMessageW(hwndDlg, IDC_STATIC_WATCHER_URI,
                WM_SETTEXT, 0, (LPARAM)(pWD->bstrURI));

            // Check the add buddy box by default
            SendDlgItemMessage(hwndDlg, IDC_CHECK_WATCHER_ADD_BUDDY,
                                BM_SETCHECK, BST_CHECKED, 0);
        }
        return TRUE;

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDC_BUTTON_WATCHER_ALLOW: 
            {
                pWD->fAddBuddy = (SendDlgItemMessage( 
                                hwndDlg, 
                                IDC_CHECK_WATCHER_ADD_BUDDY,
                                BM_GETCHECK, 
                                0, 0) == BST_CHECKED);

                pWD->fAllow = TRUE;

                EndDialog(hwndDlg, IDOK); 
            }
            return TRUE;

        case IDC_BUTTON_WATCHER_BLOCK:
            {
                pWD->fAddBuddy = (SendDlgItemMessage( 
                                hwndDlg, 
                                IDC_CHECK_WATCHER_ADD_BUDDY,
                                BM_GETSTATE, 
                                0, 0) == BST_CHECKED);

                pWD->fAllow = FALSE;

                EndDialog(hwndDlg, IDOK); 
            }
            return TRUE;

        case IDCANCEL: 
            EndDialog(hwndDlg, wParam); 
            return TRUE; 
        } 
    } 

    return FALSE; 
}

/////////////////////////////////////////////
//
// ShowWatcherDialog
//

HRESULT ShowWatcherDialog(
        HWND hWndParent,
        BSTR bstrName, BSTR bstrURI,
        BOOL *pfAllow, BOOL *pfAddBuddy)
{
    int iRes;
    WATCHER_DATA wd;
    
    wd.bstrName = bstrName;
    wd.bstrURI = bstrURI;

    // Show the dialog
    iRes = DialogBoxParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_WATCHER),
        hWndParent,
        (DLGPROC)WatcherDialogProc,
        (LPARAM)&wd
        );

    if (iRes == IDOK)
    {
        *pfAllow = wd.fAllow;
        *pfAddBuddy = wd.fAddBuddy;

        return S_OK;
    }

    return E_FAIL;
}

/////////////////////////////////////////////
//
// ADDRESS_DATA
// 

struct ADDRESS_DATA
{
    PWSTR szTitle;
    WCHAR szURI[MAX_STRING];
    WCHAR szName[MAX_STRING];
};

/////////////////////////////////////////////
//
// AddressDialogProc
// 

INT_PTR CALLBACK AddressDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
    ADDRESS_DATA *pAD;
    pAD = (ADDRESS_DATA *)GetWindowLongPtr(hwndDlg, DWLP_USER);
                
    switch (uMsg) 
    { 
    case WM_INITDIALOG:
        {
            // Store the ADDRESS_DATA pointer in the
            // window's user data
            SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
            pAD = (ADDRESS_DATA *)lParam;

            // Set the dialog title
            SetWindowText(hwndDlg, pAD->szTitle);
        }
        return TRUE;

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDOK: 
            {
                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_ADDRESS_URI, 
                    pAD->szURI,
                    MAX_STRING
                    );

                GetDlgItemTextW(
                    hwndDlg,
                    IDC_EDIT_ADDRESS_NAME, 
                    pAD->szName,
                    MAX_STRING
                    );

                // Fall through
            }

        case IDCANCEL: 
            EndDialog(hwndDlg, wParam); 
            return TRUE; 
        } 
    } 

    return FALSE; 
}

/////////////////////////////////////////////
//
// ShowAddressDialog
//

HRESULT ShowAddressDialog(HWND hWndParent,
                          PWSTR szTitle, BOOL fGetName,
                          BSTR *pbstrURI, BSTR *pbstrName)
{
    int iRes;
    ADDRESS_DATA ad;
    
    ad.szTitle = szTitle;

    // Show the dialog
    iRes = DialogBoxParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(fGetName ? IDD_DIALOG_ADDRESS_NAME : IDD_DIALOG_ADDRESS),
        hWndParent,
        (DLGPROC)AddressDialogProc,
        (LPARAM)&ad
        );

    if (iRes == IDOK)
    {
        // Check for empty strings
        if (ad.szURI[0] == L'\0')
        {
            return E_FAIL;
        }

        *pbstrURI = SysAllocString(ad.szURI);

        if (!(*pbstrURI))
        {
            // Out of memory
            return E_OUTOFMEMORY;
        }

        if (fGetName)
        {
            *pbstrName = SysAllocString(ad.szName);

            if (!(*pbstrName))
            {
                // Out of memory
                SAFE_FREE_STRING(*pbstrURI);

                return E_OUTOFMEMORY;
            }
        }

        return S_OK;
    }

    return E_FAIL;
}

/////////////////////////////////////////////
//
// SESSION_DATA
// 

struct SESSION_DATA
{
    BSTR bstrName;
    BSTR bstrURI;
    BOOL fAccept;
};

/////////////////////////////////////////////
//
// SessionDialogProc
// 

INT_PTR CALLBACK SessionDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
    SESSION_DATA *pSD;
    pSD = (SESSION_DATA *)GetWindowLongPtr(hwndDlg, DWLP_USER);

    switch (uMsg) 
    { 
    case WM_INITDIALOG:
        {
            // Store the SESSION_DATA pointer in the
            // window's user data
            SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
            pSD = (SESSION_DATA *)lParam;

            // Populate the session text
            SendDlgItemMessageW(hwndDlg, IDC_STATIC_SESSION_NAME,
                WM_SETTEXT, 0, (LPARAM)(pSD->bstrName));

            SendDlgItemMessageW(hwndDlg, IDC_STATIC_SESSION_URI,
                WM_SETTEXT, 0, (LPARAM)(pSD->bstrURI));
        }
        return TRUE;

    case WM_COMMAND: 
        switch (LOWORD(wParam)) 
        { 
        case IDC_BUTTON_SESSION_ACCEPT: 
            {
                pSD->fAccept = TRUE;

                EndDialog(hwndDlg, IDOK); 
            }
            return TRUE;

        case IDC_BUTTON_SESSION_REJECT:
        case IDCANCEL: 
            {
                pSD->fAccept = FALSE;

                EndDialog(hwndDlg, IDOK); 
            }
            return TRUE; 
        } 
    } 

    return FALSE; 
}

/////////////////////////////////////////////
//
// ShowSessionDialog
//

HRESULT ShowSessionDialog(
        HWND hWndParent,
        BSTR bstrName, BSTR bstrURI,
        BOOL *pfAccept)
{
    int iRes;
    SESSION_DATA sd;
    
    sd.bstrName = bstrName;
    sd.bstrURI = bstrURI;

    // Show the dialog
    iRes = DialogBoxParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_SESSION),
        hWndParent,
        (DLGPROC)SessionDialogProc,
        (LPARAM)&sd
        );

    if (iRes == IDOK)
    {
        *pfAccept = sd.fAccept;

        return S_OK;
    }

    return E_FAIL;
}