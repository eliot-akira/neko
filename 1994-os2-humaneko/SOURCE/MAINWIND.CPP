//==============================================================================
//  OS/2 PM C++ Class Member File - "HumaNeko - A human 'Neko' for OS/2"
//
//  MainWind - this class handles low level application initialization (anchor
//             block and message queue) and message dispatching
//
//  Written By Eric A. Wolf
//  Last Modified: December 23, 1993
//==============================================================================

#ifndef INCL_PM
  #define INCL_PM                  // include Presentation Manager definitions
  #include "os2.h"                 // include standard OS/2 definitions
#endif

#include "humaneko.h"              // include header file for resources
#include "mainwind.h"              // include class definition header file

#undef NULL                        // redefine NULL to be of type LONG
#define NULL 0L


//==============================================================================
//  define class member - Initialize - initializes anchor block and message
//                                     queue for our application
//==============================================================================

BOOL Main::Initialize(void) {
    // initialize anchor block for application
    if ((hab = WinInitialize(NULL)) == (HAB)NULL) return FALSE;
    // initialize message queue for primary thread of application
    if ((hmq = WinCreateMsgQueue(NULL,0)) == (HMQ)NULL) return FALSE;
    return TRUE;
}


//==============================================================================
//  define class member - MessageLoop - get messages and dispatch them to our
//                                      application as long as there are some
//==============================================================================

BOOL Main::MessageLoop(void) {
    BOOL rc;
    QMSG qmsg;
    // if there are messages in the queue, dispatch them to our application
    while ((rc = (BOOL)WinGetMsg(hab,&qmsg,(HWND)NULL,0,0))!=FALSE)
        WinDispatchMsg(hab,&qmsg);
    return rc;
}


//==============================================================================
//  define class member - Terminate - our application is done so destroy our
//                                    message queue and call system to terminate
//                                    our window and do clean up work
//==============================================================================

void Main::Terminate(void)
{
    // destroy our message queue and terminate the window
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );
}

