//==============================================================================
//  OS/2 PM C++ Class Header File - "HumaNeko - A human 'Neko' for OS/2"
//
//  Application Window Class - this is a direct descendent of the general
//                             window class, Window.  This class is specific for
//                             the main window for a OS/2 PM application
//
//  Written By Eric A. Wolf
//  Last Modified: December 22, 1993
//==============================================================================

#ifndef APPWINDOWCLASS_H
#define APPWINDOWCLASS_H

extern VOID APIError(HWND hwndframe, HWND hwndclient);

class AppWindow : public Window
{
//------------------------------------------------------------------------------
//                         Private Class Variables
//------------------------------------------------------------------------------
private:
    static char szClassName[14];   // store the class name for this window
    static BOOL initialized;       // has the window been initialized?

//------------------------------------------------------------------------------
//                          Class Member functions
//------------------------------------------------------------------------------
public:
    AppWindow(void);               // class constructor

    static void Register(void);    // member to register a window class

    // define a function to vector messages to our app's message handler
    MRESULT WndProc(HWND hWnd, ULONG iMessage, MPARAM mParam1, MPARAM mParam2);

    ~AppWindow(void) {                // Application Window destructor
        WinDestroyWindow(hWndFrame);  // - call system to destroy window
        }
};

#endif

