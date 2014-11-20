/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 * @Abstract: Utils.cpp
 * ---------- [ Source Definition File]
 * 
 * @Description:
 * -------------
 * Utilities for logging Error Messages in the debugger  
 * or in a Popup message box
 *
 ******************************************************/


#include "Stdafx.h"
#include "Utils.h"


/**********************************************************
//@DBLOGGER
//
//@Description
//  
//  Spews text output to the debugger
//
//@Args In:
//
//  LPCTSTR str   -> Formatted String
//  
//
//@Args Out:    None
//
//
//*********************************************************/
extern "C" int DBLOGGER(TCHAR *str, ...) 
{
	
	_ASSERTE( str != NULL);
	
	TCHAR buffer[512] = {_T('\0')};

	va_list v;
	va_start(v,str);
	_tcscpy_s(buffer,_countof(buffer),_T("\n"));
	_vsntprintf_s(buffer,_countof(buffer),_TRUNCATE,str,v);
	::OutputDebugString(buffer);

	va_end(v);
	return 1;
}


/**********************************************************
//@LOGMSGERROR
//
//@Description
//  
//  Reports an Error in a Message Box
//
//@Args In:
//
//  LPCTSTR str   ->  String
//  
//
//@Args Out:    None
//
//
//*********************************************************/
extern "C" void LOGMSGERROR(TCHAR *str, ...)
{

	_ASSERT(NULL != str);

	TCHAR buffer[512] = {_T('\0')};

	va_list v;
	va_start(v,str);
	_tcscpy_s(buffer,_countof(buffer),_T("\n"));
	_vsntprintf_s(buffer,_countof(buffer),_TRUNCATE,str,v);

	MessageBox(NULL, buffer, _T("RTCOut Error"), MB_OK);

	va_end(v);
}



/**********************************************************
//@LOGMSG
//
//@Description
//  
//  Reports an Error in a Message Box
//
//@Args In:
//
//  LPCTSTR str   ->  String
//  
//
//@Args Out:    None
//
//
//*********************************************************/
extern "C" void LOGMSG(TCHAR *str, ...)
{

	_ASSERT(NULL != str);

	TCHAR buffer[512] = {_T('\0')};

	va_list v;
	va_start(v,str);
	_tcscpy_s(buffer,_countof(buffer),_T("\n"));
	_vsntprintf_s(buffer,_countof(buffer),_TRUNCATE,str,v);

	MessageBox(NULL, buffer, _T("RTCOut Info"), MB_OK);

	va_end(v);
}