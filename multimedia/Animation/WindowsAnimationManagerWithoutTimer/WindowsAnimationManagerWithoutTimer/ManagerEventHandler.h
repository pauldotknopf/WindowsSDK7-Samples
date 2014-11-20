// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include <ATLBase.h>
#include <ATLWin.h>
#include <UIAnimation.h>
#include "UIAnimationHelper.h"
#include "AnimationSample.h"

// Event handler object for manager status changes

class CManagerEventHandler :
    public CUIAnimationManagerEventHandlerBase<CManagerEventHandler>
{
public:

    CManagerEventHandler()
      : m_pWindow(NULL)
    {
    }

    void Initialize(
        CWindow *pWindow
        )
    {
        m_pWindow = pWindow;
    }

    // IUIAnimationManagerEventHandler

    HRESULT __stdcall OnManagerStatusChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus
        )
    {
        if (newStatus == UI_ANIMATION_MANAGER_BUSY)
        {
            m_pWindow->Invalidate(
                FALSE
                );
        }

        return S_OK;
    }

private:

    CWindow *m_pWindow;
};
