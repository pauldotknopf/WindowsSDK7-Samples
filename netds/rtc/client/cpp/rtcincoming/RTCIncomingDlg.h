
/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 *
 * @Abstract: RTCIncomingDlg.h
 * ---------- [ Header Definition File]
 * 
 *
 * @Description :
 * -------------
 * Header file for the Main RTCIncoming Dialog Box
 * this Class will contain the state machine transitions
 * of the RTC Core Sample, answering calls, receiving events
 * disconnected calls, displaying video and auto answer
 * 
 *
 ******************************************************/


#ifndef ___RTCINCOMINGDLG_H____
#define ___RTCINCOMINGDLG_H____



#if _MSC_VER > 1000
#pragma once
#endif 

#define WM_RTCCORE_EVENT    WM_USER+501

#define RECVVID_LEFT        168
#define RECVVID_TOP         115
#define RECVVID_RIGHT       10
#define RECVVID_BOTTOM      18

#define PREVVID_LEFT        25
#define PREVVID_TOP         115
#define PREVVID_RIGHT       10
#define PREVVID_BOTTOM      18

#define MAX_RTCTIMEOUT      5000


/**********************************************************
//@Class CRTCIncomingDlg Box
//
//@Description
//  
//  Contains RTC State Machine and implements
//  IRTCEventNotification and its Callback
//
//
//*********************************************************/

class CRTCIncomingDlg : public CDialog,
						public IRTCEventNotification
{
// Construction
public:

	CRTCIncomingDlg(CWnd* pParent = NULL);	// standard constructor


	//
	// Dialog Box Data
	//

	//{{AFX_DATA(CRTCIncomingDlg)
	enum { IDD = IDD_RTCINCOMING_DIALOG };
	CButton	m_chkAutoAnswer;
	CEdit	m_ctrlEditStatus;
	CButton	m_btnDisconnect;
	CButton	m_btnAnswer;
	//}}AFX_DATA

	
	//{{AFX_VIRTUAL(CRTCIncomingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


	//
	// UI Dialog Helpers for Modifying the State of the Buttons
	//

	void StateAnswerIDC(VARIANT_BOOL fEnable);
	void StateDisconnectIDC(VARIANT_BOOL fEnable);


	// 
	// Initialization of the RTC state machine and shutdown
	//

	HRESULT InitializeRTCApp();
	HRESULT ShutdownRTCApp();


	//
	// Event Source\Callback  Method for Receiving RTC Events
	// and posting Messages to Main thread RTCEventThread
	//

	HRESULT STDMETHODCALLTYPE Event(
			RTC_EVENT RTCEvent,
            IDispatch * pEvent
            );

	
	//
	// IUnknown Implementations from IRTCEventNotification Interface
	// QI, Addref and Release
	// 
	
	HRESULT	STDMETHODCALLTYPE QueryInterface(REFIID iid, 
											 void **ppvObject);

	
	ULONG	STDMETHODCALLTYPE AddRef(void);
	ULONG	STDMETHODCALLTYPE Release(void);

	//
	// RTC session state transition enums
	//

	typedef enum tagSESSION_OPERATION 
	{

		STO_ANSWER,
		STO_TERMINATE

	} SESSION_OPERATION;
	

	//
	// RTC State Transition Helpers
	//
	//

	HRESULT StateCoCreateClient();
	HRESULT StateInitClient();
	HRESULT StateSinkEvent(LONG lEventMask);
	HRESULT StateUnSinkEvent();
	HRESULT StateSessionControl(SESSION_OPERATION soType);
	HRESULT StateQueryActiveMedia(LONG *plActiveMedia);
	HRESULT StateListenMode(RTC_LISTEN_MODE lMode);
	HRESULT AddToMediaCapabilities(LONG lMedia, VARIANT_BOOL fPersist);
	HRESULT ReleaseStateMachine();

	HRESULT SetAutoAnswerMode(VARIANT_BOOL bMode);

	//
	// Helper Event Parsers
	//

	HRESULT RTCClientEvent(IDispatch *pIDispatch);
	HRESULT RTCSessionStateEvent(IDispatch *pIDispatch);
	HRESULT RTCMediaEvent(IDispatch *pIDispatch);


protected:
	HICON m_hIcon;

	//
	// IRTCClient Interface , default interface on 
	// the RTCClient CoClass
	//


	IRTCClient   *m_pIRTCClient;

	//
	// Pointer to Client Session
	//

	IRTCSession  *m_pIRTCSession;

	//
	// IConnectionPoint Advise Cookie
	//

	DWORD         m_dwEventCookie;


	//
	// Video Capture and Render Video Windows
	//

	IVideoWindow *m_pIRecvVideo;
	IVideoWindow *m_pIPrevVideo;
	VARIANT_BOOL  m_bAutoAnswer;

	//
	// Internal Reference Count
	//

	LONG          m_dwRefCount;


	// Generated message map functions
	//{{AFX_MSG(CRTCIncomingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAnswer();
	afx_msg void OnDisconnect();
	afx_msg void OnCheckauto();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // ___RTCINCOMINGDLG_H____
