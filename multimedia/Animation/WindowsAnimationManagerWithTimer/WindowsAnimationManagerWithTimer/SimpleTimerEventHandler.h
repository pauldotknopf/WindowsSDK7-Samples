// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include <atlbase.h>
#include <uianimation.h>

// Event handler object for timer changes

class CSimpleTimerEventHandler : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public IUIAnimationTimerEventHandler
{
public:

    DECLARE_NOT_AGGREGATABLE(CSimpleTimerEventHandler)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CSimpleTimerEventHandler)
        COM_INTERFACE_ENTRY(IUIAnimationTimerEventHandler)
    END_COM_MAP()

    CSimpleTimerEventHandler()
      : _pWindow(NULL)
    {
    }

    void Initialize(CWindow *pWindow)
    {
        _pWindow = pWindow;
    }

    // IUIAnimationTimerEventHandler

    __checkReturn HRESULT __stdcall OnPreUpdate()
    {
        return S_OK;
    }

    __checkReturn HRESULT __stdcall OnPostUpdate()
    {
        _pWindow->InvalidateRect(
            NULL,
            false
            ); 

        return S_OK;
    }

    __checkReturn HRESULT __stdcall OnRenderingTooSlow(UINT32 /* fps */)
    {
        return S_OK;
    }
    
private:

    CWindow *_pWindow;
};
