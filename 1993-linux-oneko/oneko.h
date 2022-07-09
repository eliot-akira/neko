/*
 * oneko  -  Neko runs Over the windows.
 * 
 * $Id: oneko.h,v 1.26 1992/03/17 14:08:41 kato Exp kato $
 */

/*
 * Define constant
 */

#define	MAX_TICK		9999	/* Odd Only!		 */

#define	NEKO_EVENT_MASK		(KeyPressMask | \
				 KeyReleaseMask | \
				 ButtonPressMask | \
				 ButtonReleaseMask | \
				 VisibilityChangeMask)

#define	TOY_EVENT_MASK		(ExposureMask | \
				 KeyPressMask | \
				 KeyReleaseMask | \
				 ButtonPressMask | \
				 ButtonReleaseMask | \
				 VisibilityChangeMask)

#define	ALL_EVENT_MASK		(NEKO_EVENT_MASK | TOY_EVENT_MASK)

#define	Post			Toys[0]
#define	Kotatsu			Toys[1]
#define	NTOYS			2


/*
 * Toy structure
 */

typedef struct {
    Cursor          cursor;
    Window          window;
    GC              gc;
    Pixmap          save;
    Pixmap          mask;
    XColor          fg_color, bg_color;
    char           *foreground, *background;
    char           *bitmap, *bitmask;
    int             state;
    int             x, y;
    int             hot_x, hot_y;
    unsigned int    width, height;
}               Toy;


/*
 * Toy status const.
 */

#define	TOY_NO			0	/* Toy not visible on window */
#define	TOY_APPEAR		1	/* Toy appears on window */
#define	TOY_YES			2	/* Toy visible on window */
#define	TOY_DISAPPEAR		3	/* Toy disappears on window */


/*
 * common variables
 */

extern char    *ProgramName;		/* program name		 */
extern char    *ResourceName;		/* resource name	 */

extern Display *theDisplay;		/* Pointer to Display structure */
extern Window   theRoot;		/* Window ID of Root Window */
extern Window   theNeko;		/* Window ID of Cat	 */

extern unsigned int theRootWidth;	/* Width of Root Window	 */
extern unsigned int theRootHeight;	/* Height of Root Window */

extern char    *Foreground;		/* foreground		 */
extern char    *Background;		/* background		 */
extern char    *CursorForeground;	/* cursor foreground	 */
extern char    *CursorBackground;	/* cursor background	 */
extern long     IntervalTime;		/* time			 */
extern int      NekoSpeed;		/* speed		 */
extern int      IdleSpace;		/* idle			 */
extern char    *BitmapDir;		/* bitmapdir		 */
extern char    *CursorBitmap;		/* cursor bitmap	 */
extern char    *CursorBitmask;		/* cursor bitmask	 */
extern Cursor   theCursor;		/* mouse cursor id	 */
extern Bool     NoShape;		/* noshape		 */
extern Bool     NoMouse;		/* nomouse		 */
extern Bool     SaveUnder;		/* saveunder		 */
extern Bool     BackingStore;		/* backingstore		 */
extern long     UpdateTime;		/* check mail box interval (sec) */
extern Bool     CheckMail;		/* checking mail	 */
extern char    *MailFile;		/* mail file name	 */
extern Bool     AutoRaise;		/* auto raise		 */
extern char    *Geometry;		/* post geometry	 */

extern Toy      Toys[];			/* 0 := Post, 1 := Kotatsu */

extern Pixmap   NekoSave;		/* copy neko pixmap	 */
extern GC       SaveGC;			/* copy GC id		 */

extern Bool     MailArrive;		/* mail arrived		 */


/*
 * Functions
 */

void            SetToyState();
void            ToyAppear();
void            ToyDisappear();

void            SetMail();
void            MailCheck();

char           *MakeFileName();
void            RestoreCursor();

void            Error();
void            Warning();

void            InitScreen();
void            GetResources();
