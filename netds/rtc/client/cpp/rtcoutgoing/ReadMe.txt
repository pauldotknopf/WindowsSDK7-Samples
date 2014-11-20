------------------------------------------------------------------------------

	Copyright (C) Microsoft Corporation. All rights reserved.

------------------------------------------------------------------------------


RTC Core 1.0

RTCIncoming SDK Sample

   
Overview:
=========
RTCOutgoing uses the RTC Core API to create PC-PC , PC-PHONE, PHONE-PHONE
Sessions. This Sample also can allow T120 App Sharing in PC-PC Sessions and
Audio/Video Equipment Tuning. This Sample was developed using Visual C++ 6.0

	
Building the sample:
====================

run "nmake" at the commandline. The RTCOutgoing Sample builds and runs only
on the Windows XP Operating System.
		  

How to use Run the Sample:
==========================


- run RTCOutgoing.exe

- Select the Type of Session to be created or start the Tuning Wizard by clicking 
  the Tune button prior to making calls.

- You may also selected app sharing for a PC-PC Call

- When call is complete click the disconnect button to terminate the call or the 
  call will automatically be terminated if the callee has been disconnected.

- Particiapant list will hold the list of Participants in a Session. Multiple participants
  are only supported in Phone to Phone Session types. 


What functionality this sample does show:
=========================================

- Creating PCPC , PCPH, PHPH Sessions using the RTC Core 1.0
- Application Sharing
- Usage of tuning wizard
- RTC Eventing
- IVideoWindow 


What this sample does not show:
===============================

The sample does not demonstrate how to:

- Create IM (Instant Messaging) Sessions
- Handle presence , local provisioning or registration

	

   

		 
		  


  


