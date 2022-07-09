/*
 * oneko  -  Neko runs Over the windows.
 * 
 * init.c: Initialization of pixmap table, cursor and so on.
 */

#ifndef	lint
static char     rcsid[] = "$Id: init.c,v 1.20 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif	/* SHAPE */

#include <stdio.h>
#include <sys/param.h>

#include "oneko.h"
#include "state.h"

#ifndef ONEKODIR
#define ONEKODIR	"/usr/lib/X11/oneko" /* bitmap directory */
#endif

#define	GCMASK			(GCFunction | \
				 GCForeground | \
				 GCBackground | \
				 GCTile | \
				 GCTileStipXOrigin | \
				 GCTileStipYOrigin | \
				 GCFillStyle)

#define	max(a, b)	(((a) > (b))? (a): (b))


/*
 * Global variables
 */

char           *Foreground;		/* foreground		 */
char           *Background;		/* background		 */
char           *CursorForeground;	/* cursor foreground	 */
char           *CursorBackground;	/* cursor background	 */
char           *CursorBitmap;		/* cursor bitmap	 */
char           *CursorBitmask;		/* cursor bitmask	 */

char           *BitmapDir;		/* bitmapdir		 */

char           *Geometry;		/* post geometry	 */

Cursor          theCursor;		/* Mouse cursor ID	 */

static int      theScreen;		/* Screen number	 */
static unsigned int theDepth;		/* Window depth		 */

Pixmap          NekoSave;		/* copy neko pixmap	 */
GC              SaveGC;			/* copy GC		 */


static XColor   FgColor;		/* foreground color	 */
static XColor   BgColor;		/* background color	 */
static XColor   CursorFgColor;		/* cursor foreground color */
static XColor   CursorBgColor;		/* cursor background color */


/*
 * Toy status Parameter
 */

Toy             Toys[NTOYS];		/* Post, Kotatsu	 */


/*
 * Toy status
 */

void
SetToyState(toy, set_value)
Toy            *toy;
int             set_value;
{
    toy->state = set_value;
}


/*
 * make bitmap file name
 */

static char    *
MakeBitmapFileName(base, add)
char           *base;
char           *add;
{
    static char     filename[MAXPATHLEN];

    if (BitmapDir && *BitmapDir) {
	(void) sprintf(filename, "%s/%s%s.xbm",
		       BitmapDir, base, add);
    } else {
	(void) sprintf(filename, "%%/%s/%s%s.xbm",
		       ResourceName? ResourceName: ProgramName,
		       base, add);
    }

    return MakeFileName(filename);
}

int
ReadPixmapFromBitmapFile(display, d, filename, width, height,
			 pixmap, x_hot, y_hot, fg, bg, depth)
Display        *display;
Drawable        d;
char           *filename;
unsigned int   *width, *height;
Pixmap         *pixmap;
int            *x_hot, *y_hot;
unsigned long   fg, bg;
unsigned int    depth;
{
    unsigned char *data;
    int ret;

    if ((ret = XmuReadBitmapDataFromFile(filename, width, height,
					 &data, x_hot, y_hot))
					 != BitmapSuccess) {
	return ret;
    }

    if ((*pixmap = XCreatePixmapFromBitmapData(display, d, (char *) data,
					       *width, *height, fg, bg, depth))
					       == None) {
	return (BitmapNoMemory);
    }
    return (BitmapSuccess);
}


/*
 * Initialize for Bitmaps & GCs
 */

static void
InitBitmapAndGCs(max_width, max_height)
unsigned int    *max_width, *max_height;
{
    XGCValues       values;
    Pixmap          pixmap;
    char           *bitmap_file, *bitmask_file;
    unsigned int    width, height;
    unsigned long   valuemask = GCMASK;
    int             x_hot, y_hot;
    int             i;

    *max_width = *max_height = 0;
    values.function = GXcopy;
    values.foreground = FgColor.pixel;
    values.background = BgColor.pixel;
    values.fill_style = FillTiled;
    values.ts_x_origin = 0;
    values.ts_y_origin = 0;
    values.clip_x_origin = 0;
    values.clip_y_origin = 0;
    values.subwindow_mode = IncludeInferiors;
    

    if (NoShape) {
	valuemask |= GCClipXOrigin | GCClipYOrigin |
		     GCClipMask | GCSubwindowMode;
    }

    for (i = 0; i < Nanimes; i++) {
	bitmap_file = MakeBitmapFileName(AnimationTable[i].base_name, "");

	if (ReadPixmapFromBitmapFile(theDisplay, theRoot, bitmap_file,
				     &AnimationTable[i].width,
				     &AnimationTable[i].height,
				     &pixmap,
				     &AnimationTable[i].x_hot,
				     &AnimationTable[i].y_hot,
				     FgColor.pixel,
				     BgColor.pixel,
				     theDepth) != BitmapSuccess) {
	    Error("I can't read pixmap file %s", bitmap_file);
	}
	if (AnimationTable[i].x_hot == -1) {
	    AnimationTable[i].x_hot = (AnimationTable[i].width - 1) / 2;
	}
	if (AnimationTable[i].y_hot == -1) {
	    AnimationTable[i].y_hot = AnimationTable[i].height - 1;
	}
	values.tile = pixmap;

	bitmask_file = MakeBitmapFileName(AnimationTable[i].base_name, "_mask");

	if (XReadBitmapFile(theDisplay, theRoot, bitmask_file,
			    &width, &height,
			    &AnimationTable[i].mask_pattern,
			    &x_hot, &y_hot) != BitmapSuccess) {
	    Error("I can't read bitmap file %s", bitmask_file);
	}
	if ((AnimationTable[i].width != width) &&
	    (AnimationTable[i].height != height)) {
	    Warning("width or height differ between %s, %s",
		    bitmap_file, bitmask_file);
	}
	values.clip_mask = AnimationTable[i].mask_pattern;

	AnimationTable[i].draw_GC = XCreateGC(theDisplay, theRoot,
					      valuemask, &values);

	*max_width = max(*max_width, AnimationTable[i].width);
	*max_height = max(*max_height, AnimationTable[i].height);
    }

    if (NoShape) {
	SaveGC = XCreateGC(theDisplay, theRoot,
			   GCSubwindowMode, &values);
	NekoSave = XCreatePixmap(theDisplay, theRoot,
				 *max_width, *max_height, theDepth);
    }
}


/*
 * Make mouse cursor
 */

static void
MakeMouseCursor()
{
    Pixmap          source;
    Pixmap          mask;
    unsigned int    width, height;
    int             x_hot, y_hot;
    char           *bitmap_file;

    bitmap_file = MakeFileName(CursorBitmask);
    if (XReadBitmapFile(theDisplay, theRoot, bitmap_file, &width, &height,
			&mask, &x_hot, &y_hot) != BitmapSuccess) {
	Error("can't read bitmap file %s", bitmap_file);
    }

    bitmap_file = MakeFileName(CursorBitmap);
    if (XReadBitmapFile(theDisplay, theRoot, bitmap_file, &width, &height,
			&source, &x_hot, &y_hot) != BitmapSuccess) {
	Error("can't read bitmap file %s", bitmap_file);
    }

    theCursor = XCreatePixmapCursor(theDisplay, source, mask,
				    &CursorFgColor, &CursorBgColor,
				    x_hot, y_hot);
}


/*
 * Make kotatsu cursor
 */

static void
MakeToyCursor(toy)
Toy            *toy;
{
    Pixmap          source;
    unsigned int    width, height;
    int             x_hot, y_hot;
    char           *bitmap_file;

    bitmap_file = MakeFileName(toy->bitmap);
    if (XReadBitmapFile(theDisplay, theRoot, bitmap_file, &width, &height,
			&source, &x_hot, &y_hot) != BitmapSuccess) {
	Error("can't read bitmap file %s", bitmap_file);
    }

    toy->cursor = XCreatePixmapCursor(theDisplay, source, toy->mask,
				      &toy->fg_color, &toy->bg_color,
				      x_hot, y_hot);
}


/*
 * Allocate colors
 */

static void
AllocateColor(name, color, colormap)
char           *name;
XColor         *color;
Colormap        colormap;
{
    if (!XParseColor(theDisplay, colormap, name, color)
        || !XAllocColor(theDisplay, colormap, color)) {
	Error("can't allocate color \"%s\".", name);
    }
}

#define	Flip(a, b)	{ char *_tmp; _tmp = a; a = b; b = _tmp; }

static void
SetupColors()
{
    Colormap        colormap;

    colormap = DefaultColormap(theDisplay, theScreen);

    AllocateColor(Foreground, &FgColor, colormap);
    AllocateColor(Background, &BgColor, colormap);
    AllocateColor(Kotatsu.foreground, &Kotatsu.fg_color, colormap);
    AllocateColor(Kotatsu.background, &Kotatsu.bg_color, colormap);
    AllocateColor(Post.foreground, &Post.fg_color, colormap);
    AllocateColor(Post.background, &Post.bg_color, colormap);
    AllocateColor(CursorForeground, &CursorFgColor, colormap);
    AllocateColor(CursorBackground, &CursorBgColor, colormap);
}


/*
 * Set property
 */
#if 0
static void
SetProperty()
{
    Atom            neko_atom;

    neko_atom = XInternAtom(theDisplay, "ONEKO_WINDOW_ID", False);
    XChangeProperty(theDisplay, theRoot, neko_atom, XA_WINDOW, 32,
		    PropModeReplace, &theNeko, 1);
}
#endif


/*
 * create Neko.
 */

static void
CreateNekoWindow(max_width, max_height)
unsigned int    max_width, max_height;
{
    XSetWindowAttributes neko_attributes;
    unsigned long   neko_mask;

    neko_attributes.background_pixel = BgColor.pixel;
    neko_attributes.cursor = theCursor;
    neko_attributes.save_under = SaveUnder;
    neko_attributes.backing_store = BackingStore;
    neko_attributes.override_redirect = True;

    if (NoMouse != True) {
	XChangeWindowAttributes(theDisplay, theRoot, CWCursor,
				&neko_attributes);
    }

    neko_mask = CWBackPixel | CWCursor | CWOverrideRedirect
		| CWSaveUnder | CWBackingStore;

    theNeko = XCreateWindow(theDisplay, theRoot, 0, 0,
			    NoShape? max_width: theRootWidth,
			    NoShape? max_height: theRootHeight,
			    0, theDepth, InputOutput, CopyFromParent,
			    neko_mask, &neko_attributes);

    XStoreName(theDisplay, theNeko,
	       ResourceName? ResourceName: ProgramName);
    XSelectInput(theDisplay, theNeko, NEKO_EVENT_MASK);
}


/*
 * create Toy
 */

static void
CreateToyWindow(toy)
Toy            *toy;
{
    XSetWindowAttributes attributes;
    XGCValues       values;
    Pixmap          pixmap;
    unsigned long   valuemask = GCMASK;
    unsigned long   mask;
    char           *bitmap_file;

    SetToyState(toy, TOY_NO);

    bitmap_file = MakeFileName(toy->bitmask);
    if (XReadBitmapFile(theDisplay, theRoot, bitmap_file,
			&toy->width, &toy->height, &toy->mask,
			&toy->hot_x, &toy->hot_y) != BitmapSuccess) {
	Error("can't read bitmap file %s", bitmap_file);
    }

    bitmap_file = MakeFileName(toy->bitmap);
    if (ReadPixmapFromBitmapFile(theDisplay, theRoot, bitmap_file,
				 &toy->width, &toy->height, &pixmap,
				 &toy->hot_x, &toy->hot_y,
				 toy->fg_color.pixel, toy->bg_color.pixel,
				 theDepth) != BitmapSuccess) {
	Error("can't read pixmap file %s", bitmap_file);
    }

    values.function = GXcopy;
    values.foreground = toy->fg_color.pixel;
    values.background = toy->bg_color.pixel;
    values.fill_style = FillTiled;
    values.tile = pixmap;
    values.ts_x_origin = 0;
    values.ts_y_origin = 0;
    values.clip_x_origin = 0;
    values.clip_y_origin = 0;
    values.clip_mask = toy->mask;

    if (NoShape) {
	valuemask |= GCClipXOrigin | GCClipYOrigin | GCClipMask;
    }

    toy->gc = XCreateGC(theDisplay, theRoot, valuemask, &values);

    attributes.background_pixel = toy->bg_color.pixel;
    attributes.cursor = theCursor;
    attributes.save_under = True;
    attributes.override_redirect = True;

    mask = CWBackPixel | CWCursor | CWOverrideRedirect;

    toy->window = XCreateWindow(theDisplay, theRoot,
				toy->x, toy->y,
				toy->width, toy->height,
				0, theDepth, InputOutput, CopyFromParent,
				mask, &attributes);

#ifdef SHAPE
    if (NoShape == False) {
	XShapeCombineMask(theDisplay, toy->window, ShapeBounding,
			  0, 0, toy->mask, ShapeSet);
    }
#endif	/* SHAPE */
    XFillRectangle(theDisplay, toy->window, toy->gc,
		   0, 0, toy->width, toy->height);

    XStoreName(theDisplay, toy->window,
	       ResourceName? ResourceName: ProgramName);

    XSelectInput(theDisplay, toy->window, TOY_EVENT_MASK);

    if (NoShape) {
	toy->save = XCreatePixmap(theDisplay, theRoot,
				  toy->width, toy->height, theDepth);
    }
}


/*
 * Initialize post geometry
 */

void
InitPostGeometry()
{
    int             dummy;
    char            defgeom[24];

    sprintf(defgeom, "%dx%d-10+10", Post.width, Post.height);
    XGeometry(theDisplay, theScreen, Geometry, defgeom, 0, 1, 1, 0, 0,
	      &Post.x, &Post.y, &dummy, &dummy);
}


/*
 * Initialize screen
 */

void
InitScreen()
{
    Window          temp_root;
    int             window_point_x;
    int             window_point_y;
    unsigned int    border_width;
    unsigned int    max_width, max_height;
    int             event_base, error_base;

#ifdef SHAPE
    if (!NoShape && XShapeQueryExtension(theDisplay,
				       &event_base, &error_base) == False) {
	Warning("Display not suported shape extension.");
	NoShape = True;
    }
#else	/* SHAPE */
    NoShape = True;
#endif	/* SHAPE */

    theScreen = DefaultScreen(theDisplay);
    theDepth = DefaultDepth(theDisplay, theScreen);

    theRoot = RootWindow(theDisplay, theScreen);

    XGetGeometry(theDisplay, theRoot, &temp_root,
		 &window_point_x, &window_point_y,
		 &theRootWidth, &theRootHeight,
		 &border_width, &theDepth);

    SetupColors();
    MakeMouseCursor();

    InitBitmapAndGCs(&max_width, &max_height);

    CreateNekoWindow(max_width, max_height);
    CreateToyWindow(&Kotatsu);
    CreateToyWindow(&Post);
    MakeToyCursor(&Kotatsu);
    MakeToyCursor(&Post);

    InitPostGeometry();

    /* SetProperty(); */
}
