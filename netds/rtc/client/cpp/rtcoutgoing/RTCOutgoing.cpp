/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCOutgoing.cpp
 * ---------- [ Source Definition File]
 * 
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
//@CRTCOutGoingApp MessageMap
//
//*********************************************************/


BEGIN_MESSAGE_MAP(CRTCOutgoingApp, CWinApp)
	//{{AFX_MSG_MAP(CRTCOutgoingApp)

	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/**********************************************************
//@CRTCOutgoingApp()
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
CRTCOutgoingApp::CRTCOutgoingApp()
{
	//
	// No initialization required
	//
}

/**********************************************************
//@Declaration of the OutGoing App CRTCOutGoingApp
//
//
//*********************************************************/


CRTCOutgoingApp theApp;

/**********************************************************
//@InitInstance()
//
//@Description
//  
//  App Initializer
//
//@Args In:     Void
//@Args Out:    Void
//
//
//*********************************************************/
BOOL CRTCOutgoingApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRTCOutgoingDlg dlg;
	m_pMainWnd = &dlg;

	//
	// we do not handle the response from
	// the dialog  RTCIncomingDlg 
	//

	int nResponse = dlg.DoModal();


	return FALSE;
}
