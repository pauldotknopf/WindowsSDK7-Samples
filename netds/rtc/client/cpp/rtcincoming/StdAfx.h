/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 *
 * @Abstract: Stdafx.h
 * ---------- [ PCH Header Definition File]
 * 
 *
 *
 ******************************************************/


#if !defined(AFX_STDAFX_H__DE9AFDFA_F750_4A20_B6CA_410D4C1C2666__INCLUDED_)
#define AFX_STDAFX_H__DE9AFDFA_F750_4A20_B6CA_410D4C1C2666__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



#include "process.h"
#include "rtccore.h"
#include "rtcerr.h"


#include <windows.h>

#include <atlbase.h>
#include "objbase.h"
#include <tchar.h>
#include "ocidl.h"
#include "process.h"
#include "crtdbg.h"




#endif // !defined(AFX_STDAFX_H__DE9AFDFA_F750_4A20_B6CA_410D4C1C2666__INCLUDED_)
