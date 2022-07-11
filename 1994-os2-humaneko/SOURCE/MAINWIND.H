//==============================================================================
//  OS/2 PM C++ Class Header File - "HumaNeko - A human 'Neko' for OS/2"
//
//  MainWind - this class handles low level application initialization (anchor
//             block and message queue) and message dispatching and it holds all
//             the variables that should be global to our application
//
//  Written By Eric A. Wolf
//  Last Modified: December 22, 1993
//==============================================================================

#ifndef MAINWINDOWCLASS_H
#define MAINWINDOWCLASS_H

#include "gen_win.h"                // include general window class definition

class Main
{
//------------------------------------------------------------------------------
//                          Class Member functions
//------------------------------------------------------------------------------
public:
    static BOOL Initialize(void);   // member for window initialization
                                    // sequence (initialize anchor and queue)
                                    // to initialize our application

    static BOOL MessageLoop(void);  // handle any messages given to main window

    static void Terminate(void);    // Close the window by killing message queue
                                    // and calling WinTerminate to close window

//------------------------------------------------------------------------------
//                          Public Class Variables
//------------------------------------------------------------------------------

    static HAB     hab;        // anchor block for application
    static HMQ     hmq;        // application message queue
    static HWND    hWnd;       // handle to application window (used by timer)
    static Window  *tempthis;  // Used to hold the "this" pointer that _export
                               // WndProc uses to vector to the correct member
                               // function WndProc
    static int MouseSensitivity;   // default mouse sensitivity
    static int StepRate;           // default step rate of character
    static int UpdateFrequency;    // update frequency of character
    static ULONG appTimer;         // holds unique id for our timer
    static int PlayMode;           // are we in catch or chase mode? var holder
    static POINTL Location;        // location of human in window
    static POINTL oldLocation;     // previous location of human in window
    static HWND FramehWnd;         // handle our main frame
    static BOOL updateWindowPos;   // update window position at timer tick?
    static int SleepCount;         // How long has the human been idle?
    static int animation;          // what frame of animation are we on? 0-3
    static int Movement;           // in what direction is the human moving?
    static int oldMovement;        // what was the human's previous movement
    static int Duration;           // time human goes before changing direction
    static int Taunting;           // length of time the human taunts user
    static int Surprise;           // length of time the human acts surprised
    static int Blocked;            // is the human blocked by the window frame
};

#endif

