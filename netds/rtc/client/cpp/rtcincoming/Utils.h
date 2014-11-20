
/******************************************************>
 * Copyright (c) 2001 Microsoft Corporation. 
 * All rights reserved.
 *
 *
 * @Abstract: Utils.h
 * ---------- [ Header Definition File]
 * 
 *
 * @Description:
 * -------------
 * Utilities for logging Error Messages in the debugger  
 * or in a Popup message box
 *
 *
 ******************************************************/

#ifndef __RTCINCOMINGUTILS__H__
#define __RTCINCOMINGUTILS__H__


#define FREE_BSTR_IF(bstrVal) \
	{ if (NULL != bstrVal) { SysFreeString(bstrVal); bstrVal = NULL ;}  else bstrVal = NULL; }


#ifdef __cplusplus
//
// for C Var length parameters
//
extern "C" {
#endif 

int  DBLOGGER(TCHAR *str, ...) ;
void LOGMSGERROR(TCHAR *str, ...);

#ifdef __cplusplus
}
#endif 



#endif