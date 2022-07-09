/*
 * oneko  -  Neko runs Over the windows.
 * 
 * oneko.c: main routines
 */

#ifndef	lint
static char     rcsid[] = "$Id: oneko.c,v 1.33 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif	/* SHAPE */

#include <stdio.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>

#include "oneko.h"
#include "state.h"


/*
 * get animation macro
 */

#define	GetAnimation()	(&AnimationTable[StatusTable[NekoState]. \
			 animation_index[(NekoTickCount / \
			 StatusTable[NekoState].flame_ticks) & 1]])

#define	GetStatusTable()	(&StatusTable[NekoState])


/*
 * Global variables
 */

Display        *theDisplay;		/* Pointer to Display structure */
Window          theRoot;		/* Window ID of Root Window */
Window          theNeko;		/* Window ID of Cat	 */

unsigned int    theRootWidth;		/* Width of Root Window	 */
unsigned int    theRootHeight;		/* Height of Root Window */


/*
 * Parameters (You can change also by Resource Parameters.)
 */
					/* Resource:		 */
long            IntervalTime;		/* time			 */
int             NekoSpeed;		/* speed		 */
int             IdleSpace;		/* idle			 */
Bool            NoShape;		/* noshape		 */
Bool            NoMouse;		/* nomouse		 */
Bool            SaveUnder;		/* saveunder		 */
Bool            BackingStore;		/* backingstore		 */
Bool            CheckMail;		/* mail			 */
Bool            AutoRaise;		/* autoraise		 */


/*
 * Status Parameter
 */

Bool            DontMapped = True;

int             NekoTickCount;		/* tick count for Neko	 */
int             NekoStateCount;		/* tick count for state	 */
int             NekoState;		/* status for Neko	 */

int             MouseX;			/* mouse pointer X	 */
int             MouseY;			/* mouse pointer Y	 */

int             PrevMouseX = 0;		/* last X		 */
int             PrevMouseY = 0;		/* last Y		 */

int             NekoX;			/* Neko X		 */
int             NekoY;			/* Neko Y		 */

int             NekoMoveDx;		/* Moving distance X	 */
int             NekoMoveDy;		/* Moving distance Y	 */

int             NekoLastX;		/* last Neko X		 */
int             NekoLastY;		/* last Neko Y		 */


/*
 * Interval
 */

static void
Interval()
{
    pause();
}


/*
 * tick count
 */

static void
TickCount()
{
    if (++NekoTickCount >= MAX_TICK) {
	NekoTickCount = 0;
    }
    if (NekoTickCount % 2 == 0) {
	if (NekoStateCount < MAX_TICK) {
	    NekoStateCount++;
	}
    }
}


/*
 * Neko status
 */

void
SetNekoState(SetValue)
int             SetValue;
{
    NekoTickCount = 0;
    NekoStateCount = 0;

    if (SetValue == NOTICE_STATE) {
	XRaiseWindow(theDisplay, theNeko);
    }
    NekoState = SetValue;
}


/*
 * save window into pixmap.
 */

static void
SavePixmap(x, y, anime, last_x, last_y, last_anime)
int             x, y;
int             last_x, last_y;
Animation      *anime, *last_anime;
{
    int             dist_x, dist_y;
    int             width, height;
    int             last_width, last_height;
    int             cross_width, cross_height;

    x -= anime->x_hot;
    y -= anime->y_hot;
    last_x -= last_anime->x_hot;
    last_y -= last_anime->y_hot;
    width = anime->width;
    height = anime->height;
    last_width = last_anime->width;
    last_height = last_anime->height;

    if (x > last_x) {
	dist_x = x - last_x;
	cross_width = last_width - dist_x;

	if (y > last_y) {
	    dist_y = y - last_y;
	    cross_height = last_height - dist_y;

	    XCopyArea(theDisplay, NekoSave, NekoSave, SaveGC,
		      dist_x, dist_y,
		      cross_width, cross_height,
		      0, 0);
	} else {
	    dist_y = last_y - y;
	    cross_height = height - dist_y;

	    XCopyArea(theDisplay, NekoSave, NekoSave, SaveGC,
		      dist_x, 0,
		      cross_width, cross_height,
		      0, dist_y);
	}
    } else {
	dist_x = last_x - x;
	cross_width = width - dist_x;

	if (y > last_y) {
	    dist_y = y - last_y;
	    cross_height = last_height - dist_y;

	    XCopyArea(theDisplay, NekoSave, NekoSave, SaveGC,
		      0, dist_y,
		      cross_width, cross_height,
		      dist_x, 0);
	} else {
	    dist_y = last_y - y;
	    cross_height = height - dist_y;

	    XCopyArea(theDisplay, NekoSave, NekoSave, SaveGC,
		      0, 0,
		      cross_width, cross_height,
		      dist_x, dist_y);
	}
    }
    if (y > last_y) {
	XCopyArea(theDisplay, theRoot, NekoSave, SaveGC,
		  x, last_y + last_height,
		  width, height - cross_height,
		  0, cross_height);
    } else {
	XCopyArea(theDisplay, theRoot, NekoSave, SaveGC,
		  x, y,
		  width, height - cross_height,
		  0, 0);
    }
    if (x > last_x) {
	XCopyArea(theDisplay, theRoot, NekoSave, SaveGC,
		  last_x + last_width, y,
		  width - cross_width, height,
		  cross_width, 0);
    } else {
	XCopyArea(theDisplay, theRoot, NekoSave, SaveGC,
		  x, y,
		  width - cross_width, height,
		  0, 0);
    }
}

/*
 * Drawing Neko
 */

static void
DrawNeko(x, y, anime)
int             x;
int             y;
Animation      *anime;
{
    static Animation *neko_last_anime = NULL;
    GC              neko_GC = anime->draw_GC;
    int             width = anime->width;
    int             height = anime->height;
    int             x_hot = anime->x_hot;
    int             y_hot = anime->y_hot;

    if ((x != NekoLastX) || (y != NekoLastY)
	|| ((neko_last_anime != NULL)
	&& (neko_GC != neko_last_anime->draw_GC))) {

#ifdef SHAPE
	if (NoShape == False) {
	    XShapeCombineMask(theDisplay, theNeko, ShapeBounding,
			      x - x_hot, y - y_hot,
			      anime->mask_pattern, ShapeSet);
	    if (DontMapped) {
		XMapRaised(theDisplay, theNeko);
		DontMapped = 0;
	    }
	    XSetTSOrigin(theDisplay, neko_GC, x - x_hot, y - y_hot);

	    XFillRectangle(theDisplay, theNeko, neko_GC,
			   x - x_hot, y - y_hot, width, height);
	} else
#endif	/* SHAPE */
	{
	    XWindowChanges  changes;

	    SavePixmap(x, y, anime, NekoLastX, NekoLastY, neko_last_anime);
	    changes.x = x - x_hot;
	    changes.y = y - y_hot;
	    XConfigureWindow(theDisplay, theNeko, CWX | CWY, &changes);
	    if (DontMapped) {
		XMapRaised(theDisplay, theNeko);
		DontMapped = 0;
	    }
	    XCopyArea(theDisplay, NekoSave, theNeko, SaveGC, 0, 0,
		      anime->width, anime->height, 0, 0);
	    XFillRectangle(theDisplay, theNeko, neko_GC, 0, 0, width, height);
	}
    }
    XFlush(theDisplay);

    NekoLastX = x;
    NekoLastY = y;

    neko_last_anime = anime;

    if (CheckMail) {
	MailCheck();
    }
}


/*
 * Start?
 */

static Bool
IsNekoMoveStart()
{
    if ((PrevMouseX >= MouseX - IdleSpace
	 && PrevMouseX <= MouseX + IdleSpace) &&
	(PrevMouseY >= MouseY - IdleSpace
	 && PrevMouseY <= MouseY + IdleSpace)) {
	return False;
    } else {
	return True;
    }
}


/*
 * get mouse (or Toys) point
 */

static void
GetPointer()
{
    Window          query_root, query_child;
    int             relative_x, relative_y;
    unsigned int    modkey_mask;

    PrevMouseX = MouseX;
    PrevMouseY = MouseY;

    if (Post.state == TOY_YES) {
	MouseX = Post.x + Post.hot_x;
	MouseY = Post.y + Post.hot_y;
    }
    else if (Kotatsu.state == TOY_YES) {
	MouseX = Kotatsu.x + Kotatsu.hot_x;
	MouseY = Kotatsu.y + Kotatsu.hot_y;
    }
    else {
	XQueryPointer(theDisplay, theNeko,
		      &query_root, &query_child,
		      &MouseX, &MouseY,
		      &relative_x, &relative_y,
		      &modkey_mask);
    }
}


/*
 * distance (dx, dy) to move
 */

static void
CalcDxDy()
{
    int             Dx, Dy;
    double          length;

    GetPointer();

    Dx = MouseX - NekoX;
    Dy = MouseY - NekoY;
    if ((Dx || Dy)
	&& (length = sqrt((double) (Dx * Dx + Dy * Dy))) > (double) NekoSpeed) {
	Dx = (int) ((double) (NekoSpeed * Dx) / length);
	Dy = (int) ((double) (NekoSpeed * Dy) / length);
    }
    NekoMoveDx = Dx;
    NekoMoveDy = Dy;
}


/*
 * Thinking .......
 */

static void
NekoThinkDraw()
{
    Animation      *animation;

    CalcDxDy();

    animation = GetAnimation();
    DrawNeko(NekoX, NekoY, animation);

    TickCount();

    switch (Post.state) {
    case TOY_APPEAR:
	SetNekoState(NOTICE_STATE);
	SetToyState(&Post, TOY_YES);
	Interval();
	return;
    case TOY_DISAPPEAR:
	SetNekoState(NOTICE_STATE);
	SetToyState(&Post, TOY_NO);
	Interval();
	return;
    default:
	switch (Kotatsu.state) {
	case TOY_APPEAR:
	    SetNekoState(NOTICE_STATE);
	    SetToyState(&Kotatsu, TOY_YES);
	    Interval();
	    return;
	case TOY_DISAPPEAR:
	    SetNekoState(NOTICE_STATE);
	    SetToyState(&Kotatsu, TOY_NO);
	    Interval();
	    return;
	}
    }

    if (StatusTable[NekoState].state_when_moved != NO_STATE) {
	if (IsNekoMoveStart()) {
	    SetNekoState(StatusTable[NekoState].state_when_moved);
	    goto end;
	}
    }

    if (StatusTable[NekoState].repeat_count != NO_TIME) {
	if (NekoStateCount < (StatusTable[NekoState].repeat_count
			      * StatusTable[NekoState].flame_ticks)) {
	    goto end;
	}
    }
    if (StatusTable[NekoState].action != NO_FUNC) {
	if ((*StatusTable[NekoState].action)(animation) == False) {
	    goto end;
	}
    }
    if (StatusTable[NekoState].next_state != NO_STATE) {
	SetNekoState(StatusTable[NekoState].next_state);
    }

end:
    Interval();
}


/*
 * Process Neko
 */

static void
ProcessNeko()
{
    struct itimerval value;
    Animation      *animation;

    /* Initialize Neko */

    SetNekoState(INITIAL_STATE);
    SetToyState(&Kotatsu, TOY_NO);

    animation = GetAnimation();

    NekoX = theRootWidth / 2;
    NekoY = theRootHeight / 2;

    if (NoShape) {
	XCopyArea(theDisplay, theRoot, NekoSave, SaveGC,
		  NekoX - animation->x_hot, NekoY - animation->y_hot,
		  animation->width, animation->height, 0,0);
    }

    NekoLastX = NekoX;
    NekoLastY = NekoY;

    /* Initialize timer */

    timerclear(&value.it_interval);
    timerclear(&value.it_value);

    value.it_interval.tv_usec = IntervalTime;
    value.it_value.tv_usec = IntervalTime;

    (void) setitimer(ITIMER_REAL, &value, (struct itimerval *) 0);

    /* main loop */

    do {
	NekoThinkDraw();
    } while (ProcessEvent());
}


/*
 * SIGALRM
 */

static void
NullFunction()
{
#if (defined(SYSV) && !defined(BE)) || defined(_AIX)
    (void) signal(SIGALRM, NullFunction);	/* restart alarm */
#endif
    /* No Operation */
}


/*
 * SIGINT, SIGQUIT, SIGTERM
 */

void
RestoreCursor()
{
    XSetWindowAttributes neko_attributes;

    if (NoMouse != True) {
	neko_attributes.cursor = None;
	XChangeWindowAttributes(theDisplay, theRoot, CWCursor,
				&neko_attributes);
    }
    XCloseDisplay(theDisplay);
    exit(0);
}


/*
 * Main()
 */

int
main(argc, argv)
int             argc;
char           *argv[];
{
    GetResources(argc, argv);
    if (CheckMail) {
	SetMail();
    }
    InitScreen();

#ifdef BE
    sigctl(SCUCBHANDLER, 0);
#endif /* BE */

    (void) signal(SIGALRM, NullFunction);
    (void) signal(SIGINT, RestoreCursor);
    (void) signal(SIGTERM, RestoreCursor);
    (void) signal(SIGQUIT, RestoreCursor);

    ProcessNeko();

    RestoreCursor();
}
