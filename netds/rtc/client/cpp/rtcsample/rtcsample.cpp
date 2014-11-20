// rtcsample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

/////////////////////////////////////////////
//
// WinMain
// 

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    HRESULT hr;

    // Initialize COM
    hr = CoInitialize(NULL);

    if (FAILED(hr))
    {
        // CoInitialize failed
        return 0;
    }
    
    // Initialize windows common controls
    InitCommonControls();

    // Load the rich edit library
    HMODULE hRichEdit;
    
    hRichEdit = LoadLibraryW(L"riched20.dll");

    if (hRichEdit == NULL)
    {
        // LoadLibrary failed
        return 0;
    }

    // Register the window classes
    CRTCWin::RegisterClass();
    CRTCIMSession::RegisterClass();
    CRTCAVSession::RegisterClass();

    // Create the main window
    HWND hWnd;

    hWnd = CreateWindowExW(
                0,
                APP_CLASS,
                APP_TITLE,
                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                CW_USEDEFAULT, CW_USEDEFAULT,
                APP_WIDTH, APP_HEIGHT,
                NULL,
                NULL,
                hInstance,
                NULL);

    if ( !hWnd )
    {  
        // CreateWindowExW failed
        return 0;
    }

    // Make the main window visible
    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    // Message loop
    BOOL bRet;
    MSG msg;

    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
            // GetMessage failed
            return 0;
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }

    // Free the rich edit library
    FreeLibrary(hRichEdit);

    // Shutdown COM
    CoUninitialize();

	return msg.wParam;
}