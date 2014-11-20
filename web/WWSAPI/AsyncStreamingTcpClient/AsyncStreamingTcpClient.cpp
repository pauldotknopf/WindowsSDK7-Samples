//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

#ifndef UNICODE
#define UNICODE
#endif
#include "WebServices.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "PurchaseOrder.wsdl.h"

// Print out rich error info
void PrintError(HRESULT errorCode, WS_ERROR* error)
{
    wprintf(L"Failure: errorCode=0x%lx\n", errorCode);

    if (errorCode == E_INVALIDARG || errorCode == WS_E_INVALID_OPERATION)
    {
        // Correct use of the APIs should never generate these errors
        wprintf(L"The error was due to an invalid use of an API.  This is likely due to a bug in the program.\n");
        DebugBreak();
    }

    HRESULT hr = NOERROR;
    if (error != NULL)
    {
        ULONG errorCount;
        hr = WsGetErrorProperty(error, WS_ERROR_PROPERTY_STRING_COUNT, &errorCount, sizeof(errorCount));
        if (FAILED(hr))
        {
            goto Exit;
        }
        for (ULONG i = 0; i < errorCount; i++)
        {
            WS_STRING string;
            hr = WsGetErrorString(error, i, &string);
            if (FAILED(hr))
            {
                goto Exit;
            }
            wprintf(L"%.*s\n", string.length, string.chars);
        }
    }
Exit:
    if (FAILED(hr))
    {
        wprintf(L"Could not get error string (errorCode=0x%lx)\n", hr);
    }
}


struct SEND_STATE
{
    WS_CHANNEL* channel;
    WS_MESSAGE* message;
    ULONG messageCount;
    ULONG orderCount;
};

HRESULT CALLBACK Send1(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send2(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send3(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send4(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send5(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send6(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send7(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send8(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

HRESULT CALLBACK Send1(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);

    SEND_STATE* sendState = (SEND_STATE*)state;

    next->function = Send2;

    // Create a TCP duplex session channel
    hr = WsCreateChannel(
        WS_CHANNEL_TYPE_DUPLEX_SESSION, 
        WS_TCP_CHANNEL_BINDING, 
        NULL, 
        0, 
        NULL, 
        &sendState->channel, 
        error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    
    // Initialize address of service
    WS_ENDPOINT_ADDRESS address;
    address.url.chars = L"net.tcp://localhost/example";
    address.url.length = (ULONG)::wcslen(address.url.chars);
    address.headers = NULL;
    address.extensions = NULL;
    address.identity = NULL;
    
    // Open channel to address
    hr = WsOpenChannel(
        sendState->channel, 
        &address, 
        asyncContext, 
        error);
    if (FAILED(hr))
    {
        goto Exit;
    }
Exit:
    return hr;
}

HRESULT CALLBACK Send2(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);
    UNREFERENCED_PARAMETER(asyncContext);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    hr = WsCreateMessageForChannel(sendState->channel, NULL, 0, &sendState->message, error);
    if (FAILED(hr))
    {
        return hr;
    }

    next->function = Send3;
    sendState->messageCount = 0;
    return NOERROR;
}

HRESULT CALLBACK Send3(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    if (sendState->messageCount >= 100)
    {
        next->function = Send8;
        return NOERROR;
    }

    // Initialize message headers
    hr = WsInitializeMessage(sendState->message, WS_BLANK_MESSAGE, NULL, error);
    if (FAILED(hr))
    {
        return hr;
    }

    // Add the action header
    hr = WsSetHeader(
        sendState->message, 
        WS_ACTION_HEADER, 
        WS_XML_STRING_TYPE,
        WS_WRITE_REQUIRED_VALUE,
        PurchaseOrder_wsdl.messages.PurchaseOrder.action, 
        sizeof(*PurchaseOrder_wsdl.messages.PurchaseOrder.action),
        error);

    if (FAILED(hr))
    {
        return hr;
    }

    // Send the message headers
    sendState->orderCount = 0;
    next->function = Send4;
    return WsWriteMessageStart(sendState->channel, sendState->message, asyncContext, error);
}

HRESULT CALLBACK Send4(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Get the writer for the body
    WS_XML_WRITER* writer;
    hr = WsGetMessageProperty(sendState->message, WS_MESSAGE_PROPERTY_BODY_WRITER, &writer, sizeof(writer), error);
    if (FAILED(hr))
    {
        return hr;
    }

    if (sendState->orderCount >= 100)
    {
        next->function = Send6;
        return NOERROR;
    }

    // Initialize body data
    _PurchaseOrderType purchaseOrder;
    purchaseOrder.quantity = 1;
    purchaseOrder.productName = L"Pencil";

    // Write body data
    hr = WsWriteElement(
        writer, 
        &PurchaseOrder_wsdl.globalElements.PurchaseOrderType, 
        WS_WRITE_REQUIRED_VALUE,
        &purchaseOrder, 
        sizeof(purchaseOrder), 
        error);

    if (FAILED(hr))
    {
        return hr;
    }

    next->function = Send5;
    return WsFlushWriter(writer, 128, asyncContext, error);
}

HRESULT CALLBACK Send5(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);
    UNREFERENCED_PARAMETER(asyncContext);
    UNREFERENCED_PARAMETER(error);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    sendState->orderCount++;
    next->function = Send4;
    return NOERROR;
}

HRESULT CALLBACK Send6(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    // Send the end of the message
    next->function = Send7;
    return WsWriteMessageEnd(sendState->channel, sendState->message, asyncContext, error);
}

HRESULT CALLBACK Send7(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);
    UNREFERENCED_PARAMETER(asyncContext);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    // Reset message so it can be used again
    hr = WsResetMessage(sendState->message, error);
    if (FAILED(hr))
    {
        return hr;
    }

    sendState->messageCount++;
    next->function = Send3;
    return NOERROR;
}

HRESULT CALLBACK Send8(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state, WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(callbackModel);

    SEND_STATE* sendState = (SEND_STATE*)state;
    if (FAILED(hr))
    {
        return hr;
    }

    // Close the channel
    next->function = NULL;
    return WsCloseChannel(sendState->channel, asyncContext, error);
}

struct THREAD_INFO
{
    HRESULT hr;
    HANDLE handle;
};

static void CALLBACK OnSendComplete(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state)
{
    UNREFERENCED_PARAMETER(callbackModel);

    THREAD_INFO* threadInfo = (THREAD_INFO*)state;
    threadInfo->hr = hr;
    SetEvent(threadInfo->handle);
}

// Main entry point
int __cdecl wmain(int argc, __in_ecount(argc) wchar_t **argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    
    HRESULT hr = NOERROR;
    WS_ERROR* error = NULL;
    
    WS_ASYNC_STATE asyncState;
    SEND_STATE sendState;
    sendState.channel = NULL;
    sendState.message = NULL;
    sendState.messageCount = 0;
    sendState.orderCount = 0;
    
    THREAD_INFO threadInfo;
    threadInfo.hr = NOERROR;
    threadInfo.handle = NULL;
        
    
    // Create an error object for storing rich error information
    hr = WsCreateError(
        NULL, 
        0, 
        &error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    
    threadInfo.handle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (threadInfo.handle == NULL)
    {
        goto Exit;
    }
    
    WS_ASYNC_CONTEXT sendComplete;
    sendComplete.callback = OnSendComplete;
    sendComplete.callbackState = &threadInfo;
    
    hr = WsAsyncExecute(&asyncState, Send1, WS_LONG_CALLBACK, &sendState, &sendComplete, error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    
    if (hr == WS_S_ASYNC)
    {
        WaitForSingleObject(threadInfo.handle, INFINITE);
        hr = threadInfo.hr;
        if (FAILED(hr))
        {
            goto Exit;
        }
    }
        
Exit:
    if (FAILED(hr))
    {
        // Print out the error
        PrintError(hr, error);
    }
    fflush(
        stdout);
    
    if (threadInfo.handle != NULL)
    {
        CloseHandle(threadInfo.handle);
    }
        
    if (sendState.channel != NULL)
    {
        // Close the channel
        WsCloseChannel(sendState.channel, NULL, error);
    }
    if (sendState.message != NULL)
    {
        WsFreeMessage(sendState.message);
    }
    if (sendState.channel != NULL)
    {
        WsFreeChannel(sendState.channel);
    }
    if (error != NULL)
    {
        WsFreeError(error);
    }
    
    
    fflush(stdout);
    return SUCCEEDED(hr) ? 0 : -1;
}
