/*
 * oneko  -  Neko runs Over the windows.
 * 
 * mail.c: mail routines
 */

#ifndef	lint
static char     rcsid[] = "$Id: mail.c,v 1.3 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "oneko.h"

#ifndef MAILBOX_DIRECTORY
#ifdef i386
#define MAILBOX_DIRECTORY "/var/mail/"
#else  /* i386 */
#ifdef SYSV
#ifdef SVR4
#define MAILBOX_DIRECTORY "/var/mail/"
#else  /* SVR4 */
#define MAILBOX_DIRECTORY "/usr/mail/"
#endif /* SVR4 */
#else  /* SYSV */
#define MAILBOX_DIRECTORY "/usr/spool/mail/"
#endif /* SYSV */
#endif /* i386 */
#endif /* MAILBOX_DIRECTORY */

long            MailCount;
long            UpdateTime;
Bool            MailArrive = False;
char           *MailFile;


void
SetMail()
{
    static char     buf[1024];

    if (MailFile == NULL) {
	MailFile = buf;
	(void) strcpy(MailFile, MAILBOX_DIRECTORY);
#ifndef NO_CUSERID
	(void) strcat(MailFile, cuserid(NULL));
#else
	(void) strcat(MailFile, getenv("USER"));
#endif
    }
    MailCount = 0;
    MailArrive = False;
    SetToyState(&Post, TOY_NO);
}

void
MailCheck()
{
    struct stat     file_stat;

    if (--MailCount > 0) {
	return;
    }

    MailCount = (int) ((UpdateTime * 1000000) / IntervalTime);

    if ((stat(MailFile, &file_stat) == 0)
	&& (file_stat.st_size != 0)) {
	if (MailArrive == False) {
	    ToyAppear(&Post, Post.x, Post.y);
	}
	MailArrive = True;
    }
    else {
	if (MailArrive == True) {
	    ToyDisappear(&Post);
	}
	MailArrive = False;
    }
}
