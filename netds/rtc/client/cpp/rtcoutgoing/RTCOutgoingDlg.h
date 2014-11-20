/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCOutGoingDlg.h
 * ---------- [ Header Declaration File]
 * 
 * @Description :
 * -------------
 * Class Declaration file for the Main RTCOutgoing Dialog Box
 * this Class will contain the state machine transitions
 * of the RTC Core Sample,creating PC-PC,PC-PH,PH-PH calls, 
 * receiving events disconnected calls, displaying video 
 * starting T120 App Sharing and Starting the Tunning Wizard
 *
 *
 ******************************************************/



#ifndef ____RTCOUTGOINGDLG_H_____
#define ____RTCOUTGOINGDLG_H_____

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WM_RTCCORE_EVENT    WM_USER+501

#define RECVVID_LEFT        420
#define RECVVID_TOP         170
#define RECVVID_RIGHT       13
#define RECVVID_BOTTOM      20


#define PREVVID_LEFT        420
#define PREVVID_TOP         35
#define PREVVID_RIGHT       13
#define PREVVID_BOTTOM      20


#define MAX_RTCTIMEOUT      5000



/////////////////////////////////////////////////////////////////////////////
// CRTCOutgoingDlg dialog

class CRTCOutgoingDlg : public CDialog,
					    public IRTCEventNotification
{
// Construction
public:
	CRTCOutgoingDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//
	//{{AFX_DATA(CRTCOutgoingDlg)
	enum { IDD = IDD_RTCOUTGOING_DIALOG };
	CEdit	m_ctrlEditStatus;
	CEdit	m_editLURI;
	CEdit	m_editDURI;
	CListBox	m_lbParties;
	CButton	m_btnPcPh;
	CButton	m_btnPhPh;
	CButton	m_btnPcPc;
	CButton	m_btnTune;
	CButton	m_btnRemoveParty;
	CButton	m_btnAppSharing;
	CButton	m_btnAddParty;
	CButton	m_btnDisconnect;
	CButton	m_btnConnect;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTCOutgoingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//
	// Initializing the State of the Dialog Box Controls
	//


	void InitialControlsState();

	//
	// Helpers for Changing the state of
	// the UI Controls
	//


	void StateConnectIDC(VARIANT_BOOL fEnable);
	void StateConnectIDCVisible(VARIANT_BOOL fVisible);
	void StateDisconnectIDC(VARIANT_BOOL fEnable);
	void StateAddPartyIDC(VARIANT_BOOL fEnable);
	void StateAddPartyIDCVisible(VARIANT_BOOL fVisible);
	void StateRemovePartyIDC(VARIANT_BOOL fEnable);
	void StateTuneIDC(VARIANT_BOOL fEnable);
	void StateAppSharingIDC(VARIANT_BOOL fEnable);
	void StateDUriIDC(VARIANT_BOOL fEnable);
	void StateLUriIDC(VARIANT_BOOL fEnable);
	void StatePartyListIDC(VARIANT_BOOL fEnable);
	void AddToParticipantList(BSTR bstrParty);


	// 
	// Initialization of state machine and shutdown
	//

	HRESULT InitializeRTCApp();
	HRESULT ShutdownRTCApp();


	//
	// Event Source Method for Receiving RTC Events
	// and posting Messages to Main thread RTCEventThread
	//

	HRESULT STDMETHODCALLTYPE Event(
			RTC_EVENT RTCEvent,
            IDispatch * pEvent
            );

	
	// IUnknown Implementations from IRTCEventNotification
	// 
	
	HRESULT	STDMETHODCALLTYPE QueryInterface(REFIID iid, 
											 void **ppvObject);

	ULONG	STDMETHODCALLTYPE AddRef(void);
	ULONG	STDMETHODCALLTYPE Release(void);


	//
	// state transition methods for sessions
	//

	typedef enum tagSESSION_OPERATION 
	{

		STO_CREATE,
		STO_ADDPARTY,
		STO_REMOVEPARTY,
		STO_TERMINATE

	} SESSION_OPERATION;
	

	//
	// RTC Core Helpers for State Transition
	// operations
	//

	HRESULT StateCoCreateClient();
	HRESULT StateInitClient();
	HRESULT StateSinkEvent(LONG lEventMask);
	HRESULT StateUnSinkEvent();

	HRESULT StateSessionControl(SESSION_OPERATION soType);
	HRESULT StateQueryActiveMedia(LONG *plActiveMedia);
	HRESULT StateListenMode(RTC_LISTEN_MODE lMode);
	HRESULT StateStartT120(RTC_T120_APPLET tType);
	HRESULT StateStartTunning();
	HRESULT AddToMediaCapabilities(LONG lMedia, VARIANT_BOOL fPersist);
	HRESULT RemoveFromMediaCapabilities(LONG lMedia, VARIANT_BOOL fPersist);
	HRESULT ReleaseStateMachine();

	//
	// Helper Event Parsers
	//

	HRESULT RTCClientEvent(IDispatch *pIDispatch);
	HRESULT RTCSessionStateEvent(IDispatch *pIDispatch);
	HRESULT RTCMediaEvent(IDispatch *pIDispatch);

// Implementation
protected:
	HICON m_hIcon;

	//
	// Client CoClass default interface
	//
	
	IRTCClient      *m_pIRTCClient;

	//
	// pointer to Session interface
	//

	IRTCSession     *m_pIRTCSession;

	//
	// Callback event/Advise cookie
	//

	DWORD            m_dwEventCookie;

	//
	// Render / Capture Video Windows
	//

	IVideoWindow    *m_pIRecvVideo;
	IVideoWindow    *m_pIPrevVideo;

	//
	// Pointer to Participant Information
	//

	IRTCParticipant *m_pIRTCParty;
	LONG             m_dwRefCount;

	//
	// Type of the Session
	//

	RTC_SESSION_TYPE m_SessionType;

	//
	// Generated message map functions
	//
	//{{AFX_MSG(CRTCOutgoingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPcpcradio();
	afx_msg void OnPcphradio();
	afx_msg void OnPhphradio();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnAppsharing();
	afx_msg void OnTune();
	afx_msg void OnAddparty();
	afx_msg void OnRemoveparty();
	afx_msg void OnClose();
	afx_msg void OnSelchangePartylist();
	afx_msg void OnDblclkPartylist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //  ____RTCOUTGOINGDLG_H_____
