//==============================================================================
//  OS/2 PM C++ Class Member File - "HumaNeko - A human 'Neko' for OS/2"
//
//  Application Window Class - the bulk of the program code resides here.  It
//                             handles ALL the messages for the primary
//                             application thread
//
//  Written By Eric A. Wolf
//  Last Modified: December 27, 1993
//==============================================================================

#ifndef INCL_PM                  // include OS/2 PM definitions if not already
    #define INCL_PM              // included thus far
    #define INCL_DOSPROCESS
    #include <os2.h>
#endif

#undef NULL
#define NULL 0L                  // redefine NULL to be of type long

#include "HumaNeko.h"            // include Resource Workshop definitions
#include "gen_win.h"             // include general window class definitons
#include "mainwind.h"            // include main window class definitons
#include "AppWind.h"             // include application window class defs
#include <stdlib.h>              // included for rand() - random # generator

#define BLOCKED_LEFT 1           // codes for when HumaNeko is blocked
#define BLOCKED_RIGHT 2
#define BLOCKED_DOWN 3
#define BLOCKED_UP 4

MRESULT EXPENTRY WndProc(HWND hWnd, ULONG iMessage, MPARAM mParam1, MPARAM mParam2);
MRESULT EXPENTRY OptionsProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY GenericProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

char AppWindow::szClassName[14] = "HumaNeko v1.0";    // class name
BOOL AppWindow::initialized = FALSE;

//==============================================================================
// Registers the window class with OS/2
//==============================================================================
void AppWindow::Register(void)
{
    if (!WinRegisterClass(
        Main::hab,
        szClassName,
        ::WndProc,
        CS_MOVENOTIFY,
        sizeof(Window *)
        ))
            APIError(NULL,NULL);
        initialized = TRUE;
}

//==============================================================================
// make these global so they don't need to be reconstructed for each message
// If they don't need to be reconstructed everytime we receive a message
// (especially our common HW_TIMER message), we can optimize performance.
//==============================================================================
HPS    hps;
RECTL  rc;
POINTL pt;
POINTL pointerInfo, oldPointerInfo;
SWP windowInfo;
HBITMAP hbmCheck;

//==============================================================================
// message handler for main application
//==============================================================================
MRESULT AppWindow::WndProc(HWND hWnd, ULONG iMessage, MPARAM mParam1, MPARAM mParam2)
{
    switch( iMessage )
    {
        // ---------------------------------------------------------------------
        // if the window has moved or been resized, get its new location & size
        // and redraw the interior of the window (it may have been compromised)
        // ---------------------------------------------------------------------
        case WM_MOVE:
        case WM_SIZE:
        {
            Main::updateWindowPos = TRUE;
            WinInvalidateRect( hWnd, NULL, TRUE );
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        // ---------------------------------------------------------------------
        // if the window is being created (happens only at application start),
        // then we need to do some initialization procedures.  We set some
        // variables and start a timer so it sends a WM_TIMER message to our
        // application every so often so we know when it update the human
        // ---------------------------------------------------------------------
        case WM_CREATE:
        {
            Main::hWnd = hWnd;
            Main::appTimer = WinStartTimer( Main::hab, Main::hWnd, TIMER_ID, (24 - Main::UpdateFrequency)*TIMEFACTOR );
            Main::updateWindowPos = TRUE;
            Main::Location.x = 0;
            Main::Location.y = 0;
            Main::oldLocation = Main::Location;
            Main::SleepCount = 1;
            Main::Movement = NONE;
            Main::oldMovement = NONE;

            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        case WM_CONTROLPOINTER:
        {
            // DosBeep(200,200);
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        // ---------------------------------------------------------------------
        // The system has sent us a message telling us it is time to update the
        // human.  Here, we decide what play mode we're in (chase or runaway)
        // and then move and redraw the human accordingly
        // ---------------------------------------------------------------------
        case WM_TIMER:
        {
            // if the window has moved or changed size since our last time here,
            // get the new window coordinates
            if (Main::updateWindowPos == TRUE) {
                WinQueryWindowPos( Main::FramehWnd, (PSWP)&windowInfo);
                Main::updateWindowPos = FALSE;
                }

            oldPointerInfo = pointerInfo;
            WinQueryPointerPos( HWND_DESKTOP, &pointerInfo );
            pointerInfo.x = pointerInfo.x - windowInfo.x;
            pointerInfo.y = pointerInfo.y - windowInfo.y;
            if ((Main::SleepCount == 1) || (Main::SleepCount > TIMEUNTILWAIT) || (Main::oldLocation.x != Main::Location.x) || (Main::oldLocation.y != Main::Location.y)) {
                hps = WinGetPS( hWnd );
                if (Main::SleepCount == 0) {
                    // HumaNeko is on the move, so erase his previous
                    // position so he doesn't leave a trail
                    switch(Main::Movement) {
                        case MOVE_DOWNRIGHT: {
                            rc.xLeft = Main::oldLocation.x;
                            rc.xRight = Main::Location.x + XDIMENSION + 1;
                            rc.yBottom = Main::Location.y + YDIMENSION;
                            rc.yTop = Main::oldLocation.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            rc.xLeft = Main::oldLocation.x;
                            rc.xRight = Main::Location.x;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            }
                        case MOVE_DOWNLEFT: {
                            rc.xLeft = Main::Location.x;
                            rc.xRight = Main::oldLocation.x + XDIMENSION + 1;
                            rc.yBottom = Main::Location.y + YDIMENSION;
                            rc.yTop = Main::oldLocation.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            rc.xLeft = Main::Location.x + XDIMENSION;
                            rc.xRight = Main::oldLocation.x + XDIMENSION;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            }
                        case MOVE_UPRIGHT: {
                            rc.xLeft = Main::oldLocation.x;
                            rc.xRight = Main::Location.x + XDIMENSION + 1;
                            rc.yBottom = Main::oldLocation.y;
                            rc.yTop = Main::Location.y + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            rc.xLeft = Main::oldLocation.x;
                            rc.xRight = Main::Location.x;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            }
                        case MOVE_UPLEFT: {
                            rc.xLeft = Main::Location.x;
                            rc.xRight = Main::oldLocation.x + XDIMENSION + 1;
                            rc.yBottom = Main::oldLocation.y;
                            rc.yTop = Main::Location.y + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            rc.xLeft = Main::Location.x + XDIMENSION;
                            rc.xRight = Main::oldLocation.x + XDIMENSION;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            }
                        case MOVE_DOWN: {
                            rc.xLeft = Main::Location.x;
                            rc.xRight = Main::Location.x + XDIMENSION + 1;
                            rc.yBottom = Main::Location.y + YDIMENSION;
                            rc.yTop = Main::oldLocation.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            break;
                            }
                        case MOVE_UP: {
                            rc.xLeft = Main::Location.x;
                            rc.xRight = Main::Location.x + XDIMENSION + 1;
                            rc.yBottom = Main::oldLocation.y;
                            rc.yTop = Main::Location.y + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            break;
                            }
                        case MOVE_RIGHT: {
                            rc.xLeft = Main::oldLocation.x;
                            rc.xRight = Main::Location.x;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            break;
                            }
                        case MOVE_LEFT: {
                            rc.xLeft = Main::Location.x + XDIMENSION;
                            rc.xRight = Main::oldLocation.x + XDIMENSION;
                            rc.yBottom = Main::Location.y;
                            rc.yTop = Main::Location.y + YDIMENSION + 1;
                            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
                            break;
                            }
                        }
                    
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 132+(Main::Movement*4)+Main::animation, 0, 0);
                    }
                // Put in hbmCheck the handle to the correct bitmap cel    
                // for the animation we want to display
                else if ((Main::SleepCount > TIMEUNTILWAIT) && (Main::SleepCount < TIMEUNTILOCCUPY))
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 128+Main::animation, 0, 0);
                else if (Main::SleepCount > TIMEUNTILOCCUPY)
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 132+Main::animation, 0, 0);
                else
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 128, 0, 0);

                // if human is taunting user, get appropriate bitmap
                if (Main::Taunting > 0) {
                    GpiDeleteBitmap( hbmCheck );
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 168+Main::animation, 0, 0);
                    }
                // if human is surprised by user, get appropriate bitmap
                if (Main::Surprise > 0 ) {
                    GpiDeleteBitmap( hbmCheck );
                    hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 127, 0, 0);
                    }
                // if human is blocked by window frame, get appropriate bitmap
                if (Main::Blocked > 0) {
                    GpiDeleteBitmap( hbmCheck );
                    if (Main::SleepCount < TIMEUNTILWAIT)
                        hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 128, 0, 0);
                    else
                        hbmCheck = GpiLoadBitmap( hps, NULLHANDLE, 168+(Main::Blocked*4)+Main::animation, 0, 0);
                    Main::Blocked = 0;
                    }

                // if we have a cel (bitmap) to draw, do it, otherwise emit
                // an error tone to user
                if (hbmCheck != NULL)
                    WinDrawBitmap(hps, hbmCheck, NULL, &Main::Location, 0, 0, DBM_NORMAL);
                else
                    DosBeep(200,10);

                GpiDeleteBitmap( hbmCheck );

                // cycle to next cel of animation (we have 4 frames total)
                Main::animation = (Main::animation + 1) % 4;

                if ((Main::SleepCount == 1) || ((Main::SleepCount > TIMEUNTILWAIT) && (Main::SleepCount <= TIMEUNTILOCCUPY)))
                    Main::SleepCount = Main::SleepCount + 1;
                WinReleasePS( hps );
            }
            else {
                Main::SleepCount++;
                }

            Main::oldLocation = Main::Location;
            Main::oldMovement = Main::Movement;
            if (Main::PlayMode == PLAY_MODE_CHASE) {
                Main::Movement = NONE;
                //---------------------------------------
                // logic for catch-the-pointer play mode
                //---------------------------------------
                if (pointerInfo.x < Main::Location.x) {
                    Main::Location.x = Main::Location.x - Main::StepRate*2;
                    Main::Movement = MOVE_LEFT;
                    }
                else if (pointerInfo.x > Main::Location.x+48) {
                    Main::Location.x = Main::Location.x + Main::StepRate*2;
                    Main::Movement = MOVE_RIGHT;
                    }
                if (pointerInfo.y < Main::Location.y) {
                    Main::Location.y = Main::Location.y - Main::StepRate*2;
                    Main::Movement = Main::Movement + MOVE_DOWN;
                    }
                else if (pointerInfo.y > Main::Location.y+48) {
                    Main::Location.y = Main::Location.y + Main::StepRate*2;
                    Main::Movement = Main::Movement + MOVE_UP;
                    }
                if (Main::Location.x < 0) {
                    Main::Location.x = 0;
                    if (Main::Location.y==Main::oldLocation.y) {
                        Main::Blocked = BLOCKED_LEFT;
                        if (Main::SleepCount > TIMEUNTILOCCUPY)
                            Main::SleepCount = 1;
                        }
                    }
                if (Main::Location.y < 0) {
                    Main::Location.y = 0;
                    if (Main::Location.x==Main::oldLocation.x) {
                        Main::Blocked = BLOCKED_DOWN;
                        if (Main::SleepCount > TIMEUNTILOCCUPY)
                            Main::SleepCount = 1;
                        }
                    }
                if ((Main::Location.x+XDIMENSION) > (windowInfo.cx - 6)) {
                    Main::Location.x = windowInfo.cx - XDIMENSION - 6;
                    if (Main::Location.y==Main::oldLocation.y) {
                        Main::Blocked = BLOCKED_RIGHT;
                        if (Main::SleepCount > TIMEUNTILOCCUPY)
                            Main::SleepCount = 1;
                        }
                    }
                if ((Main::Location.y+YDIMENSION) > (windowInfo.cy - 54)) {
                    Main::Location.y = windowInfo.cy - YDIMENSION - 54;
                    if (Main::Location.x==Main::oldLocation.x) {
                        Main::Blocked = BLOCKED_UP;
                        if (Main::SleepCount > TIMEUNTILOCCUPY)
                            Main::SleepCount = 1;
                        }
                    }
                if (Main::Surprise > 0) {
                    Main::Location = Main::oldLocation;
                    Main::SleepCount = 1;
                    Main::Surprise--;
                    }

                if ((abs(pointerInfo.x - oldPointerInfo.x)<Main::MouseSensitivity) &&
                    (abs(pointerInfo.y - oldPointerInfo.y)<Main::MouseSensitivity) &&
                    (Main::SleepCount > TIMEUNTILOCCUPY)) {
                    Main::Location = Main::oldLocation;
                    }

                if ((Main::Location.x != Main::oldLocation.x) || (Main::Location.y!= Main::oldLocation.y)) {
                    if (Main::SleepCount > TIMEUNTILOCCUPY) {
                        Main::Surprise = 4;
                        Main::Location = Main::oldLocation;
                        Main::SleepCount = 1;
                        }
                    else
                        Main::SleepCount = 0;
                    }
                else
                    Main::Movement = NONE;
                }
            else {
                //-----------------------------
                // logic for run-away play mode
                //-----------------------------
                if (Main::Taunting > 0) {
                    Main::SleepCount = 1;
                    Main::Taunting--;
                    }
               else {
                Main::SleepCount = 0;
                if (Main::Duration == 0) {
                    Main::Duration = 5 + (rand() % 30);
                    Main::Movement = (rand() % 8);      // get rnd # 0 to 8
                    if (Main::Movement == 0) {
                        Main::Taunting = ((rand() % 3)+1)*4+4;
                        Main::Duration = 0;
                        }
                    }
                if (Main::Duration > 0) {
                    short move = Main::Movement;
                    Main::Duration--;
                    if (move >= MOVE_DOWN) {
                        Main::Location.y = Main::Location.y - Main::StepRate*2;
                        move = move - MOVE_DOWN;
                        }
                    if (move >= MOVE_UP) {
                        Main::Location.y = Main::Location.y + Main::StepRate*2;
                        move = move - MOVE_UP;
                        }
                    if (move == MOVE_LEFT) {
                        Main::Location.x = Main::Location.x - Main::StepRate*2;
                        }
                    if (move == MOVE_RIGHT) {
                        Main::Location.x = Main::Location.x + Main::StepRate*2;
                        }                    if (Main::Location.x < 0) {
                        Main::Location.x = 0;
                        Main::Duration = 0;
                        }
                    if (Main::Location.y < 0) {
                        Main::Location.y = 0;
                        Main::Duration = 0;
                        }
                    if ((Main::Location.x+XDIMENSION) > (windowInfo.cx - 6)) {
                        Main::Location.x = windowInfo.cx - XDIMENSION - 6;
                        Main::Duration = 0;
                        }
                    if ((Main::Location.y+YDIMENSION) > (windowInfo.cy - 54)) {
                        Main::Location.y = windowInfo.cy - YDIMENSION - 54;
                        Main::Duration = 0;
                        }
                    }
                 }
                }
            return 0;
        }
        // ---------------------------------------------------------------------
        // if our application has been closed, stop our timer before exiting
        // ---------------------------------------------------------------------
        case WM_CLOSE:
        {
            WinStopTimer( Main::hab, Main::hWnd, TIMER_ID );
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        // ---------------------------------------------------------------------
        // if the first mouse button has been clicked, check and see if the user
        // is trying to "catch the human" in runaway mode.
        // ---------------------------------------------------------------------
        case WM_BUTTON1DOWN:
        {
            if (Main::PlayMode == PLAY_MODE_RUN ) {
                if ((pointerInfo.x > Main::Location.x) &&
                    (pointerInfo.x < Main::Location.x+48) &&
                    (pointerInfo.y > Main::Location.y) &&
                    (pointerInfo.y < Main::Location.y+48)) {
                        WinMessageBox( HWND_DESKTOP, hWnd, "G O O D   J O B ! !\n\nBut can you catch the human again?\0", "HumaNeko - You caught him!!", 0, MB_MOVEABLE|MB_ICONEXCLAMATION|MB_OK );
                        }
                }
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        // ---------------------------------------------------------------------
        // intercept when a menuitem has been selected from our menubar.  If it
        // is one of the play modes, move the checkmark accordingly and change
        // the play-mode of the human
        // ---------------------------------------------------------------------
        case WM_MENUSELECT:
        {
            USHORT param1,param2;
            param2 = SHORT2FROMMP(mParam1);
            if (param2 == TRUE) {
                param1 = SHORT1FROMMP(mParam1);
                if (param1 == PLAY_MODE_CHASE) {
                    WinCheckMenuItem( HWNDFROMMP(mParam2), PLAY_MODE_CHASE, TRUE );
                    WinCheckMenuItem( HWNDFROMMP(mParam2), PLAY_MODE_RUN, FALSE );
                    Main::PlayMode = param1;
                    }
                else if (param1 == PLAY_MODE_RUN) {
                    WinCheckMenuItem( HWNDFROMMP(mParam2), PLAY_MODE_CHASE, FALSE );
                    WinCheckMenuItem( HWNDFROMMP(mParam2), PLAY_MODE_RUN, TRUE );
                    WinMessageBox( HWND_DESKTOP, hWnd, "Catch the human by moving the mouse\npointer over him and clicking\n\n             ...  if you can!!\0", "HumaNeko - Run Away", 0, MB_MOVEABLE|MB_NOICON|MB_OK );
                    Main::PlayMode = param1;
                    Main::Duration = 4;
                    Main::Movement = MOVE_UP;
                    }
                }
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
        }
        // ---------------------------------------------------------------------
        // if a portion of our window has been compromised and needs to be re-
        // painted, handle it here!  Note that only the background is drawn
        // because the human is drawn via the HW_TIMER message
        // ---------------------------------------------------------------------
        case WM_PAINT:
        {
            hps = WinBeginPaint( hWnd, (HPS)NULL, &rc);
            WinFillRect( hps, &rc, BACKGROUNDCOLOR );
            WinEndPaint( hps );
            break;
        }
        // ---------------------------------------------------------------------
        // something was selected off a menu, intercept and handle it here
        // ---------------------------------------------------------------------
        case WM_COMMAND:
        {
            USHORT command;
            command = SHORT1FROMMP(mParam1);
            switch( command )
            {
                // -------------------------------------------------------------
                // they need help, display our general help dialog box
                // -------------------------------------------------------------
                case HELP_GENERAL_HELP:
                {
                    WinDlgBox(HWND_DESKTOP,
                              hWnd,
                              (PFNWP)GenericProc,
                              0,
                              GENERAL_HELP,
                              (PVOID)NULL);
                    break;
                }
                // -------------------------------------------------------------
                // they selected product info, show them the prod. info dialog
                // -------------------------------------------------------------
                case HELP_PRODUCT_INFO:
                {
                    WinDlgBox(HWND_DESKTOP,
                              hWnd,
                              (PFNWP)GenericProc,
                              0,
                              PRODUCT_INFO,
                              (PVOID)NULL);
                    break;
                }
                // -------------------------------------------------------------
                // they want to change program settings, give them the options
                // dialog box (Note:  The options dialog WndProc message handler
                // (OptionsProc) is specified as the message handler)
                // -------------------------------------------------------------
                case OPTIONS_SETTINGS:
                {
                    WinDlgBox(HWND_DESKTOP,
                              hWnd,
                              (PFNWP)OptionsProc,
                              0,
                              OPTIONS_DIALOG,
                              (PVOID)NULL);
                    break;
                }
                // -------------------------------------------------------------
                // if user selected Exit, then post a message to our application
                // that it should close
                // -------------------------------------------------------------
                case OPTIONS_EXIT:
                {
                    WinSendMsg( hWnd, WM_SAVEAPPLICATION, (MPARAM)NULL, (MPARAM)NULL);
                    WinSendMsg( hWnd, WM_CLOSE, (MPARAM)NULL, (MPARAM)NULL);
                    break;
                }
                // -------------------------------------------------------------
                // if we got this far through the switch-case statement, the
                // WM_COMMAND message topic is one we don't care about, pass on
                // -------------------------------------------------------------
                default:
                    return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
            }
        }
        // ---------------------------------------------------------------------
        // if we got this far through the switch-case statement, the message is
        // one we don't care about, pass it onto the system to handle
        // ---------------------------------------------------------------------
        default:
            return WinDefWindowProc(hWnd,iMessage,mParam1,mParam2);
    }
    return FALSE;
}

//==============================================================================
// AppWindow constructor - this handles registering the window class for our
//                         application and creating a standard window for it
//                         through the parameters in flCreate
//==============================================================================
AppWindow::AppWindow(void)
{
    if (initialized == FALSE) Register();
    ULONG flCreate = FCF_MAXBUTTON|FCF_SIZEBORDER|FCF_ICON|FCF_MINBUTTON|FCF_AUTOICON|FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_BORDER|FCF_SHELLPOSITION|FCF_MENU;
    Main::tempthis = this;

    // try and create a standard window and put its frame value into hWndFrame
    if ((hWndFrame = WinCreateStdWindow(HWND_DESKTOP,0,&flCreate,szClassName,"",0,(HMODULE)NULL,MENUBAR,&hWndClient))==(HWND)NULL)
        APIError(hWndFrame, hWndClient);

    // make sure that the window is made visible!
    WinShowWindow( hWndFrame, TRUE );
}


//==============================================================================
//  Message handler for a generic information-only dialog box (help & prod info)
//==============================================================================
MRESULT EXPENTRY GenericProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch( msg )
    {
        // ---------------------------------------------------------------------
        // if we receive any system message, dismiss the dialog box
        // ---------------------------------------------------------------------
        case WM_COMMAND:
        {
            WinDismissDlg( hWnd, TRUE );
            return (MRESULT)0;
        }

        // ---------------------------------------------------------------------
        // if nothing further we want to intercept, pass message onto system
        // ---------------------------------------------------------------------
        default:
            return WinDefDlgProc( hWnd, msg, mp1, mp2 );
    }
}

//==============================================================================
//  Message handler for the Options... dialog box
//==============================================================================
MRESULT EXPENTRY OptionsProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    MRESULT MRet;

    switch( msg )
    {
        // ---------------------------------------------------------------------
        // intercept when dialog is being initialized so we can set the sliders
        // to the positions the user last had them at before the dialog is drawn
        // ---------------------------------------------------------------------
        case WM_INITDLG:
            {
            int index = 0;

            // add minor tick marks on all slider bars
            WinSendDlgItemMsg( hWnd, UPDATE_FREQUENCY, SLM_SETTICKSIZE, MPFROM2SHORT(SMA_SETALLTICKS,4),0);
            WinSendDlgItemMsg( hWnd, STEP_RATE, SLM_SETTICKSIZE, MPFROM2SHORT(SMA_SETALLTICKS,4),0);
            WinSendDlgItemMsg( hWnd, MOUSE_SENSITIVITY, SLM_SETTICKSIZE, MPFROM2SHORT(SMA_SETALLTICKS,4),0);

            // add major tick marks to each slider, respectively
            for (index=0; index <=24; index=index+2)
                WinSendDlgItemMsg( hWnd, UPDATE_FREQUENCY, SLM_SETTICKSIZE, MPFROM2SHORT(index,8),0);
            for (index=0; index <=24; index=index+8)
                WinSendDlgItemMsg( hWnd, STEP_RATE, SLM_SETTICKSIZE, MPFROM2SHORT(index,8),0);
            for (index=0; index <=24; index=index+6)
                WinSendDlgItemMsg( hWnd, MOUSE_SENSITIVITY, SLM_SETTICKSIZE, MPFROM2SHORT(index,8),0);

            // restore old positions of sliders
            WinSendDlgItemMsg( hWnd,  UPDATE_FREQUENCY, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),MPFROM2SHORT((Main::UpdateFrequency),0));
            WinSendDlgItemMsg( hWnd,  STEP_RATE, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),MPFROM2SHORT((Main::StepRate),0));
            WinSendDlgItemMsg( hWnd,  MOUSE_SENSITIVITY, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),MPFROM2SHORT((24 - Main::MouseSensitivity),0));

            // let system continue doing its initialization
            return WinDefDlgProc( hWnd, msg, mp1, mp2 );
        }

        // ---------------------------------------------------------------------
        // intercept when a control has been moved - by doing this, we can
        // have changes made in the options dialog immediately communicated back
        // to the still running primary application thread.  It will then allow
        // the user to see the changes they are making before committing to them
        // by selecting OK
        // ---------------------------------------------------------------------
        case WM_CONTROL:
        {
            USHORT controlID = SHORT1FROMMP(mp1);
            switch(controlID)
            {
                // -------------------------------------------------------------
                // intercept when update frequency slider has been moved
                // -------------------------------------------------------------
                case UPDATE_FREQUENCY:
                {
                    MRet = WinSendDlgItemMsg( hWnd,  UPDATE_FREQUENCY, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::UpdateFrequency = SHORT1FROMMP(MRet);
                    // stop current time and start a new timer with the new
                    // update frequency value the user has chosen
                    WinStopTimer( Main::hab, Main::hWnd, TIMER_ID );
                    Main::appTimer = WinStartTimer( Main::hab, Main::hWnd, TIMER_ID, (24 - Main::UpdateFrequency)*TIMEFACTOR );
                    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
                }
                // -------------------------------------------------------------
                // intercept when step rate slider has been moved
                // -------------------------------------------------------------
                case STEP_RATE:
                {
                    MRet = WinSendDlgItemMsg( hWnd,  STEP_RATE, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::StepRate = SHORT1FROMMP(MRet);
                    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
                }
                // -------------------------------------------------------------
                // intercept when mouse sensitivity slider has been moved
                // -------------------------------------------------------------
                case MOUSE_SENSITIVITY:
                {
                    MRet = WinSendDlgItemMsg( hWnd,  MOUSE_SENSITIVITY, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::MouseSensitivity = 24 - SHORT1FROMMP(MRet);
                    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
                }
                // -------------------------------------------------------------
                // something we don't care about has changed, pass message on...
                // -------------------------------------------------------------
                default:
                    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
            }
        }

        // ---------------------------------------------------------------------
        // intercept command messages from the system
        // ---------------------------------------------------------------------
        case WM_COMMAND:
        {
            USHORT command = SHORT1FROMMP(mp1);
            switch(command)
            {
                // -------------------------------------------------------------
                // intercept when OK button is pushed
                // -------------------------------------------------------------
                case DID_OK:
                {
                    MRESULT MRet;
                    short row, col;

                    // save the values of all the sliders in application
                    MRet = WinSendDlgItemMsg( hWnd,  MOUSE_SENSITIVITY, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::MouseSensitivity = 24 - SHORT1FROMMP(MRet);
                    MRet = WinSendDlgItemMsg( hWnd,  STEP_RATE, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::StepRate = SHORT1FROMMP(MRet);
                    MRet = WinSendDlgItemMsg( hWnd,  UPDATE_FREQUENCY, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),0);
                    Main::UpdateFrequency = SHORT1FROMMP(MRet);

                    // Just in case update frequency has changed, stop the
                    // current application timer and start a new one with the
                    // current update frequency time value
                    WinStopTimer( Main::hab, Main::hWnd, TIMER_ID );
                    Main::appTimer = WinStartTimer( Main::hab, Main::hWnd, TIMER_ID, (24 - Main::UpdateFrequency)*TIMEFACTOR );

                    // we got everything we needed from dialog, dismiss it!
                    WinDismissDlg( hWnd, TRUE );
                    return (MRESULT)0L;
                }
                // -------------------------------------------------------------
                // if no further system message to intercept, pass onto system
                // -------------------------------------------------------------
                default:
                    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
            }
        }
        // ---------------------------------------------------------------------
        // if nothing further we want to intercept, pass message onto system
        // ---------------------------------------------------------------------
        default:
            return WinDefDlgProc( hWnd, msg, mp1, mp2 );
    }
}

