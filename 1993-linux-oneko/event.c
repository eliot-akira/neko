/*
 * event.c
 */

#ifndef lint
static char     rcsid[] = "$Id: event.c,v 1.17 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>

#include "oneko.h"

#define	AVAIL_KEYBUF		255


/*
 * appears Koatatsu
 */

int
ClipInWindow(xp, yp, width, height, hot_x, hot_y)
int            *xp, *yp;
int             width, height;
int             hot_x, hot_y;
{
    int             ret = False;

    if (*yp <= hot_y) {
	*yp = hot_y;
	ret = True;
    } else if (*yp >= theRootHeight - (height - hot_y)) {
	*yp = theRootHeight - (height - hot_y);
	ret = True;
    }
    if (*xp <= hot_x) {
	*xp = hot_x;
	ret = True;
    } else if (*xp >= theRootWidth - (width - hot_x)) {
	*xp = theRootWidth - (width - hot_x);
	ret = True;
    }
    return ret;
}


/*
 * appears Koatatsu
 */

void
ToyAppear(toy, x, y)
Toy            *toy;
int             x, y;
{
    XWindowChanges  changes;

    SetToyState(toy, TOY_APPEAR);

    ClipInWindow(&x, &y, toy->width, toy->height, toy->hot_x, toy->hot_y);

    changes.x = toy->x = x;
    changes.y = toy->y = y;

    if (NoShape) {
	XCopyArea(theDisplay, theRoot, toy->save, SaveGC,
		  toy->x, toy->y, toy->width, toy->height, 0, 0);
    }
    XConfigureWindow(theDisplay, toy->window, CWX | CWY, &changes);
    XMapRaised(theDisplay, toy->window);
}


/*
 * disappears Koatatsu
 */

void
ToyDisappear(toy)
Toy            *toy;
{
    if (toy->state == TOY_YES) {
	SetToyState(toy, TOY_DISAPPEAR);
	XUnmapWindow(theDisplay, toy->window);
    }
}


/*
 * Expose event for Toy
 */

static void
ToyExpose(toy)
Toy            *toy;
{
    if (NoShape) {
	XCopyArea(theDisplay, toy->save, toy->window, SaveGC,
		  0, 0, toy->width, toy->height, 0, 0);
	XFlush(theDisplay);
    }
    XFillRectangle(theDisplay, toy->window, toy->gc,
		   0, 0, toy->width, toy->height);
}

NekoChangeVisibility(window)
Window window;
{
    Window          nekoKotastu[3];

    nekoKotastu[0] = theNeko;
    nekoKotastu[1] = Post.window;
    nekoKotastu[2] = Kotatsu.window;

    if (window == theNeko) {
	XRaiseWindow(theDisplay, theNeko);
	return;
    }
    ToyExpose(&Kotatsu);
    ToyExpose(&Post);
    XRestackWindows(theDisplay, nekoKotastu, 3);
}

/*
 * Key Events
 */

static Bool
ProcessKeyPress(keyEvent)
XKeyEvent      *keyEvent;
{
    int             length;
    char            key_buffer[AVAIL_KEYBUF + 1];
    KeySym          key_sym;
    XComposeStatus  compose_status;

    length = XLookupString(keyEvent, key_buffer, AVAIL_KEYBUF,
			   &key_sym, &compose_status);

    if (length > 0) {
	switch (key_buffer[0]) {
	case 'q':
	case 'Q':
	    if (keyEvent->state & Mod1Mask) {	/* META (Alt) Key */
		return False;
	    }
	    break;
	default:
	    break;
	}
    }

    return True;
}


/*
 * Events
 */

static Toy     *
WindowToToy(window)
Window          window;
{
    if (window == Kotatsu.window) {
	return &Kotatsu;
    } else if (window == Post.window) {
	return &Post;
    }

    return (Toy *) NULL;
}


/*
 * Events
 */

Bool
ProcessEvent()
{
    XEvent          event;
    Toy            *toy;
    Bool            continue_state = True;
    static Window   grabwindow = None;

    while (XCheckMaskEvent(theDisplay, ALL_EVENT_MASK, &event)) {
	switch (event.type) {
	case KeyPress:			/* Only theNeko */
	    continue_state = ProcessKeyPress((XKeyEvent *) & event);
	    if (!continue_state) {
		return continue_state;
	    }
	    break;
	case ButtonPress:
	    switch (event.xbutton.button) {
	    case Button1:
		if ((toy = WindowToToy(event.xbutton.window)) == NULL) {
		    if (event.xbutton.window != theNeko) {
			break;
		    }
		    toy = &Kotatsu;
		}
		grabwindow = toy->window;
		ToyDisappear(toy);
		if (XGrabPointer(theDisplay, theNeko, False,
				 ButtonPressMask |  ButtonReleaseMask,
				 GrabModeAsync, GrabModeAsync,
				 None, toy->cursor,
				 CurrentTime) != GrabSuccess) {
		    Warning("can't grab.");
		}
		break;
	    }
	    break;
	case ButtonRelease:
	    switch (event.xbutton.button) {
	    case Button1:
		if ((toy = WindowToToy(grabwindow)) != NULL) {
		    XUngrabPointer(theDisplay, CurrentTime);
		    ToyAppear(toy, event.xbutton.x_root - toy->hot_x,
			      event.xbutton.y_root - toy->hot_y);
		}
		break;
	    case Button2:
		if (&Kotatsu == WindowToToy(event.xbutton.window)) {
		    ToyDisappear(&Kotatsu);
		}
		break;
	    case Button3:
		RestoreCursor();
		break;
	    }
	    break;
	case Expose:			/* Only Toy.window */
	    while (XCheckTypedEvent(theDisplay, Expose, &event));
	    if ((toy = WindowToToy(event.xexpose.window)) != NULL) {
		ToyExpose(toy);
	    }
	    break;
	case VisibilityNotify:
	    if (AutoRaise && (event.xvisibility.state != VisibilityUnobscured))
		NekoChangeVisibility(event.xvisibility.window);
	    break;
	default:
	    /* Unknown Event */
	    break;
	}
    }

    return True;
}
