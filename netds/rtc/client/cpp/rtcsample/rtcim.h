// rtcimsession.h
//

#ifndef _RTCIMSESSION_H_
#define _RTCIMSESSION_H_

// Class for instant messaging session windows
class CRTCIMSession : public CRTCSession
{
public:
    CRTCIMSession();
    ~CRTCIMSession();

    // Registers the window class
    static HRESULT RegisterClass();

    // Window procedure
    static LRESULT CALLBACK WindowProc(
        HWND hWnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
        );

    // Subclass procedure for the edit control
    static LRESULT CALLBACK EditSubclassProc(
        HWND hWnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
        );

protected:
    
    HWND m_hDisplay;
    HWND m_hEdit;
    HWND m_hSend;
    BOOL m_fTyping;
    WNDPROC m_wpOrigEditProc;

    // Send an instant message
    HRESULT DoSendMessage();

    // Display an instant message
    HRESULT DoDisplayMessage(PWSTR szUser, PWSTR szMessage);

    // Send typing status
    HRESULT DoSendTyping();

    // Display typing status
    HRESULT DoDisplayTyping(PWSTR szUser, BOOL fTyping);

    // Window message handlers
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);   

    // Event handlers
    virtual HRESULT DeliverMessage(IRTCParticipant * pParticipant, BSTR bstrContentType, BSTR bstrMessage);
    virtual HRESULT DeliverUserStatus(IRTCParticipant * pParticipant, RTC_MESSAGING_USER_STATUS enStatus);
};

#endif //_RTCIMSESSION_H_