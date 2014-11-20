// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#define STRICT

#include "MainWindow.h"

CComModule _Module;     // The global ATL module

const int FAILED_TO_ENTER_MESSAGE_LOOP = 0;

int Run(
    __in int nCmdShow
    )
{
    CMainWindow mainWindow;
    
    if (FAILED(mainWindow.Initialize()))
    {
        return FAILED_TO_ENTER_MESSAGE_LOOP;
    }
 
    if (mainWindow.Create(NULL, CWindow::rcDefault, TEXT("Windows Animation Manager With Timer")) == NULL)
    {
        return FAILED_TO_ENTER_MESSAGE_LOOP;
    }

    mainWindow.ShowWindow(
        nCmdShow
        );
    
    if (!mainWindow.UpdateWindow())
    {
        return FAILED_TO_ENTER_MESSAGE_LOOP;
    }
 
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(
            &msg
            );
        DispatchMessage(
            &msg
            );
    }

    return static_cast<int>(msg.wParam);
}

int WINAPI WinMain(__in HINSTANCE /* hInstance */, __in_opt HINSTANCE /* hPrevInstance */, __in LPSTR /* pszCmdLine */, __in int nCmdShow)
{
    int nResult = FALSE;

    ::CoInitialize(NULL);

    {   
        nResult = ::Run(
            nCmdShow
            );
    }
    
    ::CoUninitialize();

    return nResult;
}
