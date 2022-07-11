//==============================================================================
// OS/2 PM C++ Program - "HumaNeko - A human 'Neko' for OS/2"
//
// Written By Eric A. Wolf - (C) 1993 - All Rights Reserved
//
// This source code may be used for reference ONLY!  It is provided AS-IS and no
// guarantees are made as to its utility, functionality or correctness.  It is
// provided solely as a guide to aid aspiring OS/2 2.x Presentation Manager
// programmers in developing their own PM applications.  No modifications are
// to be made to this code for re-release as a same or different product.  This
// code must be distributed (in its original entirety) with the executable
// portion of this product.
//
// Project Start Date:      December 10, 1993
// Project Completion Date: December 27, 1993
//
// Written using Borland C++ for OS/2, version 1.0, Borland Resource Workshop,
//               and the IBM OS/2 2.1 bitmap/icon editor
//
// File Last Modified:      December 27, 1993
//==============================================================================

#ifndef INCL_PM                  // include OS/2 PM definitions if not already
    #define INCL_PM              // included thus far
    #define INCL_DOSPROCESS
    #include <os2.h>
#endif

#undef NULL                      // define NULL in terms of LONG
#define NULL 0L

#include "humaneko.h"            // include header file for resources
#include "mainwind.h"            // include header for Main class definition
#include "appwind.h"             // include header for application window

//==============================================================================
// Declare API Error function prototype so we can use it in following functions
//==============================================================================
VOID APIError(HWND hwndFrame,HWND hwndClient);

//==============================================================================
// Declare our WndProc message handler as an exportable function
//==============================================================================
MRESULT EXPENTRY _export WndProc(HWND hWnd, ULONG iMessage, MPARAM mParam1, MPARAM mParam2);

//==============================================================================
// Initialize the variables in the main window class (mainwind.*) to defaults
//==============================================================================
HAB             Main::hab;
HMQ             Main::hmq;
Window          *Main::tempthis = NULL;
HWND            Main::hWnd = (HWND)0;
ULONG           Main::appTimer = 0;
int             Main::MouseSensitivity = 4;
int             Main::StepRate = 6;
int             Main::UpdateFrequency = 21;
int             Main::PlayMode = PLAY_MODE_CHASE;
BOOL            Main::updateWindowPos = TRUE;
POINTL          Main::Location;
POINTL          Main::oldLocation;
HWND            Main::FramehWnd = (HWND)0;
int             Main::SleepCount = 0;
int             Main::animation = 0;
int             Main::Movement = 0;
int             Main::oldMovement = 0;
int             Main::Duration = 0;
int             Main::Taunting = 0;
int             Main::Surprise = 0;
int             Main::Blocked = 0;


//==============================================================================
// main - this is the main module for our application.  It handle the
//        initialization of the window, going into a message loop for
//        dispatching messages and terminates the window upon successful exit
//        from the message loop
//==============================================================================
void cdecl main( void ) {
   // initialize the main instance of our application

   if (Main::Initialize()==FALSE)
      APIError((HWND)NULL,(HWND)NULL);
   {
        AppWindow MainWnd;               // initialize our application window
        Main::FramehWnd = MainWnd.GetFrameHandle();
        Main::MessageLoop();             // call our message handler for our app
   }
   Main::Terminate();                   // terminate the application
}


//==============================================================================
// WndProc - this function vectors WndProc (window processing) message handling
//           requests to the correct message handler
//==============================================================================
MRESULT EXPENTRY _export WndProc(HWND hWnd, ULONG iMessage, MPARAM mParam1, MPARAM mParam2)
{
    Window *pWindow = (Window *)WinQueryWindowULong(hWnd,0);
    if  (pWindow == 0) {
        if (iMessage == WM_CREATE) {
            pWindow = Main::tempthis;
            WinSetWindowULong(hWnd,0,(ULONG)pWindow);
            return pWindow->WndProc(hWnd,iMessage,mParam1,mParam2);
            }
        else return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
    else return pWindow->WndProc(hWnd,iMessage,mParam1,mParam2);
}


//==============================================================================
// APIError -- report an error to the user as returned from an API service.
//             This function is provided mainly as a debugging tool to report
//             API errors to the user during runtime
//==============================================================================
VOID APIError(HWND hwndFrame,HWND hwndClient)
{
      PERRINFO pErrInfoBlk;
      PSZ      pszOffSet;
      void     stdprint(void);
      PSZ      pszErrMsg;

      DosBeep(100,10);
      DosBeep(440,110);
      if ((pErrInfoBlk = WinGetErrorInfo(Main::hab)) != NULL)
      {
        pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
        pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
        if (hwndFrame && hwndClient)
            // if our window made it as far as definition, make the error window
            // owner be our application
            WinMessageBox(HWND_DESKTOP,      // Parent window is desk top
                       hwndFrame,            // Owner window is our frame
                       (PSZ)pszErrMsg,       // PMWIN Error message
                       "API Error Message",  // Title bar message
                       64,                   // Message identifier
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); // Flags
        else
            // if our window errored in the definition part, make the desktop
            // the owner of the error box (since we have no window to own it!)
            WinMessageBox(HWND_DESKTOP,      // Parent window is desk top
                       HWND_DESKTOP,         // Owner window is desktop
                       (PSZ)pszErrMsg,       // PMWIN Error message
                       "API Error Message",  // Title bar message
                       64,                   // Message identifier
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); // Flags
        // Discard the saved error info once we have used it
        WinFreeErrorInfo(pErrInfoBlk);
      }
      // upon fatal error, shutdown our application
      WinPostMsg(hwndClient, WM_QUIT, (ULONG)0, (ULONG)0);
}

