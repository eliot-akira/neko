/*********************************************************************
	Neko the Replicat 1.06 
	by Greg Weston <gweston@home.com> 
	and John Yanarella <yanarejm@muohio.edu>
	Inspired by Neko for X-windows by Masayuki Koba
	
	main.cpp
	
	This file contains the main routines for the replicat shell. All
	the app really is for is to export the kitten replicant.
	
	Who		When		What
	GJW		19971022	Initial design and coding
	GJW		19971024	Slight functional rearrangements
	GJW		19980515	Cleaned up code and hopefully made it Intel-
						friendly.
	GJW		19990110	Finally got an Intel box. Really cleaning up the
						code. Luckily, I missed the whole PE fiasco. Now
						have to make it compile nicely.
	JMY		20000201	Adopted this abandoned (?) kitten and revised it
						to run under R4.5 x86.  Changed method for loading 
						the images from resources to R4.5 compatible method.
						(Had to recreate graphics to do this, and altered 
						drawing method to be compatible with the new graphics)
						Added background bitmap drop capability.
	JMY		20000203	Added PPC project file and executable to the 
						distribution; provided by Marc Fisher II 
						<fisherii@engr.orst.edu>.  Thanks!!!
	JMY		20000329	Updated to compile on R5 x86.  No PPC version
						of BeOS R5 has been released thus far so the PPC 
						project is not included in this release.
*********************************************************************/

//	Private Headers
#include <Window.h>
#include <Application.h>
#include <MessageFilter.h>
#include "VuKitten.h"

//	Private Prototypes
filter_result f_quit(BMessage* ioMessage, BHandler** ioTarget, BMessageFilter* inFilter);

/*********************************************************************
	main
	Args:	<none>
	Returns:	<none>
	This is the main line of the replicat program. All we do is make
	an application, make a window, and add a kitten to the window.
	We also patch the event queue to make sure the app quits
	properly.
*********************************************************************/
int main(void)
{
	BApplication app("application/x-vnd.Bastion-replicat");
	BRect theRect(0,0,199,199);
	BWindow* win = new BWindow(theRect,"Replicat",B_TITLED_WINDOW,0);
	win->AddChild(new VuKitten(theRect));
	win->AddCommonFilter(new BMessageFilter(B_QUIT_REQUESTED,f_quit));
	win->MoveTo(75,50);
	win->Show();
	app.Run();
	return 0;
}


/*********************************************************************
	f_quit
	Args:	ioMessage - the message we have been asked to filter
			ioTarget - the handler that will receive the message
			inFilter - the messagefilter we are acting for
	Returns:	whether or not to dispatch the message
	This filter intercepts quit messages for the window and sends a
	message to the app to quit. We only have the one window, and
	according to the UI guidelines a (non-server) app without any
	open windows should close.
*********************************************************************/
filter_result f_quit(BMessage* /*ioMessage*/, BHandler** /*ioTarget*/, BMessageFilter* /*inFilter*/)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return B_DISPATCH_MESSAGE;
}
