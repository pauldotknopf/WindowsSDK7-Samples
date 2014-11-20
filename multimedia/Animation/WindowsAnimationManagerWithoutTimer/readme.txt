Windows Animation Manager without a Timer Sample

Introduction
==============================
This sample shows how to use Windows Animation Manager without the Animation Timing, utilizing Direct2D for rendering the background color of a window and syncing to the refresh rate.

 
Sample Language Implementations
===============================
This sample is available in the following language implementations:
    C++

Files
===============================
* AnimationSample.h: Defines standard system include files, or project specific include files that are used frequently, but are changed infrequently
* MainWindow.h: The header file for the MainWindow class
* MainWindow.cpp: The implemenetation of the MainWindow class
* ManagerEventHandler.h: The implemenetation of the ManagerEventHandler
* UIAnimationHelper.h: The header file for various UIAnimation helper functions
* WindowsAnimationManagerWithoutTimer.cpp: Contains the application entry point
 
 
Prerequisites
===============================
Microsoft Windows® 7
Windows® Software Development Kit (SDK) for Windows 7 and .NET Framework 3.5 Service Pack 1: Beta release 


Building the Sample
===============================

To build the sample using the command prompt:
=============================================
    1. Open the Command Prompt window and navigate to the solution directory.
    2. Type msbuild

To run the application using the command prompt:
================================================
    1. Execute WindowsAnimationManagerWithoutTimer.exe

To use the application:
=======================
    1. Left click anywhere in the client area and the background color of window will animate to a random color


