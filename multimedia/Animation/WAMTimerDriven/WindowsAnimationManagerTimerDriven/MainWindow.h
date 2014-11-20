// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#pragma once

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <uianimation.h>

// Main application window

class CMainWindow
{
public:

    CMainWindow();
    ~CMainWindow();

    HRESULT Initialize(
        HINSTANCE hInstance
        );

    HRESULT Invalidate();

protected:

    HRESULT InitializeAnimation();
    HRESULT CreateAnimationVariables();

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
        );

    HRESULT OnPaint(
        const RECT &rcPaint
        );
    
    HRESULT OnLButtonDown();

    HRESULT DrawClientArea();

    HRESULT ChangeColor(
        DOUBLE red,
        DOUBLE green,
        DOUBLE blue
        );

    DOUBLE RandomFromRange(
        DOUBLE minimum,
        DOUBLE maximum
    );

private:

    HWND m_hwnd;

    // Animation components

    IUIAnimationManager *m_pManager;
    IUIAnimationTimer *m_pTimer;
    IUIAnimationTransitionLibrary *m_pTransitionLibrary;

    // Animated Variables

    IUIAnimationVariable *m_pVariableRed;
    IUIAnimationVariable *m_pVariableGreen;
    IUIAnimationVariable *m_pVariableBlue;
};




