/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: RTCIncoming.h
 * ---------- [ Header Definition File]
 * 
 *
 *
 ******************************************************/


#ifndef ___RTCINCOMING_H___
#define ___RTCINCOMING_H___



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/**********************************************************
//@Class CRTCIncomingApp
//
//@Description
//   
//  Main RTC APP
//
//
//*********************************************************/

class CRTCIncomingApp : public CWinApp
{
public:
	CRTCIncomingApp();

	//{{AFX_VIRTUAL(CRTCIncomingApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CRTCIncomingApp)
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // ___RTCINCOMING_H___
