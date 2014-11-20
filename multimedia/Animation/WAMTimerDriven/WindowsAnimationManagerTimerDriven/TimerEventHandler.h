// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//


#pragma once

#include "UIAnimationHelper.h"

// Event handler object for timer events

class CTimerEventHandler :
    public CUIAnimationTimerEventHandlerBase<CTimerEventHandler>
{
public:

    static COM_DECLSPEC_NOTHROW HRESULT
    CreateInstance
    (
        CMainWindow *pMainWindow,
        IUIAnimationTimerEventHandler **ppTimerEventHandler
    )
    {
        CTimerEventHandler *pTimerEventHandler;
        HRESULT hr = CUIAnimationCallbackBase::CreateInstance(
            ppTimerEventHandler,
            &pTimerEventHandler
            );

        if (SUCCEEDED(hr))
        {
            pTimerEventHandler->m_pMainWindow = pMainWindow;
        }
        
        return hr;
    }

    CTimerEventHandler()
      : m_pMainWindow(NULL)
    {
    }

    // IUIAnimationTimerEventHandler

    IFACEMETHOD(OnPreUpdate)
    (
    )
    {
        return S_OK;
    }

    IFACEMETHOD(OnPostUpdate)
    (
    )
    {
        HRESULT hr = m_pMainWindow->Invalidate();

        return hr;
    }

    IFACEMETHOD(OnRenderingTooSlow)
    (
        UINT32 /* fps */
    )
    {
        return S_OK;
    }

private:

     CMainWindow *m_pMainWindow;

};

