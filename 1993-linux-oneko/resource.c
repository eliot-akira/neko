/*
 * oneko  -  Neko runs Over the windows.
 * 
 * resource.c: resorce routines
 */

#ifndef	lint
static char     rcsid[] = "$Id: resource.c,v 1.14 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef SYSV
#include <strings.h>
#endif /* SYSV */
#include <sys/param.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include "oneko.h"
#include "patchlevel.h"

#ifdef SYSV
#define	rindex(a, b)	strrchr(a, b)
#endif /* SYSV */

#ifndef LIBDIR
#define LIBDIR		"/usr/lib/X11"
#endif
#ifndef ONEKODIR
#define ONEKODIR	"/usr/lib/X11/oneko"
#endif

#define	FGCOL		"black"		/* default foreground color */
#define	BGCOL		"white"		/* default background color */
#define	KOTATSUFGCOL	"black"		/* default kotatsu foreground color */
#define	KOTATSUBGCOL	"white"		/* default kotatsu background color */
#define	POSTFGCOL	"black"		/* default post foreground color */
#define	POSTBGCOL	"white"		/* default post background color */
#define	CURSORFGCOL	"white"		/* default kotatsu foreground color */
#define	CURSORBGCOL	"black"		/* default kotatsu background color */
#define	KOTATSUBITMAP	"%/kotatsu.xbm"	/* default kotatsu bitmap */
#define	KOTATSUBITMASK	"%/kotatsu_mask.xbm"	/* default kotatsu bitmask */
#define	POSTBITMAP	"%/post.xbm"	/* default post bitmap */
#define	POSTBITMASK	"%/post_mask.xbm"	/* default post bitmask */
#define	CURSORBITMAP	"%/cursor.xbm"	/* default cursor bitmap */
#define	CURSORBITMASK	"%/cursor_mask.xbm"	/* default cursor bitmask */
#define	HELPFILE	"%/help"	/* default help message file */
#define	INTERVAL	"125000"	/* tick time in microsecond */
#define	NEKO_SPEED	"16"		/* in pixel		 */
#define	IDLE_SPACE	"6"		/* threshold for idle	 */
#define	UPDATE_TIME	"60"		/* check mail box interval (sec) */
#define	GEOMETRY	"-0+0"		/* default geometry	 */

char           *ClassName = "Oneko";	/* class name		 */
char           *ProgramName = NULL;	/* program name		 */
char           *ResourceName = NULL;	/* resource name	 */

char           *HelpFile;		/* help file name	 */

static char    *DisplayName;

static XrmOptionDescRec optionRec[] = {
    {"-display", ".display", XrmoptionSepArg, (caddr_t) NULL},
    {"-name", ".name", XrmoptionSepArg, (caddr_t) NULL},
    {"-bitmapdir", ".bitmapdir", XrmoptionSepArg, (caddr_t) NULL},
    {"-kbm", ".kotatsu.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-kbmsk", ".kotatsu.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-kotatsubitmap", ".kotatsu.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-kotatsubitmask", ".kotatsu.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-pbm", ".post.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-pbmsk", ".post.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-postbitmap", ".post.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-postbitmask", ".post.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-mbm", ".cursor.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-mbmsk", ".cursor.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-mousebitmap", ".cursor.bitmap", XrmoptionSepArg, (caddr_t) NULL},
    {"-mousebitmask", ".cursor.bitmask", XrmoptionSepArg, (caddr_t) NULL},
    {"-idle", ".idle", XrmoptionSepArg, (caddr_t) NULL},
    {"-speed", ".nekospeed", XrmoptionSepArg, (caddr_t) NULL},
    {"-time", ".intervaltime", XrmoptionSepArg, (caddr_t) NULL},
    {"-bg", "*background", XrmoptionSepArg, (caddr_t) NULL},
    {"-fg", "*foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-background", "*background", XrmoptionSepArg, (caddr_t) NULL},
    {"-foreground", "*foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-kbg", ".kotatsu.background", XrmoptionSepArg, (caddr_t) NULL},
    {"-kfg", ".kotatsu.foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-kotatsubackground", ".kotatsu.background",
					XrmoptionSepArg, (caddr_t) NULL},
    {"-kotatsuforeground", ".kotatsu.foreground",
					XrmoptionSepArg, (caddr_t) NULL},
    {"-pbg", ".post.background", XrmoptionSepArg, (caddr_t) NULL},
    {"-pfg", ".post.foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-postbackground", ".post.background", XrmoptionSepArg, (caddr_t) NULL},
    {"-postforeground", ".post.foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-mbg", ".cursor.background", XrmoptionSepArg, (caddr_t) NULL},
    {"-mfg", ".cursor.foreground", XrmoptionSepArg, (caddr_t) NULL},
    {"-mousebackground", ".cursor.background",
					XrmoptionSepArg, (caddr_t) NULL},
    {"-mouseforeground", ".cursor.foreground",
					XrmoptionSepArg, (caddr_t) NULL},
    {"-noshape", ".noshape", XrmoptionNoArg, (caddr_t) "on"},
    {"+noshape", ".noshape", XrmoptionNoArg, (caddr_t) "off"},
    {"-nomouse", ".nomouse", XrmoptionNoArg, (caddr_t) "on"},
    {"+nomouse", ".nomouse", XrmoptionNoArg, (caddr_t) "off"},
    {"-backingstore", ".backingstore", XrmoptionNoArg, (caddr_t) "on"},
    {"+backingstore", ".backingstore", XrmoptionNoArg, (caddr_t) "off"},
    {"-saveunder", ".saveunder", XrmoptionNoArg, (caddr_t) "on"},
    {"+saveunder", ".saveunder", XrmoptionNoArg, (caddr_t) "off"},
    {"-helpfile", ".helpfile", XrmoptionSepArg, (caddr_t) NULL},
    {"-xrm", NULL, XrmoptionResArg, (caddr_t) NULL},
    {"-update", ".update", XrmoptionSepArg, (caddr_t) NULL},
    {"-mail", ".mail", XrmoptionNoArg, (caddr_t) "off"},
    {"+mail", ".mail", XrmoptionNoArg, (caddr_t) "on"},
    {"-autoraise", ".autoraise", XrmoptionNoArg, (caddr_t) "on"},
    {"+autoraise", ".autoraise", XrmoptionNoArg, (caddr_t) "off"},
    {"-file", ".file", XrmoptionSepArg, (caddr_t) NULL},
    {"-geometry", ".post.geometry", XrmoptionSepArg, (caddr_t) NULL},
};

#define optionNum (sizeof optionRec / sizeof optionRec[0])

typedef enum {
    type_string,
    type_int,
    type_bool,
}               value_types;

typedef struct {
    char           *name;
    char           *class;
    value_types     type;
    char           *defvalue;
    caddr_t        *variable;
}               resvalue;

static resvalue resourceValuses[] = {
    {"display", "Display", type_string, NULL, (caddr_t *) &DisplayName},
    {"name", "Name", type_string, NULL, (caddr_t *) &ResourceName},
    {"bitmapdir", "Bitmapdir", type_string, NULL, (caddr_t *) &BitmapDir},
    {"kotatsu.bitmap", "Kotatsu.Bitmap", type_string, KOTATSUBITMAP,
						(caddr_t *) &Kotatsu.bitmap},
    {"kotatsu.bitmask", "Kotatsu.Bitmask", type_string, KOTATSUBITMASK,
						(caddr_t *) &Kotatsu.bitmask},
    {"post.bitmap", "Post.Bitmap", type_string, POSTBITMAP,
						(caddr_t *) &Post.bitmap},
    {"post.bitmask", "Post.Bitmask", type_string, POSTBITMASK,
						(caddr_t *) &Post.bitmask},
    {"cursor.bitmap", "Cursor.Bitmap", type_string, CURSORBITMAP,
						(caddr_t *) &CursorBitmap},
    {"cursor.bitmask", "Cursor.Bitmask", type_string, CURSORBITMASK,
						(caddr_t *) &CursorBitmask},
    {"idlespace", "Idlespace", type_int, (char *) IDLE_SPACE,
						(caddr_t *) &IdleSpace},
    {"nekospeed", "Nekospeed", type_int, (char *) NEKO_SPEED,
						(caddr_t *) &NekoSpeed},
    {"intervaltime", "Intervaltime", type_int, (char *) INTERVAL,
						(caddr_t *) &IntervalTime},
    {"background", "Background", type_string, BGCOL,
						(caddr_t *) &Background},
    {"foreground", "Foreground", type_string, FGCOL,
						(caddr_t *) &Foreground},
    {"kotatsu.background", "Kotatsu.Background",
	type_string, KOTATSUBGCOL, (caddr_t *) &Kotatsu.background},
    {"kotatsu.foreground", "Kotatsu.Foreground",
	type_string, KOTATSUFGCOL, (caddr_t *) &Kotatsu.foreground},
    {"post.background", "Post.Background",
	type_string, POSTBGCOL, (caddr_t *) &Post.background},
    {"post.foreground", "Post.Foreground",
	type_string, POSTFGCOL, (caddr_t *) &Post.foreground},
    {"cursor.background", "Cursor.Background",
	type_string, CURSORBGCOL, (caddr_t *) &CursorBackground},
    {"cursor.foreground", "Cursor.Foreground",
	type_string, CURSORFGCOL, (caddr_t *) &CursorForeground},
    {"noshape", "Noshape", type_bool, "off", (caddr_t *) &NoShape},
    {"nomouse", "Nomouse", type_bool, "off", (caddr_t *) &NoMouse},
    {"saveunder", "Saveunder", type_bool, "off", (caddr_t *) &SaveUnder},
    {"backingstore", "Backingstore", type_bool, "off",
						(caddr_t *) &BackingStore},
    {"helpfile", "Helpfile", type_string, HELPFILE, (caddr_t *) &HelpFile},
    {"update", "Update", type_int, (char *) UPDATE_TIME,
						(caddr_t *) &UpdateTime},
    {"mail", "Mail", type_bool, (char *) "on", (caddr_t *) &CheckMail},
    {"autoraise", "Autoraise", type_bool, (char *) "off",
						(caddr_t *) &AutoRaise},
    {"file", "File", type_string, NULL, (caddr_t *) &MailFile},
    {"post.geometry", "Post.Geometry", type_string, GEOMETRY,
						(caddr_t *) &Geometry},
};

#define NARGVARS (sizeof resourceValuses / sizeof resourceValuses[0])


/*
 * make file name
 */

char           *
MakeFileName(filename)
char           *filename;
{
    static char     buffer[MAXPATHLEN];

    if (filename[0] != '%') {
	(void) strcpy(buffer, filename);
	return buffer;
    }
    filename++;				/* skip '%' char */
    (void) sprintf(buffer, "%s%s", ONEKODIR, filename);

    return buffer;
}


/*
 * print usage messages.
 */

static void
Usage()
{
    FILE           *fp;
    char           *file;
    char            buf[256];

    file = MakeFileName(HelpFile);
    if ((fp = fopen(file, "r")) == NULL) {
	(void) fprintf(stderr, "%s:  can't open help file \"%s\".\n",
		ProgramName, file);
	return;
    }
    while (fgets(buf, sizeof(buf), fp)) {
	fputs(buf, stderr);
    }
    (void) fclose(fp);
}


/*
 * option syntax error.
 */

static void
OptionError(badOption)
char           *badOption;
{

    (void) fprintf(stderr, "%s:  bad command line option \"%s\"\n\n",
	    ProgramName, badOption);
    Usage();
    exit(1);
}


/*
 * PrintVersion()
 */

void
PrintVersion()
{
    (void) printf("Oneko - neko runs over the window.\n");
    (void) printf("        Version %d.%s patchlevel %d\n",
	   VERSION, MINOR_VERSION, PATCHLEVEL);
    exit(0);
}


/*
 * help messages.
 */

static void
Help()
{
    Usage();
    exit(0);
}


/*
 * warning messages.
 */

/*VARARGS1*/
void
Warning(s, p1, p2, p3)
char           *s;
char           *p1, p2, p3;
{
    (void) fprintf(stderr, "%s: Warning: ", ProgramName);
    (void) fprintf(stderr, s, p1, p2, p3);
    (void) fprintf(stderr, "\n");
}


/*
 * error messages.
 */

/*VARARGS1*/
void
Error(s, p1, p2, p3)
char           *s;
char           *p1, p2, p3;
{
    (void) fprintf(stderr, "%s: Fatal error: ", ProgramName);
    (void) fprintf(stderr, s, p1, p2, p3);
    (void) fprintf(stderr, "\n");
    exit(-1);
}


/*
 * convert string to lower
 */

static void
lower(str)
char *str;
{
    char *ptr;

    for (ptr = str; *ptr; ptr++) {
	if (isupper(*ptr)) {
	    *ptr -= 'A' - 'a';
	}
    }
}


/*
 * get a value from resource data base.
 */

static void
GetValue(database, name, class, valueType, def, valuep)
XrmDatabase     database;
char           *name;
char           *class;
value_types     valueType;
char           *def;
caddr_t        *valuep;			/* RETURN */
{
    char           *type;
    XrmValue        value;
    char           *string;
    char            buffer[1024];
    char            fullname[1024];
    char            fullclass[1024];
    int             len = 0;

    (void) sprintf(fullname, "%s.%s", ProgramName, name);
    (void) sprintf(fullclass, "%s.%s", ClassName, class);
    if (XrmGetResource(database, fullname, fullclass, &type, &value)) {
	string = value.addr;
	len = value.size;
    } else {
	string = def;
	if (string) {
	    len = strlen(string);
	}
    }
    if (string) {
	(void) strncpy(buffer, string, sizeof(buffer));
    }
    buffer[len] = '\0';

    switch (valueType) {
    case type_string:
	if (string) {
	    char           *ptr;
	    char           *malloc();

	    if ((ptr = malloc(strlen(buffer) + 1)) == NULL) {
		Error("GetValue - couldn't allocate memory");
	    }
	    (void) strcpy(ptr, buffer);
	    *((char **) valuep) = ptr;
	}
	else {
	    *((char **) valuep) = NULL;
	}
	break;
    case type_bool:
	lower(buffer);
	*((int *) valuep) = (!strcmp(buffer, "true") ||
			     !strcmp(buffer, "on") ||
			     !strcmp(buffer, "enabled") ||
			     !strcmp(buffer, "yes")) ? True : False;
	break;
    case type_int:
	*((int *) valuep) = atoi(buffer);
	break;
    }
}


/*
 * get resource database from Apprication Default.
 */

static          XrmDatabase
getAppDefaults(xfilesearchpath, TypeName, ClassName)
char           *xfilesearchpath;
char           *TypeName;
char           *ClassName;
{
    XrmDatabase     database = NULL;
    char            pathbuffer[1024];
    char            appdefaults[1024];
    char           *tok;

    (void) strcpy(pathbuffer, xfilesearchpath);
    tok = strtok(pathbuffer, ":");
    while (tok) {
	(void) sprintf(appdefaults, "%s/%s/%s", tok, TypeName, ClassName);
	database = XrmGetFileDatabase(appdefaults);
	if (database != NULL) {
	    return database;
	}
	tok = strtok(NULL, ":");
    }
    return database;
}


/*
 * get resources.
 */

void
GetResources(argc, argv)
int             argc;
char           *argv[];
{
    XrmDatabase     RDB = NULL;
    XrmDatabase     cmdlineDB = NULL;
    XrmDatabase     homeDB = NULL;
    XrmDatabase     applicationDB = NULL;
    XrmDatabase     serverDB = NULL;
    char           *env;
    char           *serverString;
    int             i;
    char           *getenv();

    XrmInitialize();

    if ((ProgramName = rindex(argv[0], '/')) == NULL) {
	ProgramName = argv[0];
    }

    env = getenv("XFILESEARCHPATH");
    applicationDB = getAppDefaults(env ? env : LIBDIR,
				   "app-defaults", ClassName);

    XrmParseCommand(&cmdlineDB, optionRec, optionNum, ProgramName,
		    &argc, argv);

    (void) XrmMergeDatabases(applicationDB, &RDB);
    (void) XrmMergeDatabases(cmdlineDB, &RDB);

    env = getenv("DISPLAY");
    GetValue(RDB, "display", "Display", type_string,
	     env ? env : NULL, &DisplayName);

    if (!(theDisplay = XOpenDisplay(DisplayName))) {
	Error("unable to open display %s.", DisplayName);
    }
    serverString = XResourceManagerString(theDisplay);
    if (serverString) {
	serverDB = XrmGetStringDatabase(serverString);
	(void) XrmMergeDatabases(serverDB, &RDB);
    } else {
	char            buf[1024];

	env = getenv("HOME");
	(void) sprintf(buf, "%s/.Xdefaults", env ? env : "");
	homeDB = XrmGetFileDatabase(buf);
	(void) XrmMergeDatabases(homeDB, &RDB);
    }

    for (i = 0; i < NARGVARS; i++) {
	GetValue(RDB, resourceValuses[i].name, resourceValuses[i].class,
		 resourceValuses[i].type, resourceValuses[i].defvalue,
		 resourceValuses[i].variable);
    }

    /* Parse the rest of the command line */
    for (argc--, argv++; argc > 0; argc--, argv++) {
	if (**argv != '-') {
	    OptionError(*argv);
	}
	switch (argv[0][1]) {
	case 'v':
	    PrintVersion();
	    /* NOTREACHED */
	case 'h':
	    Help();
	    /* NOTREACHED */
	default:
	    OptionError(*argv);
	    /* NOTREACHED */
	}
	/* NOTREACHED */
    }

    (void) XrmDestroyDatabase(RDB);
}
