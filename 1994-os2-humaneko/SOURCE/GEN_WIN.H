//==============================================================================
//  OS/2 PM C++ Class Header File - "HumaNeko - A human 'Neko' for OS/2"
//
//  General Window Class - a generic window class from which other classes will
//                         be built
//
//  Written By Eric A. Wolf
//  Last Modified: December 22, 1993
//==============================================================================

#ifndef GENERALWINDOWCLASS_H
#define GENERALWINDOWCLASS_H

//==============================================================================
// define our global program constants here
//==============================================================================

#define NONE             0           // code for no movement for human
#define TIMER_ID         1           // define an ID for our application timer
#define MOVE_RIGHT       1           // code for human moving right
#define MOVE_LEFT        2           // code for moving left
#define MOVE_UP          3           // code for moving up
#define MOVE_UPRIGHT     4           // code for moving up and to the right
#define MOVE_UPLEFT      5           // code for moving up and to the left
#define MOVE_DOWN        6           // code for moving down
#define MOVE_DOWNRIGHT   7           // code for moving down and to the right
#define MOVE_DOWNLEFT    8           // code for moving down and to the left
#define TIMEFACTOR      25           // multiple of clock ticks for
#define TIMEUNTILWAIT   20           // # of HW_TIMER ticks before tapping toe
#define TIMEUNTILOCCUPY 60           // # of HW_TIMER ticks before doing yo-yo
#define XDIMENSION      48           // width of the human bitmap
#define YDIMENSION      48           // length of the human bitmap
#define BACKGROUNDCOLOR CLR_DARKGRAY // background color for application window


//==============================================================================
//  declare the Window class
//==============================================================================

class Window
{
//------------------------------------------------------------------------------
//                             Class Variables
//------------------------------------------------------------------------------
protected:
    HWND hWndFrame;         // handle to frame of current window
    HWND hWndClient;        // handle to client area of current window

//------------------------------------------------------------------------------
//                          Class Member functions
//------------------------------------------------------------------------------
public:
    HWND GetFrameHandle(void)       // return handle of application frame
           { return hWndFrame; }

    HWND GetClient(void)            // return handle of application client area
           { return hWndClient; }

    BOOL ShowWindow(int Show)       // change visibility of a window
           { return WinShowWindow(  // (if Show == TRUE, the window will be made
                    hWndFrame,      // visible, if == FALSE, then the window be
                    Show); }        // made not visible.)

    BOOL Update(void)
           {return WinUpdateWindow( // force a update of a given window
                    hWndFrame); }

    virtual MRESULT WndProc(        // virtual function that will direct all
                    HWND hWnd,      // incoming messages to appropriate function
                    ULONG iMessage, // handler
                    MPARAM mParam1,
                    MPARAM mParam2)
                    = 0;
};
#endif

