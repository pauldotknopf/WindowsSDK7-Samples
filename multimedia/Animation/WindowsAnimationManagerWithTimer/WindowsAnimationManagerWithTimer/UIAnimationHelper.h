// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#ifndef __UIAnimationHelper_h__
#define __UIAnimationHelper_h__

#pragma once

#ifndef __ATLBASE_H__
	#error UIAnimationHelper.h requires ATLBase.h to be included first
#endif

#include <UIAnimation.h>

// Base class template for all UIAnimation callback base class templates
template <typename IUIAnimationInterface, typename CUIAnimationCallbackDerived>
class CUIAnimationCallbackBase :
    public CComObjectRootEx<CComSingleThreadModel>
{
public:

    // Creates an instance of CUIAnimationCallbackDerived and returns a pointer to its IUIAnimationInterface interface,
    // plus an optional pointer to the object itself.  Note that the latter is not AddRef'ed.
    static __checkReturn COM_DECLSPEC_NOTHROW HRESULT
    CreateInstance
    (
            __deref_out IUIAnimationInterface **ppUIAnimationInterface,
        __deref_opt_out CUIAnimationCallbackDerived **ppUIAnimationCallbackDerived = NULL
    )
    {
        CComObject<CUIAnimationCallbackDerived> *pUIAnimationCallbackDerived;
        HRESULT hr = CComObject<CUIAnimationCallbackDerived>::CreateInstance(&pUIAnimationCallbackDerived);
        if (FAILED(hr))
        {
            *ppUIAnimationInterface = NULL;
            
            if (ppUIAnimationCallbackDerived != NULL)
            {
                *ppUIAnimationCallbackDerived = NULL;
            }

            return hr;
        }
        
        CComQIPtr<IUIAnimationInterface> spUIAnimationInterface = pUIAnimationCallbackDerived;
        spUIAnimationInterface.CopyTo(ppUIAnimationInterface);

        if (ppUIAnimationCallbackDerived != NULL)
        {
            *ppUIAnimationCallbackDerived = pUIAnimationCallbackDerived;
        }
        
        return S_OK;
    }
};


/***************************************************************************************\

  Callback Base Class Templates

  To define a UIAnimation "callback object", simply derive a class from the appropriate
  template instantiation, e.g.:

    class CMyStoryboardEventHandler :
        public CUIAnimationStoryboardEventHandlerBase<CMyStoryboardEventHandler>
    {
        ...
        Implementations of IUIAnimationStoryboardEventHandler methods
        ...
    };

  Then, to create an instance of the class, call its static CreateInstance method: 

    CComPtr<IUIAnimationStoryboardEventHandler> spStoryboardEventHandler;
    IFR( CMyStoryboardEventHandler::CreateInstance(
        &spStoryboardEventHandler
        ) );
    m_spStoryboard->SetStoryboardEventHandler(
        spStoryboardEventHandler
        );

  An optional temporary class pointer can be used to initialize the object:

    CComPtr<IUIAnimationStoryboardEventHandler> spStoryboardEventHandler;
    CMyStoryboardEventHandler *pMyStoryboardEventHandler;
    IFR( CMyStoryboardEventHandler::CreateInstance(
        &spStoryboardEventHandler,
        &pMyStoryboardEventHandler
        ) );
    pMyStoryboardEventHandler->Initialize(this);
    m_spStoryboard->SetStoryboardEventHandler(
        spStoryboardEventHandler
        );

\***************************************************************************************/

// Base class template for implementation of IUIAnimationManagerEventHandler
template <typename CManagerEventHandlerDerived>
class CUIAnimationManagerEventHandlerBase :
    public CUIAnimationCallbackBase<IUIAnimationManagerEventHandler, CManagerEventHandlerDerived>,
    public IUIAnimationManagerEventHandler
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationManagerEventHandlerBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationManagerEventHandlerBase)
        COM_INTERFACE_ENTRY(IUIAnimationManagerEventHandler)
    END_COM_MAP()

public:

    // IUIAnimationManagerEventHandler

    // Handles OnManagerStatusChanged events, which occur when the animation manager's status changes
    IFACEMETHOD(OnManagerStatusChanged)
    (
        __in UI_ANIMATION_MANAGER_STATUS newStatus,                     // The new status
        __in UI_ANIMATION_MANAGER_STATUS previousStatus                 // The previous status
    ) PURE;
};

// Base class template for implementation of IUIAnimationVariableChangeHandler
template <typename CVariableChangeHandlerDerived>
class CUIAnimationVariableChangeHandlerBase :
    public CUIAnimationCallbackBase<IUIAnimationVariableChangeHandler, CVariableChangeHandlerDerived>,
    public IUIAnimationVariableChangeHandler
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationVariableChangeHandlerBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationVariableChangeHandlerBase)
        COM_INTERFACE_ENTRY(IUIAnimationVariableChangeHandler)
    END_COM_MAP()

public:

    // IUIAnimationVariableChangeHandler

    // Handles OnValueChanged events, which occur when an animation variable's value changes; receives value updates as DOUBLE
    IFACEMETHOD(OnValueChanged)
    (
        __in IUIAnimationStoryboard *storyboard,                        // The storyboard that is animating the variable
        __in IUIAnimationVariable *variable,                            // The animation variable that was updated
        __in DOUBLE newValue,                                           // The new value
        __in DOUBLE previousValue                                       // The previous value
    ) PURE;
};

// Base class template for implementation of IUIAnimationVariableIntegerChangeHandler
template <typename CVariableIntegerChangeHandlerDerived>
class CUIAnimationVariableIntegerChangeHandlerBase :
    public CUIAnimationCallbackBase<IUIAnimationVariableIntegerChangeHandler, CVariableIntegerChangeHandlerDerived>,
    public IUIAnimationVariableIntegerChangeHandler
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationVariableIntegerChangeHandlerBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationVariableIntegerChangeHandlerBase)
        COM_INTERFACE_ENTRY(IUIAnimationVariableIntegerChangeHandler)
    END_COM_MAP()

public:

    // IUIAnimationVariableIntegerChangeHandler

    // Handles OnIntegerValueChanged events, which occur when an animation variable's rounded value changes; receives value updates as INT32
    IFACEMETHOD(OnIntegerValueChanged)
    (
        __in IUIAnimationStoryboard *storyboard,                        // The storyboard that is animating the variable
        __in IUIAnimationVariable *variable,                            // The animation variable that was updated
        __in INT32 newValue,                                            // The new rounded value
        __in INT32 previousValue                                        // The previous rounded value
    ) PURE;
};

// Base class template for implementation of IUIAnimationStoryboardEventHandler
template <typename CStoryboardEventHandlerDerived>
class CUIAnimationStoryboardEventHandlerBase :
    public CUIAnimationCallbackBase<IUIAnimationStoryboardEventHandler, CStoryboardEventHandlerDerived>,
    public IUIAnimationStoryboardEventHandler
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationStoryboardEventHandlerBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationStoryboardEventHandlerBase)
        COM_INTERFACE_ENTRY(IUIAnimationStoryboardEventHandler)
    END_COM_MAP()

public:

    // IUIAnimationStoryboardEventHandler

    // Handles OnStoryboardStatusChanged events, which occur when a storyboard's status changes
    IFACEMETHOD(OnStoryboardStatusChanged)
    (
        __in IUIAnimationStoryboard *storyboard,                        // The storyboard that changed status
        __in UI_ANIMATION_STORYBOARD_STATUS newStatus,                  // The new status
        __in UI_ANIMATION_STORYBOARD_STATUS previousStatus              // The previous status 
    ) PURE;

    // Handles OnStoryboardUpdated events, which occur when a storyboard is updated
    IFACEMETHOD(OnStoryboardUpdated)
    (
        __in IUIAnimationStoryboard *storyboard                         // The storyboard that was updated
    ) PURE;
};

// Base class template for implementation of IUIAnimationPriorityComparison
template <typename CPriorityComparisonDerived>
class CUIAnimationPriorityComparisonBase :
    public CUIAnimationCallbackBase<IUIAnimationPriorityComparison, CPriorityComparisonDerived>,
    public IUIAnimationPriorityComparison
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationPriorityComparisonBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationPriorityComparisonBase)
        COM_INTERFACE_ENTRY(IUIAnimationPriorityComparison)
    END_COM_MAP()

public:

    // IUIAnimationPriorityComparison

    // Determines the relative priority between a scheduled storyboard and a new storyboard
    IFACEMETHOD(HasPriority)
    (
        __in IUIAnimationStoryboard *pStoryboardScheduled,              // Currently scheduled storyboard
        __in IUIAnimationStoryboard *pStoryboardNew,                    // New storyboard that conflicts with scheduled storyboard
        __in UI_ANIMATION_PRIORITY_EFFECT priorityEffect                // Potential effect on attempt to schedule storyboard if HasPriority returns S_FALSE
    ) PURE;
};

// Base class template for implementation of IUIAnimationInterpolator
template <typename CInterpolatorDerived>
class CUIAnimationInterpolatorBase :
    public CUIAnimationCallbackBase<IUIAnimationInterpolator, CInterpolatorDerived>,
    public IUIAnimationInterpolator
{
    DECLARE_NOT_AGGREGATABLE(CUIAnimationInterpolatorBase)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CUIAnimationInterpolatorBase)
        COM_INTERFACE_ENTRY(IUIAnimationInterpolator)
    END_COM_MAP()

public:

    // IUIAnimationInterpolator

    // Sets the interpolator's initial value and velocity
    IFACEMETHOD(SetInitialValueAndVelocity)
    (
        __in DOUBLE initialValue,                                       // The initial value
        __in DOUBLE initialVelocity                                     // The initial velocity
    ) PURE;

    // Sets the interpolator's duration
    IFACEMETHOD(SetDuration)
    (
        __in UI_ANIMATION_SECONDS duration                              // The interpolator duration
    ) PURE;

    // Gets the interpolator's duration
    IFACEMETHOD(GetDuration)
    (
        __out UI_ANIMATION_SECONDS *duration                            // The interpolator duration
    ) PURE;

    // Gets the final value to which the interpolator leads
    IFACEMETHOD(GetFinalValue)
    (
        __out DOUBLE *value                                             // The final value
    ) PURE;

    // Interpolates the value at a given offset
    IFACEMETHOD(InterpolateValue)
    (
         __in UI_ANIMATION_SECONDS offset,                              // The offset
        __out DOUBLE *value                                             // The interpolated value
    ) PURE;

    // Interpolates the velocity at a given offset
    IFACEMETHOD(InterpolateVelocity)
    (
         __in UI_ANIMATION_SECONDS offset,                              // The offset
        __out DOUBLE *velocity                                          // The interpolated velocity
    ) PURE;

    // Gets the interpolator's dependencies
    IFACEMETHOD(GetDependencies)
    (
        __out UI_ANIMATION_DEPENDENCIES *initialValueDependencies,      // The aspects of the interpolator that depend on its initial value
        __out UI_ANIMATION_DEPENDENCIES *initialVelocityDependencies,   // The aspects of the interpolator that depend on its initial velocity
        __out UI_ANIMATION_DEPENDENCIES *durationDependencies           // The aspects of the interpolator that depend on its duration
    ) PURE;
};


/***************************************************************************************\

  Helper Functions

\***************************************************************************************/

// Gets the tag of a variable, or (NULL, idDefault) if no tag was set
inline HRESULT
UIAnimation_GetVariableTag
(
               __in IUIAnimationVariable *pVariable,
               __in UINT32 idDefault,
    __deref_opt_out IUnknown **ppObject,
          __out_opt UINT32 *pId
)
{
    HRESULT hr = pVariable->GetTag(ppObject, pId);
    
    if (hr == UI_E_VALUE_NOT_SET)
    {
        if (ppObject)
        {
            *ppObject = NULL;
        }
        
        if (pId)
        {
            *pId = idDefault;
        }
        
        hr = S_OK;
    }

    return hr;
}

// Gets the tag of a storyboard, or (NULL, idDefault) if no tag was set
inline HRESULT
UIAnimation_GetStoryboardTag
(
               __in IUIAnimationStoryboard *pStoryboard,
               __in UINT32 idDefault,
    __deref_opt_out IUnknown **ppObject,
          __out_opt UINT32 *pId
)
{
    HRESULT hr = pStoryboard->GetTag(ppObject, pId);
    
    if (hr == UI_E_VALUE_NOT_SET)
    {
        if (ppObject)
        {
            *ppObject = NULL;
        }
        
        if (pId)
        {
            *pId = idDefault;
        }
        
        hr = S_OK;
    }

    return hr;
}

#endif // __UIAnimationHelper_h__
