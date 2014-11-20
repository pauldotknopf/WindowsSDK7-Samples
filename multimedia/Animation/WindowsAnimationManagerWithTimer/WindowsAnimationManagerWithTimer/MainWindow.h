// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>

#include <uianimation.h>

#include "AnimationSample.h"

class CMainWindow : public CWindowImpl<CMainWindow, CWindow, CFrameWinTraits>
{
public:

    DECLARE_WND_CLASS( TEXT("AnimationDemo Main Window Class") )

    HRESULT Initialize();

protected:
 
    BEGIN_MSG_MAP(CMainWindow)
        MESSAGE_HANDLER( WM_ERASEBKGND,     OnEraseBkgnd  )
        MESSAGE_HANDLER( WM_PAINT,          OnPaint       )
        MESSAGE_HANDLER( WM_DISPLAYCHANGE,  OnPaint       )
        MESSAGE_HANDLER( WM_LBUTTONDOWN,    OnLButtonDown )
        MESSAGE_HANDLER( WM_DESTROY,        OnDestroy     )
    END_MSG_MAP()

    LRESULT OnEraseBkgnd(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        BOOL &fHandled
        );

    LRESULT OnPaint(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        BOOL &fHandled
        );

    LRESULT OnLButtonDown(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        BOOL &fHandled
        );

    LRESULT OnDestroy(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        BOOL &fHandled
        );

    HRESULT ChangeColor(
        DOUBLE red,
        DOUBLE green,
        DOUBLE blue
        );

    DOUBLE RandomFromRange(
        INT32 minimum,
        INT32 maximum
        );
    
    // Animation objects

    CComPtr<IUIAnimationManager> m_spManager;  
    CComPtr<IUIAnimationTransitionLibrary> m_spTransitionLibrary;  
    CComPtr<IUIAnimationTimer> m_spTimer;

    // Animation variables

    CComPtr<IUIAnimationVariable> m_spVariableRed;
    CComPtr<IUIAnimationVariable> m_spVariableGreen;
    CComPtr<IUIAnimationVariable> m_spVariableBlue;
};
