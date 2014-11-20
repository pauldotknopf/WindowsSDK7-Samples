/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCOutGoingDlg.cpp
 * ---------- [ Source Definition File]
 *
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


#include "stdafx.h"
#include "RTCOutgoing.h"
#include "RTCOutgoingDlg.h"



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
//  About Dialog Box
//
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
//  Constructor
//
//@Args In:     Void
//@Args Out:    Void
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



/**********************************************************
//@CAboutDlg Message Map
//
//@Description
//  
//
//
//*********************************************************/

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/**********************************************************
//@Class CRTCOutgoingDlg
//
//
//
//*********************************************************/


CRTCOutgoingDlg::CRTCOutgoingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTCOutgoingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTCOutgoingDlg)

	m_pIRTCClient    = NULL;
	m_pIRTCSession   = NULL;
	m_pIRecvVideo    = NULL;
	m_pIPrevVideo    = NULL;
	m_pIRTCParty     = NULL;
	m_SessionType    = RTCST_PC_TO_PC;

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


/**********************************************************
//@DoDataExchange()
//
//@Description
//  
//
//@Args In:     CDataExchange *
//@Args Out:    Void
//
//
//*********************************************************/
void CRTCOutgoingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTCOutgoingDlg)
	DDX_Control(pDX, IDC_STATUS, m_ctrlEditStatus);
	DDX_Control(pDX, IDC_LURI, m_editLURI);
	DDX_Control(pDX, IDC_DURI, m_editDURI);
	DDX_Control(pDX, IDC_PARTYLIST, m_lbParties);
	DDX_Control(pDX, IDC_PCPHRADIO, m_btnPcPh);
	DDX_Control(pDX, IDC_PHPHRADIO, m_btnPhPh);
	DDX_Control(pDX, IDC_PCPCRADIO, m_btnPcPc);
	DDX_Control(pDX, IDC_TUNE, m_btnTune);
	DDX_Control(pDX, IDC_REMOVEPARTY, m_btnRemoveParty);
	DDX_Control(pDX, IDC_APPSHARING, m_btnAppSharing);
	DDX_Control(pDX, IDC_ADDPARTY, m_btnAddParty);
	DDX_Control(pDX, IDC_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_CONNECT, m_btnConnect);
	//}}AFX_DATA_MAP
}



/**********************************************************
//@CRTCOutGoingDlg MessageMap
//
//@Description
//  
//
//
//*********************************************************/

BEGIN_MESSAGE_MAP(CRTCOutgoingDlg, CDialog)
	//{{AFX_MSG_MAP(CRTCOutgoingDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PCPCRADIO, OnPcpcradio)
	ON_BN_CLICKED(IDC_PCPHRADIO, OnPcphradio)
	ON_BN_CLICKED(IDC_PHPHRADIO, OnPhphradio)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_APPSHARING, OnAppsharing)
	ON_BN_CLICKED(IDC_TUNE, OnTune)
	ON_BN_CLICKED(IDC_ADDPARTY, OnAddparty)
	ON_BN_CLICKED(IDC_REMOVEPARTY, OnRemoveparty)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_PARTYLIST, OnSelchangePartylist)
	ON_LBN_DBLCLK(IDC_PARTYLIST, OnDblclkPartylist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**********************************************************
//@OnInitDialog
//
//@Description
//  
//
//@Args In:     CDataExchange *
//@Args Out:    Void
//
//
//*********************************************************/
BOOL CRTCOutgoingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	//About Menu is Added to the Dialogbox
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
	// Icon Settings
	//

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		


	::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);

	m_ctrlEditStatus.SetWindowText(_T("Initializing ..."));

	HRESULT hr = InitializeRTCApp();


	if (FAILED(hr))
	{

		LOGMSGERROR(_T("InitializeRTCApp() Failed: 0x%x"), hr);

		return FALSE;

	}


	//
	// Set the Initial States of the Control
	//


	InitialControlsState();
	
	return TRUE;  
}

/**********************************************************
//@OnSysCommand()
//
//@Description
//  
//
//@Args In:     UINT, LPARAM
//@Args Out:    Void
//
//
//*********************************************************/
void CRTCOutgoingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//@OnPaint
//
//@Description
//  
// For minimize/maximize re-painting
//
//
//@Args In:     
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnPaint() 
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
//@OnQueryDragIcon
//
//@Description
//  
//
//@Args In:     Void
//@Args Out:    HCURSOR
//
//
//*********************************************************/
HCURSOR CRTCOutgoingDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/**********************************************************
//@UI Manipulation Methods
//*********************************************************/

/**********************************************************
//@InitlialControlsState
//
//@Description
//  
//  Setting the UI Controls to their initial State
//
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::InitialControlsState()
{

	HRESULT   hr  = S_OK;
	//
	// Disable Buttons that should not be functional
	//

	StateDisconnectIDC(VARIANT_FALSE);
	StateAddPartyIDC(VARIANT_FALSE);
	StateRemovePartyIDC(VARIANT_FALSE);
	StateLUriIDC(VARIANT_FALSE);
	StatePartyListIDC(VARIANT_FALSE);

	StateConnectIDCVisible(VARIANT_TRUE);
	StateAddPartyIDCVisible(VARIANT_FALSE);

	m_btnPcPh.SetCheck(0);
	m_btnPhPh.SetCheck(0);

	RedrawWindow();

	m_btnPcPc.SetCheck(1);

	RedrawWindow();
	
}

/**********************************************************
//@StateConnectIDO
//
//@Description
//   
//  Disabled/Enables the Connect Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateConnectIDC(VARIANT_BOOL fEnable)
{
	if (VARIANT_TRUE == fEnable)
	{
		m_btnConnect.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_btnConnect.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}
}


/**********************************************************
//@StateConnectIDOVisible
//
//@Description
//   
//  Disabled/Enables the Connect Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateConnectIDCVisible(VARIANT_BOOL fVisible)
{
	if (VARIANT_TRUE == fVisible)
	{
		m_btnConnect.ModifyStyle(  0 , WS_VISIBLE , 0);
		RedrawWindow();

	}
	else
	{
		m_btnConnect.ModifyStyle( WS_VISIBLE , 0 , 0);
		RedrawWindow();
	}
}

/**********************************************************
//@StateDisconnectIDC
//
//@Description
//  
//   Enables /Disables the Disconnect Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateDisconnectIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_btnDisconnect.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_btnDisconnect.ModifyStyle(0,  WS_DISABLED  , 0);
		RedrawWindow();
	}

}


/**********************************************************
//@StateAddPartyIDC
//
//@Description
//  
//  Enable / Disable Add Party
//
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateAddPartyIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_btnAddParty.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();
	}
	else
	{
		m_btnAddParty.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}

}



/**********************************************************
//@StateAddPartyIDCVisible(VARIANT_BOOL fEnable)
//
//@Description
//   
//  Disabled/Enables the Connect Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateAddPartyIDCVisible(VARIANT_BOOL fVisible)
{
	if (VARIANT_TRUE == fVisible)
	{
		m_btnAddParty.ModifyStyle( 0 , WS_VISIBLE  , 0);
		RedrawWindow();

	}
	else
	{
		m_btnAddParty.ModifyStyle( WS_VISIBLE , 0 , 0);
		RedrawWindow();
	}
}

/**********************************************************
//@StateRemovePartyIDC
//
//@Description
//    
//   Enable /Disable Remove Party Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateRemovePartyIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_btnRemoveParty.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();
	}
	else
	{
		m_btnRemoveParty.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}
}



/**********************************************************
//@StateTuneIDC
//
//@Description
//  
// Enable /Disable Tunning Wizard
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateTuneIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_btnTune.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_btnTune.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}
}


/**********************************************************
//@StateAppSharingIDC
//
//@Description
//  
//  Enable / Disable App Sharing Button
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateAppSharingIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_btnAppSharing.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_btnAppSharing.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}

}


/**********************************************************
//@StateDUriIDC
//
//@Description
//  
//  Enable / Disable Destination URI
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateDUriIDC(VARIANT_BOOL fEnable)
{
	if (VARIANT_TRUE == fEnable)
	{
		m_editDURI.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_editDURI.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}
}


/**********************************************************
//@StateLUriIDC
//
//@Description
//  
//  Enable / Disable Local URI (Req for PHPH)
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StateLUriIDC(VARIANT_BOOL fEnable)
{

	if (VARIANT_TRUE == fEnable)
	{
		m_editLURI.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();

	}
	else
	{
		m_editLURI.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}

}


/**********************************************************
//@StatePartyListIDC
//
//@Description
//  
//   Enable / Disable Participant List
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void 
CRTCOutgoingDlg::StatePartyListIDC(VARIANT_BOOL fEnable)
{
	if (VARIANT_TRUE == fEnable)
	{
		m_lbParties.ModifyStyle( WS_DISABLED , 0 , 0);
		RedrawWindow();
	}
	else
	{
		m_lbParties.ModifyStyle( 0 , WS_DISABLED , 0);
		RedrawWindow();
	}

}


/**********************************************************
//@AddToParticipantList()
//
//@Description
//  
//  Adding Participants to the List
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void CRTCOutgoingDlg::AddToParticipantList(BSTR bstrParty)
{
	_ASSERTE( bstrParty != NULL);

	m_lbParties.AddString(bstrParty);
	StatePartyListIDC(VARIANT_TRUE);
}


/**********************************************************
//@OnPcPcRadio()
//
//@Description
//  
//  For PC/PC Calls Check Box
//
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnPcpcradio() 
{
	HRESULT    hr   = S_OK;

	if (1 == m_btnPcPc.GetCheck())
	{
       m_btnPcPc.SetCheck(1);
	   m_btnPcPh.SetCheck(0);
	   m_btnPhPh.SetCheck(0);

	   StateAppSharingIDC(VARIANT_TRUE);
	   StateLUriIDC(VARIANT_FALSE);	   

	   m_SessionType =   RTCST_PC_TO_PC;
	   
	   m_editDURI.SetWindowText(_T(""));
	   m_editLURI.SetWindowText(_T(""));	      
      
	}	
}



/**********************************************************
//@OnPcPhRadio()
//
//@Description
//  
// For PC/PH Calls Check Box
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnPcphradio() 
{

	HRESULT   hr = S_OK;
	if (1 == m_btnPcPh.GetCheck())
	{
       m_btnPcPc.SetCheck(0);
	   m_btnPcPh.SetCheck(1);
	   m_btnPhPh.SetCheck(0);

	   StateAppSharingIDC(VARIANT_FALSE);
	   StateLUriIDC(VARIANT_FALSE);

	   m_SessionType =  RTCST_PC_TO_PHONE ;

	   m_editDURI.SetWindowText(_T(""));
	   m_editLURI.SetWindowText(_T(""));

   	}	
}


/**********************************************************
//@OnPhPhRadio()
//
//@Description
//  
// For PH to PH Check Box
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnPhphradio() 
{
	HRESULT     hr = S_OK;
	if (1 == m_btnPhPh.GetCheck())
	{
       m_btnPcPc.SetCheck(0);
	   m_btnPcPh.SetCheck(0);
	   m_btnPhPh.SetCheck(1);

	   StateAppSharingIDC(VARIANT_FALSE);
	   StateLUriIDC(VARIANT_TRUE);

	   m_SessionType = RTCST_PHONE_TO_PHONE;

	   
	   m_editDURI.SetWindowText(_T(""));
	   m_editLURI.SetWindowText(_T(""));

	}	
}


/**********************************************************
//@UI Event Handlers and Message Handlers
//*********************************************************/

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
LRESULT CRTCOutgoingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
//@OnClose() 
//
//@Description
//  
//   On Close Event Handler
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnClose() 
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
//@OnConnect
//
//@Description
//  
// Called / EventHandler for Connect Button Clicked
//
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void CRTCOutgoingDlg::OnConnect() 
{
	StateSessionControl(STO_CREATE);
}


/**********************************************************
//@OnDisconnect
//
//@Description
//  
// Called / EventHandler for Disconnect Button Clicked
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnDisconnect() 
{
	StateSessionControl(STO_TERMINATE);
	StateConnectIDC(VARIANT_TRUE);
}


/**********************************************************
//@OnAppsharing
//
//@Description
//  
// Called / EventHandler for App Sharing Button Clicked
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnAppsharing() 
{
	StateStartT120(RTCTA_APPSHARING);
}



/**********************************************************
//@OnTune
//
//@Description
//  
//  Called / EventHandler for Tune Button Clicked
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnTune() 
{
	StateStartTunning();
}

/**********************************************************
//@OnAddParty
//
//@Description
//  
// Called / EventHandler for Add Party Button Clicked
// 
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnAddparty() 
{
	
	StateSessionControl(STO_ADDPARTY);
	
}


/**********************************************************
//@OnRemoveParty
//
//@Description
//  
// Called / EventHandler for Remove Party Button Clicked
// 
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnRemoveparty() 
{
	StateSessionControl(STO_REMOVEPARTY);
}


/**********************************************************
//@OnSelchangePartylist() 
//
//@Description
//  
// Called / EventHandler for Remove Party Button Clicked
// implementation not operational for this sample
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
void CRTCOutgoingDlg::OnSelchangePartylist() 
{

	HRESULT               hr            = S_OK;
	IRTCEnumParticipants *pIEnumParties = NULL;
	IRTCParticipant      *pIParty       = NULL;
	CString               cstrURI;
	BSTR                  bstrURI = NULL, bstrParty = NULL;
	VARIANT_BOOL          fRemove       = VARIANT_FALSE;
			
	INT lIndex = m_lbParties.GetCurSel();
	m_lbParties.GetText( lIndex, cstrURI);

	bstrURI = cstrURI.AllocSysString();

	DBLOGGER(_T("Selected  %s\n"), bstrURI);

	hr = m_pIRTCSession->EnumerateParticipants(&pIEnumParties);

	if (FAILED(hr))
	{
	    LOGMSGERROR(_T("IRTCSession::EnumerateParticipants Failed 0x%x"), hr);
		FREE_BSTR_IF(bstrURI);
		return;
		
	}
			
	while (S_OK == (hr = pIEnumParties->Next(1, &pIParty, NULL)))
	{

		hr = pIParty->get_UserURI(&bstrParty);

		if (0 == _tcscmp(bstrParty ,  bstrURI))
		{
		   break;
		}
		else
		{
		  pIParty->Release();
	      pIParty = NULL;

		}

		FREE_BSTR_IF(bstrParty);
	}

	FREE_BSTR_IF(bstrURI);

	if (NULL != pIParty)
	{
	   hr = pIParty->get_Removable(&fRemove);

	   if (FAILED(hr))
	   {
		 LOGMSGERROR(_T("IRTCSession::RemoveParticipant Failed 0x%x"), hr);
		 return;
	   }
	   else
	   {
		   if (VARIANT_TRUE == fRemove)
		   {
			   StateRemovePartyIDC(VARIANT_TRUE);
		   }
			
	   }
	   
	   pIParty->Release();
	   pIParty = NULL;

	}
	else
	{
		LOGMSGERROR(_T("Failed to Find Selected Participant"), hr);
	}
}

/**********************************************************
//@OnDblclkPartyList
//
//@Description
//
//  When double clicking on a Participant in the list
//  a message box pops up with the participants information
//
//
//@Args In:     
//@Args Out:    
//
//
//*********************************************************/

void CRTCOutgoingDlg::OnDblclkPartylist() 
{
	HRESULT               hr            = S_OK;
	IRTCEnumParticipants *pIEnumParties = NULL;
	IRTCParticipant      *pIParty       = NULL;
	CString               cstrURI;
	BSTR                  bstrURI = NULL, bstrParty = NULL;
			
	INT lIndex = m_lbParties.GetCurSel();
	m_lbParties.GetText( lIndex, cstrURI);

	bstrURI = cstrURI.AllocSysString();

	DBLOGGER(_T("Selected  %s\n"), bstrURI);

	//
	// Enumerate the Participants and find the correct one
	//

	hr = m_pIRTCSession->EnumerateParticipants(&pIEnumParties);

	if (FAILED(hr))
	{
	    LOGMSGERROR(_T("IRTCSession::EnumerateParticipants Failed 0x%x"), hr);
		FREE_BSTR_IF(bstrURI);
		return;
		
	}
			
	while (S_OK == (hr = pIEnumParties->Next(1, &pIParty, NULL)))
	{
		//
		// Note: This application doesnt process the tel: URI 
		// or sip: string in the name so this would fail to find
		// the address. 

		hr = pIParty->get_UserURI(&bstrParty);

		if (0 == _tcscmp(bstrParty ,  bstrURI))
		{
		   break;
		}
		else
		{
		  pIParty->Release();
	      pIParty = NULL;

		}

	}

	FREE_BSTR_IF(bstrURI);

	//
	// Pop Up a Message Box with this Information
	//

	if (NULL != pIParty)
	{
	   
	   VARIANT_BOOL          fRemove       = VARIANT_FALSE;
	   RTC_PARTICIPANT_STATE PartyState;


	   pIParty->get_Removable(&fRemove);
	   pIParty->get_State(&PartyState);
	
	
	   LOGMSG(_T("Participant URI:%s\nParticipant State:%d\nParticipant Removable:%s"),
		   bstrParty, PartyState, ( (fRemove == VARIANT_TRUE) ? _T("Yes") :_T("No") ) );

	   pIParty->Release();
	   pIParty = NULL;

	}
	else
	{
		LOGMSGERROR(_T("Failed to Find Selected Participant"), hr);
	}

	FREE_BSTR_IF(bstrParty);
	
}

/**********************************************************
//@RTC Application Entry / Exit Points

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
CRTCOutgoingDlg::InitializeRTCApp()
{

	HRESULT    hr   = S_OK;

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
	// Setting the Listen mode to 
	// receive incoming calls event notifications
	//

	hr = StateListenMode( RTCLM_BOTH );
	
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}

/**********************************************************
//@ShutdownRTCApp()
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

HRESULT 
CRTCOutgoingDlg::ShutdownRTCApp()
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
//@RTC State Machine Methods for RTC Core Operations
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
CRTCOutgoingDlg::StateCoCreateClient()
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
CRTCOutgoingDlg::StateInitClient()
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);

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
//  either answering the call or disconnecting
//

//@Args In:     SESSION_OPERATION = {STO_CREATE | 
									 STO_ADDPARTY
									 STO_REMOVEPARTY
									 STO_TERMINATE}
//@Args Out:    HRESULT
//
//*********************************************************/


HRESULT 
CRTCOutgoingDlg::StateSessionControl(SESSION_OPERATION soType)
{

	HRESULT hr = S_OK;
	BSTR    bstrLocalURI  = NULL;
	BSTR    bstrURI       = NULL;

	_ASSERTE(NULL != this);

	switch(soType) 
	{

	case STO_CREATE:
		{

			//
			// Disable the Tunning Wizard upon session creation
			//

			StateTuneIDC(VARIANT_FALSE);

			if (m_SessionType == RTCST_PHONE_TO_PHONE)
			{

				//
				// A valid profile object is required to make Phone to Phone call, 
				// It is upto the reader to setup the infrastructure and create a 
				// proper profile based on their local settings. This code has been 
				// provided as an example. It will fail with proper error code.
				//

				//
				// Phone URI must be a Tel URI in 
				// canonical format eg: +12223334444
				//

				CString cstrLocalURI;
				m_editLURI.GetWindowText(cstrLocalURI);

				if (TRUE  == cstrLocalURI.IsEmpty())
				{
					LOGMSGERROR(_T("Set the Local URI for Phone to Phone Sessions"));
					return E_INVALIDARG;
				}
				else
				{
					bstrLocalURI = cstrLocalURI.AllocSysString();
					_ASSERTE(NULL != bstrLocalURI);
				}
			}

			hr = m_pIRTCClient->CreateSession( m_SessionType, 
											   bstrLocalURI, 
											   NULL,
											   0,
											   &m_pIRTCSession);

			FREE_BSTR_IF(bstrLocalURI);

			if ( FAILED(hr) )
			{

				LOGMSGERROR(_T("IRTCSession::CreateSession() Failed 0x%x"), hr);
				return hr;
			}



			CString cstrURI;

			m_editDURI.GetWindowText(cstrURI);

			if (TRUE == cstrURI.IsEmpty())
			{
				LOGMSGERROR(_T("Set the Destination URI"), hr);
				return hr;
			}
			else
			{

				bstrURI   = cstrURI.AllocSysString();

				_ASSERTE(NULL != bstrURI);

				hr = m_pIRTCSession->AddParticipant(bstrURI, 
													bstrURI, 
													&m_pIRTCParty);

				if ( FAILED(hr) )
				{

					LOGMSGERROR(_T("IRTCSession::AddParticipant() Failed 0x%x"), hr);
					return hr;
				}
				else
				{
					AddToParticipantList(bstrURI);
				}

				FREE_BSTR_IF(bstrURI);
			}

			break;
		}
	case STO_ADDPARTY:
		{

			CString cstrURI;

			m_editDURI.GetWindowText(cstrURI);

			if (TRUE == cstrURI.IsEmpty())
			{
				LOGMSGERROR(_T("Set the Party URI"), hr);
				return hr;
			}
			else
			{
				bstrURI   = cstrURI.AllocSysString();

				_ASSERTE(NULL != bstrURI);

				if (NULL != m_pIRTCParty)
				{
					m_pIRTCParty->Release();
					m_pIRTCParty = NULL;
				}

				hr = m_pIRTCSession->AddParticipant(bstrURI, 
													bstrURI, 
													&m_pIRTCParty);

				if ( FAILED(hr) )
				{

					LOGMSGERROR(_T("IRTCSession::AddParticipant() Failed 0x%x"), hr);
					return hr;
				}
				else
				{
					AddToParticipantList(bstrURI);
				}

				m_editDURI.SetWindowText(_T(""));
				FREE_BSTR_IF(bstrURI);
			}
			break;
		}
	case STO_REMOVEPARTY:
		{
			IRTCEnumParticipants *pIEnumParties = NULL;
			IRTCParticipant      *pIParty       = NULL;
			CString               cstrURI;
			BSTR                  bstrURI = NULL, bstrParty = NULL;
			
			INT lIndex = m_lbParties.GetCurSel();
			m_lbParties.GetText( lIndex, cstrURI);

			bstrURI = cstrURI.AllocSysString();

			DBLOGGER(_T("Removing %s\n"), bstrURI);

			hr = m_pIRTCSession->EnumerateParticipants(&pIEnumParties);

			if (FAILED(hr))
			{
			    LOGMSGERROR(_T("IRTCSession::EnumerateParticipants Failed 0x%x"), hr);
				FREE_BSTR_IF(bstrURI);
				return hr;
			}
			
			while (S_OK == (hr = pIEnumParties->Next(1, &pIParty, NULL)))
			{

				hr = pIParty->get_UserURI(&bstrParty);

				if (0 == _tcscmp(bstrParty ,  bstrURI))
				{
					break;
				}
				else
				{
					pIParty->Release();
					pIParty = NULL;

				}

				FREE_BSTR_IF(bstrParty);
			}

			FREE_BSTR_IF(bstrURI);

			if (NULL != pIParty)
			{
				hr = m_pIRTCSession->RemoveParticipant(pIParty);

				if (FAILED(hr))
				{
					 LOGMSGERROR(_T("IRTCSession::RemoveParticipant Failed 0x%x"), hr);
					 return hr;
				}
				else
				{
					m_lbParties.DeleteString(lIndex);
				}

				pIParty->Release();
				pIParty = NULL;
			}
			else
			{
				LOGMSGERROR(_T("Failed to Find Selected Participant"), hr);

			}

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
CRTCOutgoingDlg::StateListenMode(RTC_LISTEN_MODE lMode)
{

	HRESULT hr = S_OK;

	_ASSERTE(NULL != this);
	_ASSERTE(NULL != m_pIRTCClient);

	hr = m_pIRTCClient->put_ListenForIncomingSessions(lMode);

	if (FAILED(hr))
	{

		LOGMSGERROR(_T("IRTCClient::put_ListenForIncomingSessions Failed 0x%x"), 
			hr);
		return hr;
	}
	else
	{
		m_ctrlEditStatus.SetWindowText(_T("Ready ..."));
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
CRTCOutgoingDlg::StateQueryActiveMedia(LONG *plActiveMedia)
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
CRTCOutgoingDlg::StateSinkEvent(LONG lEventMask)
{

	HRESULT                   hr          = S_OK;
	IUnknown                  *pUnknown   = NULL;
	IConnectionPointContainer *pCPC       = NULL;
	IConnectionPoint          *pCP        = NULL;
	
	_ASSERTE(NULL != this);

	
	//
	// Right now I will actually override this.
	//

	lEventMask = RTCEF_CLIENT                     |
				 RTCEF_SESSION_STATE_CHANGE       |
				 RTCEF_MEDIA;

				
	hr = m_pIRTCClient->put_EventFilter(lEventMask);


	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IRTCClient::put_EventFilter Failed 0x%x"), 
			hr);

		return hr;
	}

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("InitializeSink() Failed 0x%x"), hr);
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

	


	hr = m_pIRTCClient->QueryInterface(IID_IConnectionPointContainer,
								   reinterpret_cast<void**> (&pCPC)
								   );

	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IRTCClient::QueryInterface(IConnectionPointContainer) Failed 0x%x"),
			hr);

		return hr;
	}

	
	hr = pCPC->FindConnectionPoint( IID_IRTCEventNotification, 
									&pCP );


	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("IConnectionPointContainer::FindConnectionPoint() Failed 0x%x"), 
			hr);

		pCPC->Release();
		return hr;
	}


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
CRTCOutgoingDlg::StateUnSinkEvent()
{

	HRESULT					   hr	      = S_OK;
	IUnknown                  *pUnknown   = NULL;
	IConnectionPointContainer *pCPC       = NULL;
	IConnectionPoint          *pCP        = NULL;
	

	hr = QueryInterface(IID_IUnknown,
					    reinterpret_cast<void**> (&pUnknown)
					   );

	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("QueryInterface(IUnknown) Failed 0x%x"), hr);
		return hr;
	}


	hr = m_pIRTCClient->QueryInterface(IID_IConnectionPointContainer,
								   reinterpret_cast<void**> (&pCPC)
								   );

	if (FAILED(hr)) 
	{
		LOGMSGERROR(_T("IRTCClient::QueryInterface(IConnectionPointContainer) Failed 0x%x"), hr);

		return hr;
	}

	
	hr = pCPC->FindConnectionPoint( IID_IRTCEventNotification, 
									&pCP );


	if (FAILED(hr)) 
	{

		LOGMSGERROR(_T("IConnectionPointContainer::FindConnectionPoint Failed 0x%x"), 
			hr);


		pCPC->Release();
		return hr;
	}


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
CRTCOutgoingDlg::ReleaseStateMachine()
{

	ULONG ulRef         = 0;
	BOOL  fRet          = FALSE;

	if ( NULL != m_pIRecvVideo )
	{

		ulRef = m_pIRecvVideo->Release();
		m_pIRecvVideo  = NULL;
	}

	if ( NULL != m_pIPrevVideo )
	{
		ulRef = m_pIPrevVideo->Release();
		m_pIRecvVideo = NULL;

	}

	if ( NULL != m_pIRTCSession )
	{
		ulRef = m_pIRTCSession->Release();
		m_pIRTCSession = NULL;
	}

	if ( NULL != m_pIRTCParty)
	{
		ulRef = m_pIRTCParty->Release();
		m_pIRTCParty = NULL;
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
CRTCOutgoingDlg::AddToMediaCapabilities(LONG lMedia, VARIANT_BOOL fPersist)
{

	HRESULT hr = S_OK;
	LONG    lGetMedia = 0;
	LONG    lSetMedia = 0;

	_ASSERTE(NULL != m_pIRTCClient);

	hr = m_pIRTCClient->get_MediaCapabilities(&lGetMedia);

	
	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::get_MediaCapabilities() Failed 0x%x"), hr);
		return hr;
	}
	else
	{

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
//@RemoveFromMediaCapabilities
//
//@Description
//  
// Removing Media Capabilities from Media
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCOutgoingDlg::RemoveFromMediaCapabilities(LONG lMedia, 
											 VARIANT_BOOL fPersist)
{
	HRESULT hr = S_OK;
	LONG    lGetMedia = 0;
	LONG    lSetMedia = 0;

	_ASSERTE(NULL != m_pIRTCClient);

	hr = m_pIRTCClient->get_MediaCapabilities(&lGetMedia);

	
	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::get_MediaCapabilities() Failed 0x%x"), hr);
		return hr;
	}
	else
	{
		//
		// are the media bits set
		//

		if ( (lGetMedia & lMedia) == lMedia ) 
		{

			lSetMedia = lGetMedia & (~lMedia);

		
			hr = m_pIRTCClient->SetPreferredMediaTypes(lSetMedia, fPersist);

			if (FAILED(hr))
			{
				LOGMSGERROR(_T("IRTCClient::SetPreferredMediaTypes(%d) Failed 0x%x"), 
					lSetMedia, hr);
				return hr;
			}

		}
	}

	return hr;
}

/**********************************************************
//@StateStartT120
//
//@Description
//  
// Start the T120 Applet
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCOutgoingDlg::StateStartT120(RTC_T120_APPLET tType)
{
	HRESULT hr = S_OK;

	_ASSERTE(NULL != m_pIRTCClient);

	hr = m_pIRTCClient->StartT120Applet(tType);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::StartT120Applet Failed 0x%x"), hr);
		return hr;
	}

	//
	// We dont have the Media Capability by Default
	// so if we are in connected state we add the stream
	//

	if (m_pIRTCSession != NULL)
	{
		RTC_SESSION_STATE SessionState;

		hr = m_pIRTCSession->get_State(&SessionState);

		if (FAILED(hr))
		{
			LOGMSGERROR(_T("IRTCSession::get_State Failed 0x%x"), hr);
			return hr;
		}


		if (RTCSS_CONNECTED  == SessionState)
		{

			hr = m_pIRTCSession->AddStream(RTCMT_T120_SENDRECV, 200);

			if (FAILED(hr))
			{

				LOGMSGERROR(_T("IRTCSession::AddStream(T120) Failed 0x%x"), hr);
				return hr;
			}
		}

	}

	return hr;
}

/**********************************************************
//@StateStartTunning
//
//@Description
//  Starts the Tunning Wizard
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/
HRESULT 
CRTCOutgoingDlg::StateStartTunning()
{
	HRESULT hr = S_OK;

	_ASSERTE(NULL != m_pIRTCClient);

	//
	// Send Parent Window
	//

	hr = m_pIRTCClient->InvokeTuningWizard( (OAHWND) m_hWnd);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCClient::InvokeTuningWizard Failed 0x%x"), hr);
		return hr;
	}
	
	return hr;
	
}
/**********************************************************
//@QueryInterface
//
//@Description
//  
//  QI
//
//@Args In:     None
//@Args Out:    None
//
//
//*********************************************************/

HRESULT	
CRTCOutgoingDlg::QueryInterface(REFIID iid, 
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
//@ RTCEventNotification Interface Implementations
//*********************************************************/

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
CRTCOutgoingDlg::AddRef()
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
CRTCOutgoingDlg::Release()
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
CRTCOutgoingDlg::Event(
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
// No Parsing
//
//@Args In:     
//@Args Out:    
//
//
//*********************************************************/

HRESULT 
CRTCOutgoingDlg::RTCClientEvent(IDispatch *pIDispatch)
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
//  Destructor
//
//@Args In:     
//@Args Out:    
//
//
//*********************************************************/
HRESULT 
CRTCOutgoingDlg::RTCSessionStateEvent(IDispatch *pIDispatch)
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
	

	hr = pISessionState->get_State(&SessionState);

	if (FAILED(hr))
	{
		LOGMSGERROR(_T("IRTCSessionStateChangeEvent::get_State Failed 0x%x"), hr);

		pISessionState->Release();
		pISessionState = NULL;
		return hr;
	}
		

	switch(SessionState)
	{
	case RTCSS_IDLE: break;
	case RTCSS_INCOMING:
	{
		m_ctrlEditStatus.SetWindowText(_T("Incoming Call..."));
		break;
	}
	case RTCSS_ANSWERING: 	
		m_ctrlEditStatus.SetWindowText(_T("Answering ..."));
		break;
	case RTCSS_INPROGRESS:
		m_ctrlEditStatus.SetWindowText(_T("In Progress ..."));
		break;
	case RTCSS_CONNECTED :
		{
			VARIANT_BOOL  fCanAdd  = VARIANT_FALSE;

			StateConnectIDC(VARIANT_FALSE);
			StateDisconnectIDC(VARIANT_TRUE);
			m_ctrlEditStatus.SetWindowText(_T("Connected ..."));

			//
			// Check if you can add participants to the session
			// enable the add participant button
			//
			hr = m_pIRTCSession->get_CanAddParticipants(&fCanAdd);

			if (FAILED(hr))
			{
				LOGMSGERROR(_T("IRTCSessionStateChangeEvent::get_State Failed 0x%x"), hr);
				return hr;
			}
			else if (VARIANT_TRUE == fCanAdd)
			{
				StateAddPartyIDC(VARIANT_TRUE);
				
				StateConnectIDCVisible(VARIANT_FALSE);
				StateAddPartyIDCVisible(VARIANT_TRUE);

				m_editDURI.SetWindowText(_T(""));
			}
			break;
		}
	case RTCSS_DISCONNECTED:
		{

			//
			// Disabling call-control related UI
			//

			StateDisconnectIDC(VARIANT_FALSE);
			StateConnectIDC(VARIANT_TRUE);
			StateConnectIDCVisible(VARIANT_TRUE);
			StatePartyListIDC(VARIANT_FALSE);
			StateTuneIDC(VARIANT_TRUE);

			StateAddPartyIDC(VARIANT_FALSE);
			StateRemovePartyIDC(VARIANT_FALSE);

			
			StateAddPartyIDCVisible(VARIANT_FALSE);

			//
			// Removing All Participants from the listbox
			//

			for (INT lIndex = 0; lIndex < m_lbParties.GetCount(); lIndex++)
			{
				m_lbParties.DeleteString(lIndex);
			}

			m_ctrlEditStatus.SetWindowText(_T("Disconnected ..."));
			m_ctrlEditStatus.SetWindowText(_T("Ready ..."));

			//
			// Releasing the Session Interface
			//

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
CRTCOutgoingDlg::RTCMediaEvent(IDispatch *pIDispatch)
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

				
				GetDlgItem( IDC_VRECVS, &hwdVideoRecv );

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

				GetDlgItem( IDC_VSENDS , &hwdVideoSend );

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

