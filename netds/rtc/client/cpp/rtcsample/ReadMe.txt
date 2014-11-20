========================================================================
        RTCSample Application (Using RTC Client API)  
========================================================================

   
Author:
=======
	Brian Ostergren

History:
========

	10-19-01  Created RTCSample

Sample Name:
=============

	RTCSample.exe


Sample Description:
=================

	RTCSample is a fully functional RTC application the uses the RTC
        Client API to expose Presence, Instant Messaging, and PC-to-PC
        audio/video call features.

	
Configurations & Setup:
=======================
      
        - OS       = Windows XP
        - Build    = Build all the files in Platform SDK Environment by
                     running nmake
		 

Building the Sample :
======================

- When building the sample you need to link with uuid.lib and set the include
  path to point the RTC Headers rtccore.h and rtcerr.h OR include in
  RTCSample.cpp rtccore_i.c (generated from running midl on rtccore.idl) for
  this you don't need to link with uuid.lib
		  

How To Run The Sample:
======================

- Start RTCSample.exe

- At the logon dialog, type a user URI in the form of user@domain and enter
  the address of your SIP proxy/registrar server.

- If you don't have a server you can cancel the logon dialog and still place
  peer-to-peer calls and instant messages.
