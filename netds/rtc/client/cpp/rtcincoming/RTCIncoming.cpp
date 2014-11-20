/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 *
 * @Abstract: RTCIncoming.cpp
 * ---------- [ Source Definition File]
 * 
 *
 *
 ******************************************************/


#include "stdafx.h"
#include "RTCIncoming.h"
#include "RTCIncomingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**********************************************************
//@Message Map for CRTCIncomingApp
//
//@Description
//  
//  This will Contain all the Event Handlers for 
//  this Class
//*********************************************************/

BEGIN_MESSAGE_MAP(CRTCIncomingApp, CWinApp)
	//{{AFX_MSG_MAP(CRTCIncomingApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/**********************************************************
//@CRTCIncomingApp()
//
//@Description
//  
//  Constructor 
//
//
//*********************************************************/
CRTCIncomingApp::CRTCIncomingApp()
{
	// No Initialization Code Required.
}


/**********************************************************
//@Class CRTCIncomingApp Instance
//
//
//*********************************************************/
CRTCIncomingApp theApp;


/**********************************************************
//@InitInstance
//
//@Description
//  
//  Initializer for the CRTCIncomingApp
//
//
//*********************************************************/
BOOL CRTCIncomingApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRTCIncomingDlg dlg;
	m_pMainWnd = &dlg;

	//
	// Starting the Main RTCIncoming Dialog Box
	// no response processing
	//
	int nResponse = dlg.DoModal();
	
	return FALSE;
}
