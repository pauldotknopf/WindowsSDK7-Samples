// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "MainWindow.h"
#include "ManagerEventHandler.h"

/******************************************************************
*                                                                 *
*  CMainWindow::~CMainWindow                                      *
*                                                                 *
*  Release all resources after they are no longer needed.         *
*                                                                 *
******************************************************************/

CMainWindow::~CMainWindow()
{
    DiscardDeviceResources();
}

/******************************************************************
*                                                                 *
*  CMainWindow::Initialize                                        *
*                                                                 *
*  Create device-independent resources for rendering D2D and      *
*  animation interfaces 									      *
*                                                                 *
******************************************************************/
HRESULT CMainWindow::Initialize()
{
    HRESULT hr = S_OK;

    IFR( CreateDeviceIndependentResources() );

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

    // Register manager event handler to kick start invalidating

    CManagerEventHandler *pManagerEventHandler;
    IFR( CManagerEventHandler::CreateInstance(
        &m_spManagerEventHandler,
        &pManagerEventHandler
        ) );
    pManagerEventHandler->Initialize(
        this
        );
    IFR( m_spManager->SetManagerEventHandler(
        m_spManagerEventHandler
        ) );

    // Create animation RGB variables

    IFR( m_spManager->CreateAnimationVariable(
        1.0,
        &m_spVariableRed
        ) );

    IFR( m_spManager->CreateAnimationVariable(
        1.0,
        &m_spVariableGreen
        ) );

    IFR( m_spManager->CreateAnimationVariable(
        1.0,
        &m_spVariableBlue
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
*  Handles incoming WM_PAINT and WM_DISPLAYCHANGE messages and    *
*  calls Render() to draw all elements on the application window. *
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

    (void)Render();

    EndPaint(
        &ps
        );

    return FALSE;
}

/******************************************************************
*                                                                 *
*  CMainWindow::OnResize                                          *
*                                                                 *
*  Handles incoming WM_SIZE messages, resizes the render target   *
*  appropriately, and calls ArrangeInRows.                        *               
*                                                                 *
******************************************************************/

LRESULT CMainWindow::OnResize(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL &fHandled
    )
{
    UINT width = LOWORD(lParam);
    UINT height = HIWORD(lParam);

    if (m_spRT)
    {
        D2D1_SIZE_U size = D2D1::SizeU(
            width,
            height
            );

        m_spRT->Resize(
            size
            );
    }

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
        RandomFromRange(0.0, 1.0),
        RandomFromRange(0.0, 1.0),
        RandomFromRange(0.0, 1.0)
        ) );

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
*  CMainWindow::Render                                            *
*                                                                 *
*  Calls RenderCore and if the window is occluded will discard    *
*  D2D device resources. This will also generate WM_PAINT         *
*  messages while the Animation Manager is busy to ensure that    *
*  animation are rendered.										  *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::Render()
{
    HRESULT hr = S_OK;
    UI_ANIMATION_SECONDS seconds;
    IFR( m_spTimer->GetTime(
        &seconds
        ) );
    IFR( m_spManager->Update(
        seconds
        ) );
    
    hr = RenderCore();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        DiscardDeviceResources();
    }
    else
    {
        IFR( hr );
    }

    UI_ANIMATION_MANAGER_STATUS status;
    m_spManager->GetStatus(
        &status
        );

    if (status == UI_ANIMATION_MANAGER_BUSY)
    {
        Invalidate(
            FALSE
            );
    }

    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::RenderCore                                        *
*                                                                 *
*  Calls CreateDeviceResources(), fills the background with a     *
*  gradient, and draws the directory text on the screen.          *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::RenderCore()
{
    HRESULT hr = S_OK;

    IFR( CreateDeviceResources() );

    if (!(m_spRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        D2D1_SIZE_F rtSize = m_spRT->GetSize();

        m_spRT->BeginDraw();

        // Get the RGB animation variable values

        DOUBLE red;
        m_spVariableRed->GetValue(
            &red
            );

        DOUBLE green;
        m_spVariableGreen->GetValue(
            &green
            );

        DOUBLE blue;
        m_spVariableBlue->GetValue(
            &blue
            );

        // Change the RGB of the background brush to new animated value

        m_spBackgroundBrush->SetColor(
            D2D1::ColorF(
                static_cast<FLOAT>(red),
                static_cast<FLOAT>(blue),
                static_cast<FLOAT>(green),
                1.0
                )
            );

        m_spRT->FillRectangle(
            D2D1::RectF(
                0,
                0,
                rtSize.width,
                rtSize.height
                ),
            m_spBackgroundBrush
            );

        hr =  m_spRT->EndDraw();
    }
    
    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::CreateDeviceIndependentResources                  *
*                                                                 *
*  Creates device indenpendent resources for D2D and              *
*  DWrite.														  *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create D2D factory

    IFR( D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_spD2DFactory
        ) );

    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::CreateDeviceResources                             *
*                                                                 *
*  Creates device resources for D2D and DWrite and calls          * 
*  FindImages to load bitmaps from thumbnail cache                *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_spRT)
    {
        CRect rect;
        GetClientRect(
            &rect
            );

        D2D1_SIZE_U size = D2D1::SizeU(
            rect.Width(),
            rect.Height()
            );

        // Create a D2D render target

        IFR( m_spD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                this->m_hWnd,
                size
                ),
            &m_spRT
            ) );

        // Create a brush for the background

        IFR( m_spRT->CreateSolidColorBrush(
            D2D1::ColorF(
                D2D1::ColorF::White
                ),
            &m_spBackgroundBrush
            ) );

        // Initial background color change

        IFR( ChangeColor(
            RandomFromRange(0.0, 1.0),
            RandomFromRange(0.0, 1.0),
            RandomFromRange(0.0, 1.0)
            ) );
    }

    return hr;
}

/******************************************************************
*                                                                 *
*  CMainWindow::DiscardDeviceResources                            *
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a D2D device is lost                                      *
*                                                                 *
******************************************************************/

void CMainWindow::DiscardDeviceResources()
{
    m_spRT.Release();
    m_spBackgroundBrush.Release();
}

/******************************************************************
*                                                                 *
*  CMainWindow::ChangeColor                                       *
*                                                                 *
*  Randomly animates an RGB value of the window background    	  *
*                                                                 *
******************************************************************/

HRESULT CMainWindow::ChangeColor(
    FLOAT red,
    FLOAT green,
    FLOAT blue
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

FLOAT CMainWindow::RandomFromRange(
    FLOAT minimum,
    FLOAT maximum
    )
{
     return minimum + (maximum - minimum) * (FLOAT)rand() / RAND_MAX;
}

