// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "MainWindow.h"
#include "SimpleTimerEventHandler.h"

/******************************************************************
*                                                                 *
*  CMainWindow::Initialize                                        *
*                                                                 *
*  Initialize Animation									          *
*                                                                 *
******************************************************************/
HRESULT CMainWindow::Initialize()
{
    HRESULT hr = S_OK;

    // Create the Animation Manager, Transition Library, and Animation Timer
    
    IFR( m_spManager.CoCreateInstance(  
        __uuidof(UIAnimationManager), 
        NULL, 
        CLSCTX_INPROC_SERVER
        ) );

    IFR( m_spTransitionLibrary.CoCreateInstance(  
        __uuidof(UIAnimationTransitionLibrary),
        NULL,
        CLSCTX_INPROC_SERVER
        ) );

    IFR( m_spTimer.CoCreateInstance( 
        __uuidof(UIAnimationTimer),
        NULL,
        CLSCTX_INPROC_SERVER
        ) );

    // Connect the animation manager to the timer
    // UI_ANIMATION_IDLE_BEHAVIOR_DISABLE tells the timer to shut itself off wh,en there is nothing to animate

    CComQIPtr<IUIAnimationTimerUpdateHandler> spTimerUpdateHandler(m_spManager);
    _ASSERT(spTimerUpdateHandler != NULL);
    IFR( m_spTimer->SetTimerUpdateHandler(
        spTimerUpdateHandler,
        UI_ANIMATION_IDLE_BEHAVIOR_DISABLE
        ) );

    // Create a timer event handler

    CComObject<CSimpleTimerEventHandler> *pSimpleTimerEventHandler;
    IFR( CComObject<CSimpleTimerEventHandler>::CreateInstance(
        &pSimpleTimerEventHandler
        ) );
    pSimpleTimerEventHandler->Initialize(
        this
        );
    CComQIPtr<IUIAnimationTimerEventHandler> spTimerEventHandler(pSimpleTimerEventHandler);
    _ASSERT(spTimerEventHandler);

    // Set the timer event handler

    IFR( m_spTimer->SetTimerEventHandler(
        spTimerEventHandler
        ) );

    // Create animation RGB variables

    IFR( m_spManager->CreateAnimationVariable(
        255,
        &m_spVariableRed
        ) );

    IFR( m_spManager->CreateAnimationVariable(
        255,
        &m_spVariableGreen
        ) );

    IFR( m_spManager->CreateAnimationVariable(
        255,
        &m_spVariableBlue
        ) );

    // Initial background color change

    IFR( ChangeColor(
        RandomFromRange(0, 255),
        RandomFromRange(0, 255),
        RandomFromRange(0, 255)
        ) );

    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::OnEraseBkgnd                                      *
*                                                                 *
*  Clears the background of the application window.               *
*                                                                 *
******************************************************************/

LRESULT CMainWindow::OnEraseBkgnd(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL &fHandled
    )
{
    fHandled = TRUE;
    return TRUE;
}

/******************************************************************
*                                                                 *
*  CMainWindow::OnPaint                                           *
*                                                                 *
*  Handles incoming WM_PAINT and WM_DISPLAYCHANGE messages        *
*                                                                 *
******************************************************************/

LRESULT CMainWindow::OnPaint(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL &fHandled
    )
{
    PAINTSTRUCT ps;
    BeginPaint(
        &ps
        );

    // Get the animation variable values

    INT32 red;
    m_spVariableRed->GetIntegerValue(
        &red
        );

    INT32 green;
    m_spVariableGreen->GetIntegerValue(
        &green
        );

    INT32 blue;
    m_spVariableBlue->GetIntegerValue(
        &blue
        );

    // Create brush to paint the background color

    HBRUSH hBrush = CreateSolidBrush(
        RGB( red, green, blue)
        );  

    HDC hDC = GetDC();

    RECT rect;
    GetClientRect(
        &rect
        );

    FillRect(
        hDC,
        &rect,
        hBrush
        );

    DeleteObject(
        hBrush
        );

    ReleaseDC(hDC);

    EndPaint(
        &ps
        );

    return FALSE;
}

/******************************************************************
*                                                                 *
*  CMainWindow::OnLButtonDown                                     *
*                                                                 *
*  Handles incoming WM_LBUTTONDOWN messages and changes the       *
*  background color of the window to a random color               * 
*                                                                 *
******************************************************************/

LRESULT CMainWindow::OnLButtonDown(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL &fHandled
    )
{
    HRESULT hr = S_OK;

    IFR( ChangeColor(
        RandomFromRange(0, 255),
        RandomFromRange(0, 255),
        RandomFromRange(0, 255)
        ) );

    Invalidate(
        FALSE
        );

    return FALSE;
}

/******************************************************************
*                                                                 *
*  CMainWindow::OnDestroy                                         *
*                                                                 *
*  Handles incoming WM_DESTORY messages and destroys the window   *               
*                                                                 *
******************************************************************/

LRESULT CMainWindow::OnDestroy(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL &fHandled
    )
{
    PostQuitMessage(
        FALSE
        );
    
    return FALSE;
}

/******************************************************************
*                                                                 *
*  CMainWindow::ChangeColor                                       *
*                                                                 *
*  Randomly animates an RGB value of the window background    	  *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::ChangeColor(
    DOUBLE red,
    DOUBLE green,
    DOUBLE blue
    )
{
    HRESULT hr = S_OK;
    const DOUBLE DURATION = 2.0;
    const DOUBLE ACCELERATION_RATIO = 0.2;
    const DOUBLE DECELERATION_RATIO = 0.8;

    // Create a new storyboard

    CComPtr<IUIAnimationStoryboard> spStoryboard;
    IFR( m_spManager->CreateStoryboard(&spStoryboard) );

    // Create an accelerate/decelerate transition over the red

    CComPtr<IUIAnimationTransition> spTransitionRed;    
    IFR( m_spTransitionLibrary->CreateAccelerateDecelerateTransition(
        DURATION,
        red,
        ACCELERATION_RATIO,
        DECELERATION_RATIO,
        &spTransitionRed
        ) );

    // Create an accelerate/decelerate transition over the green

    CComPtr<IUIAnimationTransition> spTransitionGreen;    
    IFR( m_spTransitionLibrary->CreateAccelerateDecelerateTransition(
        DURATION,
        green,
        ACCELERATION_RATIO,
        DECELERATION_RATIO,
        &spTransitionGreen
        ) );

    // Create an accelerate/decelerate transition over the blue

    CComPtr<IUIAnimationTransition> spTransitionBlue;    
    IFR( m_spTransitionLibrary->CreateAccelerateDecelerateTransition(
        DURATION,
        blue,
        ACCELERATION_RATIO,
        DECELERATION_RATIO,
        &spTransitionBlue
        ) );

    // Add RGB transitions

    IFR( spStoryboard->AddTransition(
        m_spVariableRed,
        spTransitionRed
        ) );

    IFR( spStoryboard->AddTransition(
        m_spVariableGreen,
        spTransitionGreen
        ) );

    IFR( spStoryboard->AddTransition(
        m_spVariableBlue,
        spTransitionBlue
        ) );

    // Schedule the storyboard to animate

    UI_ANIMATION_SECONDS timeNow;
    IFR( m_spTimer->GetTime(
        &timeNow
        ) );

    IFR( spStoryboard->Schedule(
        timeNow
        ) );

    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::RandomFromRange                                   *
*                                                                 *
*  Generates a random number between minimum and maximum value    *
*                                                                 *
******************************************************************/

DOUBLE CMainWindow::RandomFromRange(
    INT32 minimum,
    INT32 maximum
    )
{
     return minimum + (maximum - minimum) * (DOUBLE)rand() / RAND_MAX;
}
