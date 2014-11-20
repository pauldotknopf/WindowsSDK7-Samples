/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCIncomginDlg.cpp
 * ---------  [ Source Definition File]
 * 
 *
 * @Description :
 * -------------
 * Class Definition file for the Main RTCIncoming Dialog Box
 * this Class will contain the state machine transitions
 * of the RTC Core Sample, answering calls, receiving events
 * disconnected calls, displaying video and auto answer
 *
 *
 ******************************************************/

#include "stdafx.h"
#include "RTCIncoming.h"
#include "RTCIncomingDlg.h"
#include "Utils.h"

//
// Should change this to link to library 
//



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**********************************************************
//@Class CAboutDlg
//
//@Description
//  
//  Class for the About Dialog Box
//
//
//*********************************************************/

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/**********************************************************
//@CAboutDlg()
//
//@Description
//  
//  Constructor of the CAboutDlg Class
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}


/**********************************************************
//@DoDataExchange()
//
//@Description
//  
//  calles CDialog::DoDataExchange
//
//@Args In:     CDataExchange *
//@Args Out:    Void
//
//
//*********************************************************/
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/**********************************************************
//@CRTCIncomingDlg
//
//@Description
//  
//  Constructor for CRTCIncomgDlg box initializes member
//  interfaces and variables declared in class.
//
//@Args In:     CWnd * 
//@Args Out:    Void
//
//
//*********************************************************/

CRTCIncomingDlg::CRTCIncomingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTCIncomingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTCIncomingDlg)

    m_pIRTCClient    = NULL;
	m_pIRTCSession   = NULL;
	m_dwEventCookie  = 0;
	m_pIRecvVideo    = NULL;
	m_pIPrevVideo    = NULL;
	m_bAutoAnswer    = VARIANT_FALSE;

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



/**********************************************************
//@DoDataExchange()
//
//@Description
//  
//  Calle CDialog::DoDataExchange
//
//@Args In:     CDataExchange *
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCIncomingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTCIncomingDlg)
	DDX_Control(pDX, IDC_CHECKAUTO, m_chkAutoAnswer);
	DDX_Control(pDX, IDC_STATUS, m_ctrlEditStatus);
	DDX_Control(pDX, IDC_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_ANSWER, m_btnAnswer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRTCIncomingDlg, CDialog)

	////
	//{{AFX_MSG_MAP(CRTCIncomingDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ANSWER, OnAnswer)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_CHECKAUTO, OnCheckauto)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**********************************************************
//@OnInitDialog()
//
//@Description
//  
//  Called when Dialog Box is Initialized
//
//@Args In:     Void
//@Args Out:    BOOL 
//
//
//*********************************************************/

BOOL CRTCIncomingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range
	//

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	//
	// Initalize the COM Threading Model
	//
	

	::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);

	//
	// UI Disable Call Control Buttons
	//
	//

	StateAnswerIDC(VARIANT_FALSE);
	StateDisconnectIDC(VARIANT_FALSE);


	//
	// Initialize RTC App to get ready for 
	// Incoming Call listen mode
	//
	

	HRESULT hr = InitializeRTCApp();


	if (FAILED(hr))
	{
		LOGMSGERROR(_T("InitializeRTCApp() Failed: 0x%x"), hr);
		return FALSE;
	}
	
	// return TRUE  unless you set the focus to a control
	return TRUE;  
	
}



/**********************************************************
//@OnSysCommand()
//
//@Description
//  
//  Called when Dialog Box is Initialized
//
//@Args In:     UINT nID, LPARAM lParam
//@Args Out:    VOID 
//
//
//*********************************************************/

void CRTCIncomingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


/**********************************************************
//@OnSysCommand()
//
//@Description
//  
//  If you add a minimize button to your dialog, you will need 
//  the code below to draw the icon.  For MFC applications using 
//  the document/view model, this is automatically done for you by
//  the framework.
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/

void CRTCIncomingDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/**********************************************************
//@OnQueryDragIcon()
//
//@Description
//  
//  
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/

HCURSOR CRTCIncomingDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



/**********************************************************
//@ Entry Point and Exit Point of the Application 
//*********************************************************/

/**********************************************************
//@InitializeRTCApp()
//
//@Description
//  
//  - CoCreating CLSID_RTCClient object and getting
//    the IRTCClient Interface
//  - Initializing the Client interface
//  - Setting the Event Call Back Connection Point for 
//    the Source sink
//  - Setting the Listen mode (opening SIP ports) 
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::InitializeRTCApp()
{

	HRESULT hr = S_OK;


	//
	// CoCreating CLSID_RTCClient object and getting
	// the IRTCClient Interface
	//

	hr = StateCoCreateClient();

	if (FAILED(hr))
	{
		return hr;
	}

	//
	// Initializing the Client
	//

	hr = StateInitClient();

	if (FAILED(hr))
	{
		return hr;
	}

	// Setting the Event Call Back Connection Point for 
	// the Source sink

	hr = StateSinkEvent(0);

	if (FAILED(hr))
	{
		return hr;
	}

	//
	// Setting the Listen mode to receive incoming calls 
	// event notifications. RTCLM_BOTH flag causes the 
	// application to open and listen on a dynamic and a
	// registered port 5060 for any incoming SIP messages.
	//

	hr = StateListenMode( RTCLM_BOTH );
	
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}


/**********************************************************
//@ShutdownRTCApp
//
//@Description
//  
//   DestroyWindow shutdown
//
//
//*********************************************************/
HRESULT CRTCIncomingDlg::ShutdownRTCApp()
{

	HRESULT hr  = S_OK;


    _ASSERTE(NULL != m_pIRTCClient);

    StateUnSinkEvent();

    hr = m_pIRTCClient->Shutdown();
 
   if (FAILED(hr))
   {
  	  LOGMSGERROR(_T("IRTCClient::Shutdown() Failed 0x%x"), hr);
   }

   ReleaseStateMachine();

   CoUninitialize();
   DestroyWindow();

	return S_OK;
}



/**********************************************************
//@ UI Helpers to Enable/Disable Buttons
//*********************************************************/
/**********************************************************
//@StateAnswerIDC
//
//@Description
//
//  Changes the Style of the Answer Button from enabled/
//  to disabled
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/
void 
CRTCIncomingDlg::StateAnswerIDC(VARIANT_BOOL fEnable)
{
   if (VARIANT_TRUE == fEnable)
   {

	   	m_btnAnswer.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();
		
   }
   else
   {
	   	m_btnAnswer.ModifyStyle( 0 ,  WS_DISABLED, 0);
		RedrawWindow();
   }
}

/**********************************************************
//@StateDisconnectIDC
//
//@Description
//
//  Changes the Style of the Disconnect Button from enabled/
//  to disabled
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/
void 
CRTCIncomingDlg::StateDisconnectIDC(VARIANT_BOOL fEnable)
{

   if (VARIANT_TRUE == fEnable)
   {

	   	m_btnDisconnect.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();
   }
   else
   {
	   	m_btnDisconnect.ModifyStyle( 0 ,  WS_DISABLED, 0);
		RedrawWindow();
   }
}



/**********************************************************
//@ Dialog Box Event/Message Handlers
//*********************************************************/

/**********************************************************
//@OnAnswer
//
//@Description
//
//  Called when the Answer Button is pressed
//  
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/
void CRTCIncomingDlg::OnAnswer() 
{

	HRESULT hr = S_OK;
	
	hr = StateSessionControl(STO_ANSWER);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("StateSessionControl(ANSWER): Failed 0x%x"), hr);
	}
	else
	{
		StateDisconnectIDC(VARIANT_TRUE);
	}
}

/**********************************************************
//@OnDisconnect
//
//@Description
//
//  Called when Disconnect button is pressed
//  
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/

void CRTCIncomingDlg::OnDisconnect() 
{

	HRESULT  hr   = S_OK;
	hr = StateSessionControl(STO_TERMINATE);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("StateSessionControl(TERMINATE): Failed 0x%x"), hr);

	}

	//
	// Disable Disconnect Button and Enable Answer
	//
	
	StateDisconnectIDC(VARIANT_FALSE);
	StateAnswerIDC(VARIANT_FALSE);
}

/**********************************************************
//@OnCheckAuto
//
//@Description
//  
//   Called when AutoAnswer check box is checked/unchecked
//
//@Args In:     Void
//@Args Out:    Void 
//
//
//*********************************************************/
void CRTCIncomingDlg::OnCheckauto() 
{
	if ( 1  == m_chkAutoAnswer.GetCheck())
	{
		SetAutoAnswerMode(VARIANT_TRUE);
	}
	else
	{
		SetAutoAnswerMode(VARIANT_FALSE);
	}	
}

/**********************************************************
//@OnClose() 
//
//@Description
//  
//   On Close Event Handler
//
//
//*********************************************************/
void CRTCIncomingDlg::OnClose() 
{

	HRESULT   hr = S_OK;

	_ASSERTE(NULL != m_pIRTCClient);

	//
	// Need to call PrepareForShutdown Before shutdown
	//

	hr = m_pIRTCClient->PrepareForShutdown();
	if (FAILED(hr))
	{
	   LOGMSGERROR(_T("IRTCClient::PrepareForShutdown Failed 0x%x"), hr);
	   m_pIRTCClient->Shutdown(); 
	   return;
	}

	ShowWindow( SW_HIDE );
}

/**********************************************************
//@WindowProc
//
//@Description
//
//  Overriding to Process the WM_RTCCOREEVENT only
//  for the RTCCE_ASYNC_CLEANUP event
//
//
//*********************************************************/

LRESULT CRTCIncomingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	
	HRESULT    hr   = S_OK;
	
	switch(message)
	{
	  case WM_RTCCORE_EVENT:
		{
		  ShutdownRTCApp();
		  break;
		}
	default : return CDialog::WindowProc(message, wParam, lParam);
		
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


/**********************************************************
//@SetAutoAnswerMode
//
//@Description
//  Setting the Answering mode for the Application
//  
//
//@Args In:     VARIANT_BOOL (T == AutoAnswer else manual)
//@Args Out:    Void 
//
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::SetAutoAnswerMode(VARIANT_BOOL bMode)
{
	m_bAutoAnswer = bMode;
	return S_OK;
}

/**********************************************************
//@ RTC State Helper Methods
//*********************************************************/
/**********************************************************
//@StateCoCreateClient()
//
//@Description
//  
//  CoCreate the RTC Client COM Object
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::StateCoCreateClient()
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);

	hr = CoCreateInstance(CLSID_RTCClient,
						  NULL,
						  CLSCTX_INPROC_SERVER,
						  IID_IRTCClient,
						  reinterpret_cast<void **> (&m_pIRTCClient)
						  );


	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("CoCreateInstance(CLSID_RTCClient) : Failed 0x%x"), hr);
		return hr;
	}

	return hr;

}


/**********************************************************
//@StateInitClient
//
//@Description
//  
//  Initialize the RTCClient Interface
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::StateInitClient()
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCClient);

	hr = m_pIRTCClient->Initialize();

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IRTCClient::Initialize : Failed 0x%x"), 
			hr);
		return hr;
	}

	return hr;
}

/**********************************************************
//@StateSessionControl
//
//@Description
//  
//  Call Control for this App we are only concerned with 
//  either answering the call or disconnecting the session
//

//@Args In:     SESSION_OPERATION = {STO_ANSWER | STO_TERMINATE}
//@Args Out:    HRESULT
//
//*********************************************************/


HRESULT 
CRTCIncomingDlg::StateSessionControl(SESSION_OPERATION soType)
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCSession);

	switch(soType) 
	{

	case STO_ANSWER:
		{
			
			hr = m_pIRTCSession->Answer();


			if ( FAILED(hr) )
			{

				LOGMSGERROR(_T("IRTCSession::Answer() Failed 0x%x"), hr);

				return hr;
			}

			//
			// Enable the Disconnect Button
			//

			StateDisconnectIDC(VARIANT_TRUE);
			
			break;
		}
	case STO_TERMINATE:
		{

			hr = m_pIRTCSession->Terminate( RTCTR_NORMAL );

			if ( FAILED(hr) )
			{
				LOGMSGERROR(_T("IRTCSession:::Terminate() Failed 0x%x"), hr);
			}

			break;
		}
	default:
		return E_NOTIMPL;

	}

	return hr;
}

/**********************************************************
//@StateListenMode
//
//@Description
//  
// Call to set the List Mode on the Client to open and 
// list to corresponding SIP ports
//
//
//@Args In:     RTC_LISTEN_MODE = {see RTC SDK for types}
//@Args Out:    HRESULT
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::StateListenMode(RTC_LISTEN_MODE lMode)
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCClient);

	//
	// Setting the Listen mode to receive incoming calls 
	// event notifications. Depending upon the flag the 
	// application will open and listen on a dynamic and/or a
	// registered port 5060 for any incoming SIP messages.
	//

	hr = m_pIRTCClient->put_ListenForIncomingSessions(lMode);

	if (FAILED(hr))
	{

		LOGMSGERROR(_T("IRTCClient::put_ListenForIncomingSessions Failed 0x%x"), 
			hr);
		return hr;
	}
	else
	{
		m_ctrlEditStatus.SetWindowText(_T("Listening ..."));
	}

	return hr;
}



/**********************************************************
//@StateQueryActiveMedia
//
//@Description
//  
// Query for Active media streams on the Client
//
//
//@Args In:     Void
//@Args Out:    long active media mask
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::StateQueryActiveMedia(LONG *plActiveMedia)
{
	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCClient);


	LONG   lActive   = 0;
	BSTR   bstrMedia = NULL;

	hr = m_pIRTCClient->get_ActiveMedia(&lActive);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::get_ActiveMedia() Failed 0x%x"), hr);
		return hr;
	}

	*plActiveMedia = lActive;


	FREE_BSTR_IF(bstrMedia);

	return hr;
}


/**********************************************************
//@StateSinkEvent()
//
//@Description
//  
//  set the Connection point source and event filter
//
//@Args In:     LONG Event Mask
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::StateSinkEvent(LONG lEventMask)
{

	HRESULT                   hr          = S_OK;
	IUnknown                  *pUnknown   = NULL;
	IConnectionPointContainer *pCPC       = NULL;
	IConnectionPoint          *pCP        = NULL;
	
	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCClient);

	//
	// For this Application we need to only
	// set the following Event Masks
	//

	lEventMask = RTCEF_CLIENT                     |
				 RTCEF_SESSION_STATE_CHANGE       |
				 RTCEF_MEDIA;

	//
	// Set the Event filter to the Client
	//
				
	hr = m_pIRTCClient->put_EventFilter(lEventMask);


	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IRTCClient::put_EventFilter Failed 0x%x"), 
			hr);

		return hr;
	}


	hr = QueryInterface(IID_IUnknown,
				        reinterpret_cast<void**> (&pUnknown)
					   );

	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("QueryInterface(IUnknown) Failed 0x%x"), hr);

		return hr;
	}

	
	//
	// Getting the ConnectionPointContainer
	// from IRTCEventNotification Interface
	//

	hr = m_pIRTCClient->QueryInterface(IID_IConnectionPointContainer,
								   reinterpret_cast<void**> (&pCPC)
								   );

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IRTCClient::QueryInterface(IConnectionPointContainer) Failed 0x%x"),
			hr);
		return hr;
	}

	
	//
	// Get the Connection Point
	//

	hr = pCPC->FindConnectionPoint( IID_IRTCEventNotification, 
									&pCP );


	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("IConnectionPointContainer::FindConnectionPoint() Failed 0x%x"), 
			hr);

		pCPC->Release();
		return hr;
	}

	//
	// Advise and store cookie for UnAdvise later
	//

	hr = pCP->Advise( pUnknown, &m_dwEventCookie );

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IConnectionPoint::Advise() Failed 0x%x"), hr);

		pCPC->Release();
		pCP->Release();
		return hr;
	}


	pCP->Release();
	pCPC->Release();
	pUnknown->Release(); 

	return hr;
}


/**********************************************************
//@StateUnSinkEvent
//
//@Description
//  
//  unsink event connection point
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCIncomingDlg::StateUnSinkEvent()
{

	HRESULT					   hr	      = S_OK;
	IUnknown                  *pUnknown   = NULL;
	IConnectionPointContainer *pCPC       = NULL;
	IConnectionPoint          *pCP        = NULL;

	
	//
	// Get IUnknown From IRTCEventNotifcation derived
	// implementation (this class)
	//

	hr = QueryInterface(IID_IUnknown,
					    reinterpret_cast<void**> (&pUnknown)
					   );

	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("QueryInterface(IUnknown) Failed 0x%x"), hr);
		return hr;
	}


	//
	// Get the Connectionpoint container
	//

	hr = m_pIRTCClient->QueryInterface(IID_IConnectionPointContainer,
								   reinterpret_cast<void**> (&pCPC)
								   );

	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("IRTCClient::QueryInterface(IConnectionPointContainer) Failed 0x%x"), hr);

		return hr;
	}

	//
	// Find the Connection Point 
	//

	hr = pCPC->FindConnectionPoint( IID_IRTCEventNotification, 
									&pCP );


	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IConnectionPointContainer::FindConnectionPoint Failed 0x%x"), 
			hr);

		pCPC->Release();
		return hr;
	}

	//
	// Call Advise and save cookie for unadvise
	//

	hr = pCP->Unadvise( m_dwEventCookie );

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IConnectionPoint::Unadvise() Failed 0x%x"), 
			hr);

		pCPC->Release();
		pCP->Release();
		return hr;
	}


	pCP->Release();
	pCPC->Release();
	pUnknown->Release(); 


	return hr;
}



/**********************************************************
//@ReleaseStateMachine
//
//@Description
//  
//  Internal Destructor , releases all the Interfaces
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

HRESULT
CRTCIncomingDlg::ReleaseStateMachine()
{

	ULONG ulRef         = 0;
	BOOL  fRet          = FALSE;

	if ( NULL != m_pIRecvVideo )
	{

		ulRef = m_pIRecvVideo->Release();
		m_pIRecvVideo = NULL;

	}

	if ( NULL != m_pIPrevVideo )
	{

		ulRef = m_pIPrevVideo->Release();
		m_pIPrevVideo = NULL;

	}

	if ( NULL != m_pIRTCSession )
	{

		ulRef = m_pIRTCSession->Release();
		m_pIRTCSession = NULL;
	}

	return S_OK;
}

/**********************************************************
//@AddToMediaCapabilities
//
//@Description
//  
//  Add Media Capability to the Client
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::AddToMediaCapabilities(LONG lMedia, VARIANT_BOOL fPersist)
{

	HRESULT hr = S_OK;
	LONG    lGetMedia = 0;
	LONG    lSetMedia = 0;

	_ASSERTE(NULL != m_pIRTCClient);

	//
	// Get the Current Media Capabilities
	// and then add the media mask to the current media
	//

	hr = m_pIRTCClient->get_MediaCapabilities(&lGetMedia);

	
	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::get_MediaCapabilities() Failed 0x%x"), hr);
		return hr;
	}
	else
	{
		//
		// Add New Media Capability to Current Media Mask
		//

		lSetMedia = lGetMedia | lMedia;

		hr = m_pIRTCClient->SetPreferredMediaTypes(lSetMedia, fPersist);

		if (FAILED(hr))
		{
			LOGMSGERROR(_T("IRTCClient::SetPreferredMediaTypes Failed 0x%x"), hr);
			return hr;
		}

	}

	return hr;
}



/**********************************************************
//@ RTCEventNotification Implementation
//*********************************************************/

/**********************************************************
//@QueryInterface
//
//@Description
//  
//  QI for IUnknown Implementation
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

HRESULT	
CRTCIncomingDlg::QueryInterface(REFIID iid, 
									 void **ppvObject)
{
	if(iid == IID_IRTCEventNotification)
	{
		
		*ppvObject = static_cast<IRTCEventNotification *> (this);
		AddRef();
		return S_OK;
	}


	if(iid == IID_IUnknown)
	{
		
		*ppvObject = static_cast<IUnknown *> (this);
		AddRef();
		return S_OK;
	}

	else 
	{
	   *ppvObject = NULL;
	   return E_NOINTERFACE;
	}

	return S_OK;
}

/**********************************************************
//@AddRef
//
//@Description
//  
//  Ref Count
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

ULONG 
CRTCIncomingDlg::AddRef()
{
	
	ULONG ulrefCount = InterlockedIncrement(&m_dwRefCount);
	return ulrefCount;
}


/**********************************************************
//@Release
//
//@Description
//  
//  object release
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

ULONG 
CRTCIncomingDlg::Release()
{

	ULONG ulrefCount = InterlockedDecrement(&m_dwRefCount);


	if(0 == ulrefCount)
	{
		delete this;
	}

	return ulrefCount;
}

/**********************************************************
//@Event
//
//@Description
//  RTC CallBack for Events, the messages are
//  posted to the thread message pump
//
//  
//
//@Args In:     RTC_EVENT , IDispatch
//@Args Out:    HRESULT
//
//
//*********************************************************/

HRESULT
STDMETHODCALLTYPE
CRTCIncomingDlg::Event(
				      RTC_EVENT RTCEvent,
					  IDispatch * pEvent
					  )

{
	HRESULT hr  = S_OK;

	if (NULL != pEvent) 
	{
		switch (RTCEvent)
		{

		  case RTCE_CLIENT                    : 
			hr = 	RTCClientEvent(pEvent);
		    break;
		  case RTCE_SESSION_STATE_CHANGE      :
		    hr = 	RTCSessionStateEvent(pEvent);
		    break;
		  case RTCE_MEDIA                     : 
		    hr =   	RTCMediaEvent(pEvent);
		    break;	
	    	default:  break;
		}
	}

    return hr;
}


/**********************************************************
//@ RTC Event Parsers
//*********************************************************/

/**********************************************************
//@RTCClientEvent
//
//@Description
//
//  Sample is not doing any Special Processing for this Event
//
//@Args In:     
//@Args Out:    
//
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::RTCClientEvent(IDispatch *pIDispatch)
{
	HRESULT               hr            = S_OK;	
	IRTCClientEvent      *pIClientEvent = NULL;
	RTC_CLIENT_EVENT_TYPE EventType;

	hr = pIDispatch->QueryInterface(IID_IRTCClientEvent,
									reinterpret_cast<void**> (&pIClientEvent)
									);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IDispatch::QueryInterface(IRTCClientEvent) Failed 0x%x"), 
			hr);
		return hr;
	}
	
	//
	// Getting the Session Interface
	//

	hr = pIClientEvent->get_EventType(&EventType);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClientEvent::get_EventType Failed 0x%x"), hr);

		pIClientEvent->Release();
		pIClientEvent = NULL;
		return hr;
	}

	if (RTCCET_ASYNC_CLEANUP_DONE  == EventType)
	{
		//
		// When the WndProc gets this message we can 
		// destroy the window
		//

		DBLOGGER(_T("Final Exit ... ASYNC_CLEANUP_DONE\n"));

		SendMessage(WM_RTCCORE_EVENT, (WPARAM) EventType, (LPARAM) NULL);
	}

	if (NULL != pIClientEvent)
	{
		pIClientEvent->Release();
		pIClientEvent = NULL;

	}
	return hr;
}

/**********************************************************
//@RTCSessionStateEvent
//
//@Description
//
//  Session State Processing 
//
//@Args In:     
//@Args Out:    
//
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::RTCSessionStateEvent(IDispatch *pIDispatch)
{
	HRESULT hr                                  = S_OK;
	IRTCSessionStateChangeEvent *pISessionState = NULL;
	RTC_SESSION_STATE SessionState;


	hr = pIDispatch->QueryInterface(IID_IRTCSessionStateChangeEvent,
									reinterpret_cast<void**> (&pISessionState)
									);

	if (FAILED(hr))
	{

		LOGMSGERROR(_T("IDispatch::QueryInterface(IRTCSessionStateChangeEvent) Failed 0x%x"), 
			hr);
		return hr;
	}
	
	//
	// Getting the Session Interface
	//

	hr = pISessionState->get_State(&SessionState);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCSessionStateChangeEvent::get_State Failed 0x%x"), hr);

		pISessionState->Release();
		pISessionState = NULL;
		return hr;
	}
		

	//
	// Determine the Session State to Perform automatic 
	// operation or set the state in the Status Edit Box
	// if the session pointer is not null then we currently 
	// have a session going on.
	//

	switch(SessionState)
	{
	case RTCSS_IDLE: break;
	case RTCSS_INCOMING:
	{

		if ( m_pIRTCSession != NULL) 
		{
			pISessionState->Release();
			pISessionState = NULL;
			return S_OK;
		}

		m_ctrlEditStatus.SetWindowText(_T("Incoming Call..."));
		hr = pISessionState->get_Session(&m_pIRTCSession);

		if (FAILED(hr))
		{
			LOGMSGERROR(_T("IRTCSessionStateChangeEvent::get_Session Failed 0x%x"), hr);
			pISessionState->Release();
			pISessionState = NULL;
			return hr;
		}

		//
		// Answer Automatically if on Auto Answer mode
		//


		if (m_bAutoAnswer == VARIANT_TRUE)
		{
			
			if(SUCCEEDED(hr = StateSessionControl(STO_ANSWER)))
			{
				StateDisconnectIDC(VARIANT_TRUE);
			}
		}
		else
		{
		   StateAnswerIDC(VARIANT_TRUE);
		}

		break;
	}
	case RTCSS_ANSWERING: 	
		m_ctrlEditStatus.SetWindowText(_T("Answering ..."));
		break;
	case RTCSS_INPROGRESS:
		m_ctrlEditStatus.SetWindowText(_T("In Progress ..."));
		break;
	case RTCSS_CONNECTED :
		m_ctrlEditStatus.SetWindowText(_T("Connected ..."));
		break;
	case RTCSS_DISCONNECTED:
		{
			//
			// Release the Session object if the Session
			// was terminated
			//

			StateDisconnectIDC(VARIANT_FALSE);
			StateAnswerIDC(VARIANT_FALSE);
			m_ctrlEditStatus.SetWindowText(_T("Disconnected ..."));
			m_ctrlEditStatus.SetWindowText(_T("Listening ..."));


			m_pIRTCSession->Release();
			m_pIRTCSession = NULL;

			break;
		}
	default : 
		{

			pISessionState->Release();
			pISessionState = NULL;
			return E_UNEXPECTED;

		}

	}


	if (NULL != pISessionState)
	{
		pISessionState->Release();
		pISessionState = NULL;

	}
	
	return hr;
}


/**********************************************************
//@RTCMediaEvent
//
//@Description
//
//  Parser for Media Event
//
//  here we start the video window in a specific 
//  window location of the dialog box, we can 
//  only do this if the video stream has started
//  if an application doesnt care about setting 
//  the video window coordinated you can get the
//  video window and put_AutoShow(TRUE) , this will
//  make the video window automatically visible when
//  the stream has started. 
//
//
//@Args In:     IDispatch Event Interface
//@Args Out:    HRESULT
//
//
//*********************************************************/

HRESULT 
CRTCIncomingDlg::RTCMediaEvent(IDispatch *pIDispatch)
{
	HRESULT hr                          = S_OK;
	IRTCMediaEvent        *pIMediaEvent = NULL;
	LONG                   lMediaType   = 0;
	HWND                   hwdVideoRecv , hwdVideoSend;
	RTC_MEDIA_EVENT_TYPE   EventType;

	hr = pIDispatch->QueryInterface(IID_IRTCMediaEvent,
									reinterpret_cast<void**> (&pIMediaEvent)
									);

	if (FAILED(hr))
	{

		LOGMSGERROR(_T("IDispatch::QueryInterface(IRTCMediaEvent) Failed 0x%x"), hr);
		return hr;
	}

	pIMediaEvent->get_MediaType(&lMediaType);
	pIMediaEvent->get_EventType(&EventType);

	if ( (lMediaType == RTCMT_VIDEO_RECEIVE) && (EventType == RTCMET_STARTED) )
	{

		   RECT    rectPos;				

		   hr = m_pIRTCClient->get_IVideoWindow(  RTCVD_RECEIVE , &m_pIRecvVideo);

			if (SUCCEEDED(hr))
			{
				CString cstrText;

				hr = m_pIRecvVideo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
        
				if(FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::put_WindowStyle() Failed 0x%x"), hr);
										
					pIMediaEvent->Release();
					pIMediaEvent = NULL;

					return hr;
				}
        
				hr = m_pIRecvVideo->put_Owner((OAHWND) m_hWnd);
        
				if(FAILED(hr))
				{
					
					LOGMSGERROR(_T("IVideoWindow::put_Owner() Failed 0x%x"), hr);
										
					pIMediaEvent->Release();
					pIMediaEvent = NULL;

					return hr;
				}

				
				GetDlgItem( IDC_VRECV, &hwdVideoRecv );

				::GetClientRect(hwdVideoRecv, &rectPos);

		        hr = m_pIRecvVideo->SetWindowPosition(
			    rectPos.left   + RECVVID_LEFT,
				rectPos.top    + RECVVID_TOP,
				rectPos.right  - RECVVID_RIGHT,
				rectPos.bottom - RECVVID_BOTTOM
				);
			        
				if(FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::SetWindowPosition() Failed 0x%x"), hr);
										
					pIMediaEvent->Release();
					pIMediaEvent = NULL;

					return hr;
				} 
        
				hr = m_pIRecvVideo->put_Visible(TRUE);

				if (FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::put_Visible Failed 0x%x"), hr);
										
					pIMediaEvent->Release();
					pIMediaEvent = NULL;

					return hr;
				}

			}

	}

	else if ( ( lMediaType == RTCMT_VIDEO_RECEIVE) && (EventType == RTCMET_STOPPED))
	{

		//
		// we must put_Visible false and set the owner
		// as null See DShow IVideoWindow SDK 
		// NOTE: should not close window without releasing video 
		// or not removing handler ownership from video window.
		//

		m_pIRecvVideo->put_Visible(FALSE);
		m_pIRecvVideo->put_Owner(NULL);   


		m_pIRecvVideo->Release();
		m_pIRecvVideo = NULL;
		
	}

	else if ( (lMediaType == RTCMT_VIDEO_SEND) &&  (EventType == RTCMET_STARTED))
	{

		   RECT    rectPos;				

		   hr = m_pIRTCClient->get_IVideoWindow(  RTCVD_PREVIEW , &m_pIPrevVideo);

			if (SUCCEEDED(hr))
			{
				CString cstrText;

				hr = m_pIPrevVideo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
        
				if(FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::put_WindowStyle() Failed 0x%x"), hr);
					
					pIMediaEvent->Release();
					pIMediaEvent = NULL;

					return hr;
				}
        
				hr = m_pIPrevVideo->put_Owner((OAHWND) m_hWnd);
        
				if(FAILED(hr))
				{
					
					LOGMSGERROR(_T("IVideoWindow::put_Owner() Failed 0x%x"), hr);
					
					pIMediaEvent->Release();
					pIMediaEvent = NULL;
					return hr;
				}

				GetDlgItem( IDC_VSEND , &hwdVideoSend );

				::GetClientRect(hwdVideoSend, &rectPos);

		        hr = m_pIPrevVideo->SetWindowPosition(
		        rectPos.left + PREVVID_LEFT,
				rectPos.top  + PREVVID_TOP,
				rectPos.right - PREVVID_RIGHT,
				rectPos.bottom - PREVVID_BOTTOM);

			        
				if(FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::SetWindowPosition() Failed 0x%x"), hr);
					
					pIMediaEvent->Release();
					pIMediaEvent = NULL;
					return hr;
				} 
        
				hr = m_pIPrevVideo->put_Visible(TRUE);

				if (FAILED(hr))
				{
					LOGMSGERROR(_T("IVideoWindow::put_Visible Failed 0x%x"), hr);
					
					pIMediaEvent->Release();
					pIMediaEvent = NULL;
					return hr;
				}

			}

	}

	else if ( ( lMediaType == RTCMT_VIDEO_SEND) && 
		 (EventType == RTCMET_STOPPED))
	{

		//
		// we must put_Visible false and set the owner
		// as null See DShow IVideoWindow SDK 
		// NOTE: should not close window without releasing video 
		// or not removing handler ownership from video window.
		//

		m_pIPrevVideo->put_Visible(FALSE);
		m_pIPrevVideo->put_Owner(NULL);   

		m_pIPrevVideo->Release();
		m_pIPrevVideo = NULL;

		
	}


	if (NULL != pIMediaEvent)
	{

		pIMediaEvent->Release();
		pIMediaEvent = NULL;

	}

	return hr;

}


