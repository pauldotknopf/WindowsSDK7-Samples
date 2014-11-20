====================================================================================
ETW MANIFEST-BASED ADVANCED PROVIDER SAMPLE
====================================================================================

Sample Language Implementations
=================================================
   This sample is available in the following language implementations:
   C++.

FILES
=================================================
AdvancedProvider.cpp
   Main program. Calls generated macros in AdvancedProviderEvents.h to log event data. 

AdvancedProvider.man
   ETW manifest. Defines ETW provider, provider events and their payloads.

AdvancedProvider.rc
   Defines application version and manifest information.

makefile
   Defines build and compilation options. It is used to build the project from Microsoft Platform SDK build environment. This file contains scripts to build the executable. In particular, the definition has the following sections:
-	A script to run MC.EXE to generate header, message and resource files out of the provided manifest file.
-	A script to generate a RES file using RC.EXE.
-	A script to compile and link all output files.
-	A script to clean up the project directory.

Note: Output files are generated in a subfolder called “Output”.

RunE2E.cmd
   Command-line script to deploy and test the application. This script should be run from the project directory. Refer to the last section for details on this.


Tools used in this sample development
=================================================

- MC.EXE
     Used to generate resource files and event logging macros. Refer to Build Section for more details. 

- SDK Build Environment
     Used to build the project from console. Refer to Build section for more details.

- Visual Studio 2008
     May be used to build the project. Refer to Build #2 section for instructions if you are using Visual Studio.


BUILD
====================================
1. Using SDK Build Environment

Use nmake command to build the project under the  Microsoft Platform SDK build environment. Output files are created in a subfolder called “Output”.  This procedure uses the makefile definition to build the sample binaries. 


2. Using Visual Studio
  
Use the provided solution file to build the project from Visual Studio 2008. The Visual Studio project is configured to run MC.EXE automatically as a pre-build event to generate header and message resource files. This configuration step can be added as follows:
   
•	Right click on the Project 
•	Select Properties 
•	Select Build Events under Configuration Properties 
•	Select Pre-Build Events and then you will see the following script:  MC.EXE -um AdvancedProvider.man -h $(OutDir) -z AdvancedProviderEvents

Both build approaches generates the following files every time the project is built:

•	AdvancedProviderEvents.h - generated in a sub-directory called “Output” and is referenced by AdvancedProvider.cpp. It defines macros for ETW Event registration and EventWrites based on the provided manifest.
        Note: Visual Studio project is also configured to include this file from “Output”
•	AdvancedProviderEvents.rc - generated in the current directory.
•	AdvancedProviderEventsTEMP.BIN – generated in the current directory.
•	AdvancedProviderEvents_MSG00001.bin – generated in the current directory.

The next step is to create a RES file by using the provided AdvancedProvider.rc and MC generated message files.
A Post-Build Event is also used to clean up the project directory by deleting MC generated files.

INSTALL/DEPLOY, UNINSTALL and VIEWING/CONSUMPTION
===============================================
RunE2E.cmd has defined scripts to copy, install and run the sample on the same box where you ran the project. If you want to run the test on a separate box, make sure you follow the steps in the RUNE2E.cmd file.

The three important files to test the sample after a successful build are:
•	AdvancedProvider.exe
•	AdvancedProvider.man
•	RunE2E.cmd


More on RUNE2E.cmd. 

1.	Copy binary and manifest
md %systemdrive%\Eventing\Advanced\CPP >NUL 2>&1
copy Output\AdvancedProvider.exe %systemdrive%\Eventing\Advanced\CPP
copy AdvancedProvider.man %systemdrive%\Eventing\Advanced\CPP
pushd %systemdrive%\Eventing\Advanced\CPP

Because of the absolute path requirement for message file path and resource file path, this sample chooses “%systemdrive%\Eventing\Advanced\CPP” as install/deploy location for the executable and hence the build results should be copied from “Output” directory to “%systemdrive%\Eventing\Advanced\CPP” and then pushes the current directory to “%systemdrive%\Eventing\Advanced\CPP” so that the following scripts could be Advanced.

2.	Install the provider using Windows commandline tool wevtutil.exe
wevtutil im AdvancedProvider.man

3.	Start the logging session for AdvancedProvider.exe using Windows commandline tool logman.exe
logman start -ets AdvancedProvider -p "Microsoft-Windows-SDKSample-AdvancedProvider" 0 0 -o AdvancedProvider.etl 

4.	Execute the provider
             AdvancedProvider.exe

5.	Stop the provider session  
logman stop AdvancedProvider -ets

6.	Generate a dumpfile using Windows commandline tool tracerpt.exe
tracerpt -y AdvancedProvider.etl 

7.	Uninstall the provider
wevtutil um ..\AdvancedProvider.man

8.	Open dumpfile.xml
notepad dumpfile.xml

Note: You can also use event viewer to verify that the events for this provider have been logged.

