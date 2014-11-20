// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#include "MainWindow.h"
#include "TimerEventHandler.h"

#include <comdef.h>

// Simple macro to release non-null interfaces
#define _SafeRelease(p)     { if (NULL != p) { p->Release(); p = NULL; } }

// The class constructor

CMainWindow::CMainWindow() :
    m_hwnd(NULL)
{
    // Animation
    
    m_pManager = NULL;
    m_pTimer = NULL;
    m_pTransitionLibrary = NULL;

    // Animated Variables

    m_pVariableRed = NULL;
    m_pVariableGreen = NULL;
    m_pVariableBlue = NULL;
}

// The class destructor

CMainWindow::~CMainWindow()
{
    // Animation

    _SafeRelease(m_pManager);
    _SafeRelease(m_pTimer);
    _SafeRelease(m_pTransitionLibrary);

    // Animated Variables

    _SafeRelease(m_pVariableRed);
    _SafeRelease(m_pVariableGreen);
    _SafeRelease(m_pVariableBlue);
}

// Creates the CMainWindow window and initializes 
// device-independent resources.

HRESULT CMainWindow::Initialize(
    HINSTANCE hInstance                            
    )
{
    WNDCLASSEX wcex;
    HRESULT hr;
    ATOM atom;

    // Register the window class

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = CMainWindow::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = hInstance;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = TEXT("WAMMainWindow");
        wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    atom = RegisterClassEx(&wcex);

    hr = atom ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Create the CMainWindow window.

        m_hwnd = CreateWindow(
            TEXT("WAMMainWindow"),
            TEXT("Windows Animation - Timer-Driven Animation Demo"),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            hInstance,
            this
            );

        hr = m_hwnd ? S_OK : E_FAIL;
        if(SUCCEEDED(hr))
        {
            // Initialize Animation

            hr = InitializeAnimation();
            if(SUCCEEDED(hr))
            {
                hr = CreateAnimationVariables();
                if (SUCCEEDED(hr))
                {
                    // Display the window

                    ShowWindow(m_hwnd, SW_SHOWNORMAL);
                        
                    hr = UpdateWindow(m_hwnd);
                    if (SUCCEEDED(hr))
                    {
                        // Fade in with Red

                        hr = ChangeColor(255.0, 0.0, 0.0);
                    }
                }
            }
        }
    }

    return hr;
}

// Invalidates the client area for redrawing

HRESULT CMainWindow::Invalidate()
{
    BOOL bResult = InvalidateRect(
        m_hwnd,
        NULL,
        FALSE
        );

    HRESULT hr = bResult > 0 ? S_OK : E_FAIL;

    return hr;
}

// Creates and initializes the main animation components

HRESULT CMainWindow::InitializeAnimation()
{
    // Create Animation Manager

    HRESULT hr = CoCreateInstance(
            CLSID_UIAnimationManager,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pManager)
            );

    if (SUCCEEDED(hr))
    {
        // Create Animation Timer

        hr = CoCreateInstance(
            CLSID_UIAnimationTimer,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pTimer)
            );

        if (SUCCEEDED(hr))
        { 
            // Create Animation Transition Library

            hr = CoCreateInstance(
                CLSID_UIAnimationTransitionLibrary,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_pTransitionLibrary)
                );

            if (SUCCEEDED(hr))
            {
                // Connect the animation manager to the timer.
                // UI_ANIMATION_IDLE_BEHAVIOR_DISABLE tells the timer to shut itself
                // off when there is nothing to animate.

                IUIAnimationTimerUpdateHandler *pTimerUpdateHandler;
                hr = m_pManager->QueryInterface(
                   IID_PPV_ARGS(&pTimerUpdateHandler)
                    );

                if (SUCCEEDED(hr))
                {
                    hr = m_pTimer->SetTimerUpdateHandler(
                        pTimerUpdateHandler,
                        UI_ANIMATION_IDLE_BEHAVIOR_DISABLE
                        );
                    pTimerUpdateHandler->Release();

                    if (SUCCEEDED(hr))
                    {
                        // Create and set the Timer Event Handler

                        IUIAnimationTimerEventHandler *pTimerEventHandler;
                        hr = CTimerEventHandler::CreateInstance(
                            this,
                            &pTimerEventHandler
                            );

                        if (SUCCEEDED(hr))
                        {
                            hr = m_pTimer->SetTimerEventHandler(
                                pTimerEventHandler
                                );
                            pTimerEventHandler->Release();
                        }
                    }
                }
            }
        }
    }
    
    return hr;
}

// Creates the RGB animation variables for the background color

HRESULT CMainWindow::CreateAnimationVariables()
{
    const DOUBLE INITIAL_RED = 255.0;
    const DOUBLE INITIAL_GREEN = 255.0;
    const DOUBLE INITIAL_BLUE = 255.0;

    HRESULT hr = m_pManager->CreateAnimationVariable(
        INITIAL_RED,
        &m_pVariableRed
        );
    if (SUCCEEDED(hr))
    {
        hr = m_pManager->CreateAnimationVariable(
            INITIAL_GREEN,
            &m_pVariableGreen
            );
        if (SUCCEEDED(hr))
        {
            hr = m_pManager->CreateAnimationVariable(
                INITIAL_BLUE,
                &m_pVariableBlue
                );
        }
    }
    
    return hr;
}

// The window message handler

LRESULT CALLBACK CMainWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    const int MESSAGE_PROCESSED = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        CMainWindow *pCMainWindow = (CMainWindow *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pCMainWindow)
            );

        return MESSAGE_PROCESSED;
    }

    CMainWindow *pCMainWindow = reinterpret_cast<CMainWindow *>(static_cast<LONG_PTR>(
        ::GetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA
            )));

    if (pCMainWindow != NULL)
    {
        switch(message)
        {
        case WM_LBUTTONDOWN:
            {
                pCMainWindow->OnLButtonDown();
            }
            return MESSAGE_PROCESSED;

        case WM_PAINT:
        case WM_DISPLAYCHANGE:
            {
                PAINTSTRUCT ps;
                BeginPaint(
                    hwnd,
                    &ps
                    );

                pCMainWindow->OnPaint(
                    ps.rcPaint
                    );

                EndPaint(
                    hwnd,
                    &ps
                    );
            }
            return MESSAGE_PROCESSED;

        case WM_DESTROY:
            {
                PostQuitMessage(
                    0
                    );
            }
            return MESSAGE_PROCESSED;
        }
    }
    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
        );
}

//  Called whenever the client area needs to be drawn

HRESULT CMainWindow::OnPaint(const RECT &rcPaint)
{
    HRESULT hr = DrawClientArea();

    return hr;
}

// When the left mouse button is clicked on the client area, schedule
// animations to change the background color of the window

HRESULT CMainWindow::OnLButtonDown()
{
    HRESULT hr = ChangeColor(
        RandomFromRange(0.0, 255.0),
        RandomFromRange(0.0, 255.0),
        RandomFromRange(0.0, 255.0)
        );

    return hr; 
}

// Draws the contents of the client area

HRESULT CMainWindow::DrawClientArea()
{
    // Get the RGB animation variable values

    INT32 red;
    HRESULT hr = m_pVariableRed->GetIntegerValue(
        &red
        );

    if (SUCCEEDED(hr))
    {
        INT32 green;
        hr = m_pVariableGreen->GetIntegerValue(
            &green
            );

        if (SUCCEEDED(hr))
        {

            INT32 blue;
            hr = m_pVariableBlue->GetIntegerValue(
                &blue
                );

            if (SUCCEEDED(hr))
            {
                // Create brush to paint the background color

                HBRUSH hBrush = CreateSolidBrush(
                    RGB( red, green, blue )
                    );  

                HDC hDC = GetDC(
                    m_hwnd
                    );

                RECT rect;
                GetClientRect(
                    m_hwnd,
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

                ReleaseDC(
                    m_hwnd,
                    hDC
                    );
            }
        }
    }
    
    return hr;
}

// Animates the background color to a new value

HRESULT CMainWindow::ChangeColor(
    DOUBLE red,
    DOUBLE green,
    DOUBLE blue
    )
{
    const UI_ANIMATION_SECONDS DURATION = 0.5;
    const DOUBLE ACCELERATION_RATIO = 0.5;
    const DOUBLE DECELERATION_RATIO = 0.5;

    // Create a storyboard

    IUIAnimationStoryboard *pStoryboard = NULL;
    HRESULT hr = m_pManager->CreateStoryboard(
        &pStoryboard
        );
    if (SUCCEEDED(hr))
    {
        // Create transitions for the RGB animation variables

        IUIAnimationTransition *pTransitionRed;
        hr = m_pTransitionLibrary->CreateAccelerateDecelerateTransition(
            DURATION,
            red,
            ACCELERATION_RATIO,
            DECELERATION_RATIO,
            &pTransitionRed
            );
        if (SUCCEEDED(hr))
        {
            IUIAnimationTransition *pTransitionGreen;
            hr = m_pTransitionLibrary->CreateAccelerateDecelerateTransition(
                DURATION,
                green,
                ACCELERATION_RATIO,
                DECELERATION_RATIO,
                &pTransitionGreen
                );
            if (SUCCEEDED(hr))
            {
                IUIAnimationTransition *pTransitionBlue;
                hr = m_pTransitionLibrary->CreateAccelerateDecelerateTransition(
                    DURATION,
                    blue,
                    ACCELERATION_RATIO,
                    DECELERATION_RATIO,
                    &pTransitionBlue
                    );
                if (SUCCEEDED(hr))
                {
                    // Add transitions to the storyboard

                    hr = pStoryboard->AddTransition(
                        m_pVariableRed,
                        pTransitionRed
                        );
                    if (SUCCEEDED(hr))
                    {
                        hr = pStoryboard->AddTransition(
                            m_pVariableGreen,
                            pTransitionGreen
                            );

                        if (SUCCEEDED(hr))
                        {
                            hr = pStoryboard->AddTransition(
                                m_pVariableBlue,
                                pTransitionBlue
                                );
                            if (SUCCEEDED(hr))
                            {
                                // Schedule the storyboard with the current time

                                UI_ANIMATION_SECONDS secondsNow;
                                hr = m_pTimer->GetTime(
                                    &secondsNow
                                    );
                                if (SUCCEEDED(hr))
                                {
                                    hr = pStoryboard->Schedule(
                                        secondsNow
                                        );
                                }
                            }
                        }
                    }

                    pTransitionBlue->Release();
                }

                pTransitionGreen->Release();
            }

            pTransitionRed->Release();
        }

        pStoryboard->Release();
    }

    return hr;
}
// Generate a random value in the given range

DOUBLE CMainWindow::RandomFromRange(
    DOUBLE minimum,
    DOUBLE maximum
    )
{
     return minimum + (maximum - minimum) * (DOUBLE)rand() / RAND_MAX;
}
