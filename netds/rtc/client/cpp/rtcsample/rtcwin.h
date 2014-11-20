// rtcwin.h
//

#ifndef _RTCWIN_H_
#define _RTCWIN_H_

// RTC events will be posted to the message queue with
// this user defined window message
#define WM_RTC_EVENT      WM_USER+100

class CRTCEvents;
class CRTCSession;

// Class for the application window
class CRTCWin
{
public:
    CRTCWin();
    ~CRTCWin();

    // Registers the window class
    static HRESULT RegisterClass();    

    // Window procedure
    static LRESULT CALLBACK WindowProc(
        HWND hWnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
        );

    // Remove a session from the session list
    HRESULT RemoveSession(
        CRTCSession * pSessWindow
        );    

    // Get the local user URI
    HRESULT GetUserURI(BSTR *pbstrURI);    

private:

    HWND    m_hWnd;
    HWND    m_hStatusBar;
    HWND    m_hBuddyList;

    IRTCClient * m_pClient;
    IRTCProfile * m_pProfile;
    CRTCEvents * m_pEvents; 
    RTC_REGISTRATION_STATE m_enState;
    BOOL m_fPresenceEnabled;

    std::vector<CRTCSession *> m_SessionList;

    // Helpers
    void ShowMessageBox(PWSTR szText);
    void SetStatusText(PWSTR szText);

    // Add a session to the session list
    HRESULT AddSession(
        IRTCSession * pSession,
        RTC_SESSION_TYPE enType
        );

    // Find a session in the session list
    HRESULT FindSession(
        IRTCSession * pSession,
        CRTCSession ** ppSessWindow
        );

    // Cleanup the session list
    HRESULT CleanupSessions();

    // Update a buddy entry in the buddy list
    HRESULT UpdateBuddyList(IRTCBuddy * pBuddy);

    // Clear the buddy list
    HRESULT ClearBuddyList();

    // Logon
    HRESULT DoLogOn(BSTR bstrURI, BSTR bstrServer, BSTR bstrTransport);
    HRESULT OnLoggingOn();
    HRESULT OnLoggedOn();

    // Logoff
    HRESULT DoLogOff();  
    HRESULT OnLoggingOff();
    HRESULT OnLoggedOff();

    // Authentication
    HRESULT DoAuth(BSTR bstrURI, BSTR bstrAccount, BSTR bstrPassword);

    // Enable presence
    HRESULT DoEnablePresence(BOOL fEnable);

    // Generate provisioning XML profile
    HRESULT DoGenerateProvisioning(BSTR bstrURI, BSTR bstrServer,
                                   BSTR bstrTransport, BSTR *pbstrXML);

    // Add a buddy
    HRESULT DoAddBuddy(BSTR bstrURI, BSTR bstrName);

    // Remove a buddy
    HRESULT DoRemoveBuddy(IRTCBuddy *pBuddy);

    // Place a call (AV or IM)
    HRESULT DoCall(RTC_SESSION_TYPE enType, BSTR bstrURI);

    // Start the tuning wizard
    HRESULT DoTuningWizard();

    // Set local presence status
    HRESULT DoSetPresence(RTC_PRESENCE_STATUS enStatus);    

    // Window message handlers
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnRTCEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // RTC event handlers
    void OnRTCRegistrationStateChangeEvent(
                IRTCRegistrationStateChangeEvent *pEvent
                );

    void OnRTCSessionStateChangeEvent(
                IRTCSessionStateChangeEvent *pEvent
                );

    void OnRTCParticipantStateChangeEvent(
                IRTCParticipantStateChangeEvent *pEvent
                );

    void OnRTCMessagingEvent(
                IRTCMessagingEvent *pEvent
                );

    void OnRTCMediaEvent(
                IRTCMediaEvent *pEvent
                );

    void OnRTCIntensityEvent(
                IRTCIntensityEvent *pEvent
                );

    void OnRTCClientEvent(
                IRTCClientEvent *pEvent
                );

    void OnRTCBuddyEvent(
                IRTCBuddyEvent *pEvent
                );

    void OnRTCWatcherEvent(
                IRTCWatcherEvent *pEvent
                );
};

#endif //_RTCWIN_H_