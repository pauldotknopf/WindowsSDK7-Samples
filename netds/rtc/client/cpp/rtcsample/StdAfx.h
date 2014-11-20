// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Uses Unicode character set
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE     
#define _UNICODE
#endif

// Application window defaults
#define APP_TITLE L"RTC Sample"
#define APP_CLASS L"RTCAPP"
#define APP_WIDTH  250
#define APP_HEIGHT 400

// IM window defaults
#define IM_TITLE  L"RTC Message"
#define IM_CLASS  L"RTCIM"
#define IM_WIDTH   400
#define IM_HEIGHT  400

// Audio/Video window defaults
#define AV_TITLE  L"RTC Audio/Video Call"
#define AV_CLASS  L"RTCAV"
#define AV_WIDTH   250
#define AV_HEIGHT  355

// String constants
#define MAX_STRING 256
#define MAX_XML_STRING 4096

// Video size constants
#define QCIF_CX_SIZE  176
#define QCIF_CY_SIZE  144
#define QQCIF_CX_SIZE (QCIF_CX_SIZE/2)
#define QQCIF_CY_SIZE (QCIF_CY_SIZE/2)

// Helper macros
#define SAFE_RELEASE(x) {if(x){(x)->Release();(x)=NULL;}}
#define SAFE_FREE_STRING(x) {if(x){SysFreeString(x);(x)=NULL;}}

// Includes
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <richedit.h>
#include <tchar.h>
#include <rtccore.h>
#include <rtcerr.h>
#include <string.h>
#include <vector>

#include "rtcwin.h"
#include "rtcevents.h"
#include "rtcdialogs.h"
#include "rtcsession.h"
#include "rtcim.h"
#include "rtcav.h"
#include "resource.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
