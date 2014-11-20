// rtcevents.h
//

#ifndef _RTCEVENTS_H_
#define _RTCEVENTS_H_

// Class to sink RTC events
class CRTCEvents :
	public IRTCEventNotification
{
private:
    DWORD m_dwRefCount;
    DWORD m_dwCookie;
    HWND  m_hWnd;

public:
    CRTCEvents() : m_dwRefCount(0),
                   m_dwCookie(0),
                   m_hWnd(NULL)
    {}

    /////////////////////////////////////////////
    //
    // QueryInterface
    // 

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject)
    {
        if (iid == IID_IRTCEventNotification)
        {
            *ppvObject = (void *)this;
            AddRef();
            return S_OK;
        }

        if (iid == IID_IUnknown)
        {
            *ppvObject = (void *)this;
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    /////////////////////////////////////////////
    //
    // AddRef
    // 

	ULONG STDMETHODCALLTYPE AddRef()
    {
        m_dwRefCount++;
        return m_dwRefCount;
    }
    
    /////////////////////////////////////////////
    //
    // Release
    // 

	ULONG STDMETHODCALLTYPE Release()
    {
        m_dwRefCount--;

        if ( 0 == m_dwRefCount)
        {
            delete this;
        }

        return 1;
    }

    /////////////////////////////////////////////
    //
    // Advise
    // 

    HRESULT Advise(IRTCClient * pClient, HWND hWnd)
    {    
	    IConnectionPointContainer * pCPC = NULL;
	    IConnectionPoint * pCP = NULL;
        HRESULT hr;

        // Find the connection point container
	    hr = pClient->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

	    if (SUCCEEDED(hr))
        {
            // Find the connection point
		    hr = pCPC->FindConnectionPoint(IID_IRTCEventNotification, &pCP);

            SAFE_RELEASE(pCPC);

            if (SUCCEEDED(hr))
            {
                // Advise the connection
		        hr = pCP->Advise(this, &m_dwCookie);

                SAFE_RELEASE(pCP);
            }
        }

        // Store the window handle of the application so we
        // can post messages to it when events are fired
        m_hWnd = hWnd;

	    return hr;
    }

    /////////////////////////////////////////////
    //
    // Unadvise
    // 

    HRESULT Unadvise(IRTCClient * pClient)
    {
	    IConnectionPointContainer * pCPC = NULL;
	    IConnectionPoint * pCP = NULL;
        HRESULT hr;

        if (m_dwCookie)
        {
            // Find the connection point container
	        hr = pClient->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

	        if (SUCCEEDED(hr))
            {
                // Find the connection point
		        hr = pCPC->FindConnectionPoint(IID_IRTCEventNotification, &pCP);

                SAFE_RELEASE(pCPC);

                if (SUCCEEDED(hr))
                {
                    // Unadvise this connection
		            hr = pCP->Unadvise(m_dwCookie);

                    SAFE_RELEASE(pCP);
                }
            }
        }

	    return hr;
    }

    /////////////////////////////////////////////
    //
    // Event
    // 

	HRESULT STDMETHODCALLTYPE Event(
        RTC_EVENT enEvent,
        IDispatch * pDisp
        )
    {
        // We will post a message containing the event to the
        // application window.
        
        // Add a reference to the event so we can hold onto it while
        // the event is in the message queue.
        pDisp->AddRef();

        // Post the message
        PostMessage( m_hWnd, WM_RTC_EVENT, (WPARAM)enEvent, (LPARAM)pDisp );

        return S_OK;
    }
};

#endif //__RTCTEST__