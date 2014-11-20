------------------------------------------------------------------------------

	Copyright (C) Microsoft Corporation. All rights reserved.

------------------------------------------------------------------------------


RTC Core 1.0

RTCIncoming SDK Sample

   
Overview:
=========
RTCIncoming uses the RTC Core API to receive, answer and disconnect from
incoming PC Calls. This sample was developed with Visual C++ 6.0

	
Building the sample:
====================

run "nmake" at the commandline. The RTCIncoming Sample builds and runs only
on the Windows XP Operating System.
		  

How to use Run the Sample:
==========================


- Start RTCIncoming.exe
- Make a Call from another machine to the machine running RTCIncoming.exe the
  answer button will be enabled once there is an incoming call.

- You make check the Auto Answer check box for RTCIncoming.exe to answer calls 
  automatically.

- if Video is supported on caller/callee machines a video receive and preview 
  window will be positioned in the RTCIncoming dialog box.

- When call is complete click the disconnect button to terminate the call
  or when other party disconnects RTCIncoming.exe call will be terminated automatically.



What functionality this sample does show:
=========================================

- Receiving and answering in coming calls using the RTC Core API using C++ 
  and demonstrates the usage of IVideoWindow.




What this sample does not show:
===============================

The sample does not demonstrate how to:

- Create sessions
- Handle presence or local provisioning
- use APPSHARING and WHITEBOARD



	
