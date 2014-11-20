// rtcdialogs.h
//

#ifndef _RTCDIALOGS_H_
#define _RTCDIALOGS_H_

// Show the logon dialog
HRESULT ShowLogonDialog(HWND hWndParent, 
                        BSTR *pbstrURI, BSTR *pbstrServer, BSTR *pbstrTransport);

// Show the authentication dialog
HRESULT ShowAuthDialog(HWND hWndParent, BSTR bstrOldURI,
                       BSTR *pbstrURI, BSTR *pbstrAccount, BSTR *pbstrPassword);

// Show the incoming watcher dialog
HRESULT ShowWatcherDialog(HWND hWndParent,
                          BSTR bstrName, BSTR bstrURI,
                          BOOL *pfAllow, BOOL *pfAddBuddy);

// Show the address dialog (outgoing session/buddy)
HRESULT ShowAddressDialog(HWND hWndParent,
                          PWSTR szTitle, BOOL fGetName,
                          BSTR *pbstrURI, BSTR *pbstrName);

// Show the incoming session dialog
HRESULT ShowSessionDialog(HWND hwndParent,
                          BSTR bstrName, BSTR bstrURI,
                          BOOL *pfAccept);

#endif //_RTCDIALOGS_H_