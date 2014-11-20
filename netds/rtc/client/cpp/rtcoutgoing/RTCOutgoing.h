/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCOutgoing.h
 * ---------- [ Header Declaration File]
 * 
 *
 *
 ******************************************************/



#ifndef ___RTCOUTGOING_H____
#define ___RTCOUTGOING_H____



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif



#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRTCOutgoingApp:
// See RTCOutgoing.cpp for the implementation of this class
//

class CRTCOutgoingApp : public CWinApp
{
public:
	CRTCOutgoingApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTCOutgoingApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CRTCOutgoingApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // ___RTCOUTGOING_H____
