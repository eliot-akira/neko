#include <X11/Xlib.h>
#include "oneko.h"

Display        *theDisplay;
char           *ClassName, *ProgramName, *ResourceName;
char           *Foreground, *Background;
char           *CursorForeground, *CursorBackground;
char           *CursorBitmap, *CursorBitmask;
char           *BitmapDir, *Geometry;
char           *HelpFile, *MailFile;
long            IntervalTime, UpdateTime;
int             NekoSpeed, IdleSpace;
Bool            NoShape, NoMouse, SaveUnder;
Bool            BackingStore, CheckMail, AutoRaise;
Toy             Toys[NTOYS];

main(argc, argv)
int             argc;
char           *argv[];
{
    ClassName = "Oneko";

    GetResources(argc, argv);

    (void) printf("DisplayName\t\t= %s\n", DisplayString(theDisplay));
    (void) printf("ProgramName\t\t= %s\n", ProgramName);
    (void) printf("ResourceName\t\t= %s\n", ResourceName);
    (void) printf("Foreground\t\t= %s\n", Foreground);
    (void) printf("Background\t\t= %s\n", Background);
    (void) printf("Kotatsu.foreground\t= %s\n", Kotatsu.foreground);
    (void) printf("Kotatsu.background\t= %s\n", Kotatsu.background);
    (void) printf("Post.foreground\t= %s\n", Post.foreground);
    (void) printf("Post.background\t= %s\n", Post.background);
    (void) printf("CursorForeground\t= %s\n", CursorForeground);
    (void) printf("CursorBackground\t= %s\n", CursorBackground);
    (void) printf("Kotatsu.bitmap\t\t= %s\n", Kotatsu.bitmap);
    (void) printf("Kotatsu.bitmask\t\t= %s\n", Kotatsu.bitmask);
    (void) printf("Post.bitmap\t\t= %s\n", Post.bitmap);
    (void) printf("Post.bitmask\t\t= %s\n", Post.bitmask);
    (void) printf("CursorBitmap\t\t= %s\n", CursorBitmap);
    (void) printf("CursorBitmask\t\t= %s\n", CursorBitmask);
    (void) printf("BitmapDir\t\t= %s\n", BitmapDir? BitmapDir: "(null)");
    (void) printf("IntervalTime\t\t= %d\n", IntervalTime);
    (void) printf("NekoSpeed\t\t= %d\n", NekoSpeed);
    (void) printf("IdleSpace\t\t= %d\n", IdleSpace);
    (void) printf("NoShape\t\t\t= %s\n", NoShape ? "Ture" : "False");
    (void) printf("NoMouse\t\t\t= %s\n", NoMouse ? "Ture" : "False");
    (void) printf("SaveUnder\t\t= %s\n", SaveUnder ? "Ture" : "False");
    (void) printf("BackingStore\t\t= %s\n", BackingStore ? "Ture" : "False");
    (void) printf("HelpFile\t\t= %s\n", HelpFile);
    (void) printf("UpdateTime\t\t= %d\n", UpdateTime);
    (void) printf("CheckMail\t\t= %s\n", CheckMail ? "Ture" : "False");
    (void) printf("MailFile\t\t= %s\n", MailFile);
    (void) printf("AutoRaise\t\t= %s\n", AutoRaise ? "Ture" : "False");
    (void) printf("Geometry\t\t= %s\n", Geometry);
}
