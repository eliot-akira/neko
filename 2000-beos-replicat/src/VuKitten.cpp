/*********************************************************************
	Neko the Replicat 1.06 
	by Greg Weston <gweston@home.com> 
	and John Yanarella <yanarejm@muohio.edu>
	Inspired by Neko for X-windows by Masayuki Koba

	VuKitten.cpp
	
	This file contains the implementation of a kitten as a BeOS
	view.
	
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

//	Class Declaration
#include "VuKitten.h"

//	Private Headers
#include <cstdlib>
#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <Path.h>
#include <Dragger.h>
#include <Resources.h>
#include <TranslationUtils.h>
#include <Entry.h>

//	Static Initilization
const double VuKitten::kSmallAngle = atan(1) / 2;
const bigtime_t VuKitten::kShortDelay = 200000;
const bigtime_t VuKitten::kLongDelay = 400000;


/*********************************************************************
	Instantiate
	Args:	inArchive - the message containing an archived kitten
	Returns:	a kitten created as per the archive
	This routine is the public instantiation function for archived
	kittens. Given a message, we look to see if it contains an
	archived kitten. If it does, we call the archive constructor.
*********************************************************************/
BArchivable* VuKitten::Instantiate(BMessage* inArchive)
{
	//if(!validate_instantiation(inArchive,"VuKitten")) return NULL;
	return new VuKitten(inArchive);
}

#if 0
#pragma mark -
#endif

/*********************************************************************
	VuKitten
	Args:	<none>
	Returns:	<none>
	The default constructor builds a new kitten based on some generic
	parameters.
*********************************************************************/
VuKitten::VuKitten(BRect inRect)
	: BView(inRect,"replicat",B_FOLLOW_ALL_SIDES,B_WILL_DRAW), mKittens(NULL), mBackground(NULL)
{
	// Load kitten images from resources (or file if present)
	mKittens = BTranslationUtils::GetBitmap("neko");

	// Load background bitmap
	mBackground = BTranslationUtils::GetBitmap("background");
	
	//	Set up the playpen and add the replication UI
	InitKitten();

	inRect.left = inRect.right - 7;
	inRect.top = inRect.bottom - 7;
	AddChild(new BDragger(inRect, this, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM));
}

/*********************************************************************
	VuKitten
	Args:	inArchive - a message containing an archived kitten
	Returns:	<none>
	The archive constructor builds a new kitten from data stored in
	an archive message.
*********************************************************************/
VuKitten::VuKitten(BMessage* inArchive)
	: BView(inArchive), mKittens(NULL), mBackground(NULL)
{
	//	Extract the kitten images
	BMessage theKittens;
	if(inArchive->FindMessage("neko_kittens",&theKittens) == B_OK)
		mKittens = new BBitmap(&theKittens);
	BMessage background;
	if(inArchive->FindMessage("background",&background) == B_OK)
		mBackground = new BBitmap(&background);

	//	Don't let replicated views resize
	SetResizingMode(B_FOLLOW_ALL_SIDES);

	InitKitten();	
}


/*********************************************************************
	~VuKitten
	Args:	<none>
	Returns:	<none>
	The destructor kills the kitten and destroys the image.
*********************************************************************/
VuKitten::~VuKitten(void)
{
	mRunning = false;
	(void)kill_thread(mThread);
	delete mKittens;
	delete mBackground;
}

#if 0
#pragma mark -
#endif

/*********************************************************************
	AttachedToWindow
	Args:	<none>
	Returns:	<none>
	Adds background bitmap if it exists.
*********************************************************************/
void VuKitten::AttachedToWindow()
{
	if (mBackground) {
		SetViewBitmap(mBackground);
	}
	
	BView::AttachedToWindow();
}


/*********************************************************************
	Archive
	Args:	inMessage - the message into which we archive
			inDeep - true if we should archive subordinate objects
	Returns:	a status code for the archiving operation
	This routine supports replication of neko by archiving the
	view and the utility objects in a message for storage or
	transmission.
*********************************************************************/
status_t VuKitten::Archive(BMessage* inMessage, bool inDeep) const
{
	//	Let the view archive itself
	BView::Archive(inMessage, inDeep);
	
	//	Let the loader know where to find the code
	inMessage->AddString("add_on","application/x-vnd.Bastion-replicat");
	inMessage->AddString("class", "VuKitten");
	
	//	Archive the images
	BMessage theKittens;
	if((mKittens != NULL) && (mKittens->Archive(&theKittens,inDeep) == B_OK))
		inMessage->AddMessage("neko_kittens",&theKittens);
	
	BMessage background;
	if((mBackground != NULL) && (mBackground->Archive(&background,inDeep) == B_OK))
		inMessage->AddMessage("background",&background);
	
	return B_OK;
}


/*********************************************************************
	Draw
	Args:	inUpdate - the area that needs to be redrawn
	Returns:	<none>
	This routine draws the current image of the kitten.
*********************************************************************/
void VuKitten::Draw(BRect /*inUpdate*/)
{
	if(mKittens == NULL) FillEllipse(mCatBox);
	else
	{
		BRect theRect(0,0,31,31);
		theRect.OffsetBy(mCatFace*32,0);
		DrawBitmap(mKittens,theRect,mCatBox);
	}
}


/*********************************************************************
	MessageReceived
	Args:	inMessage - the message we got
	Returns:	<none>
	This routine captures about requests and displays a system modal
	(grumble) dialog with copyright info. We also receive color drops
	from roColour or bitmap drops and change the background of the playpen.
*********************************************************************/
void VuKitten::MessageReceived(BMessage* inMessage)
{
	switch(inMessage->what)
	{
		case B_SIMPLE_DATA: 
			{
				entry_ref ref;
				inMessage->FindRef("refs", &ref);
				BEntry entry(&ref);
				BPath path(&entry);
		
				delete mBackground;
				mBackground = BTranslationUtils::GetBitmap(path.Path());
				
				if (mBackground) {
					SetViewBitmap(mBackground);
				}
				Invalidate();
			}
			break;
		
		case B_ABOUT_REQUESTED:
			{
				BAlert* alert = new BAlert("About Replicat","Replicat 1.06\n\nby Greg Weston <gweston@home.com>\nand John Yanarella <yanarejm@muohio.edu>","Cool!"); 
				alert->Go(NULL);
			}
			break;
			
		case B_PASTE:
			if(inMessage->WasDropped()) DoPaste(inMessage);
			break;
			
		default:
			BView::MessageReceived(inMessage);
			break;
	}
}


/*********************************************************************
	MouseMoved
	Args:	inPoint - the current location of the mouse
			inTransit - the type of movement noted
			inMessage - the message (if any) being dragged
	Returns:	<none>
	This routine handles mouse movement over the view.
*********************************************************************/
void VuKitten::MouseMoved(BPoint inPoint, uint32 inTransit, const BMessage* /*inMessage*/)
{
	switch(inTransit)
	{
		case B_EXITED_VIEW:
			mMouseOutside = true;
			break;
		
		default:
			CalcVector(inPoint);
			mMouseMoved = true;
			mMouseOutside = false;
			break;
	}
}

#if 0
#pragma mark -
#endif

/*********************************************************************
	CalcVector
	Args:	inPoint - the current mouse location
	Returns:	<none>
	This utility calculates and caches the direction that the
	kitten will travel in the next frame.
*********************************************************************/
void VuKitten::CalcVector(BPoint inPoint)
{
	mMouseLoc = inPoint;

	if(mMouseLoc.y < 32)
	{
		mMouseLoc.y = 32;
		mOutsidePos = kPosAbove;
	}
	else if(mMouseLoc.y > Bounds().bottom+1)
	{
		mMouseLoc.y = Bounds().bottom+1;
		mOutsidePos = kPosBelow;
	}

	if(mMouseLoc.x < 16)
	{
		mMouseLoc.x = 16;
		mOutsidePos = kPosLeft;
	}
	else if(mMouseLoc.x > Bounds().right - 15)
	{
		mMouseLoc.x = Bounds().right - 15;
		mOutsidePos = kPosRight;
	}

	float dx = (mMouseLoc.x - mCatLoc.x);
	float dy = (mCatLoc.y - mMouseLoc.y);

	mDistance = sqrt(dx*dx+dy*dy);
	mTheta = atan2(dy,dx);

	mSleepTime = kShortDelay;
}


/*********************************************************************
	DoPaste
	Args:	inMessage - the pasted data
	Returns:	<none>
	This utility processes paste messages. We only check for data
	from roColour, and use the pasted color info to change the
	background of the kitten's playpen.
*********************************************************************/
void VuKitten::DoPaste(BMessage* inMessage)
{
	ssize_t theSize;
	const rgb_color* theColor;
	if(inMessage->FindData("RGBColor",B_RGB_COLOR_TYPE,(const void**)&theColor,&theSize) == B_OK)
	{
		SetViewColor(*theColor);
		Invalidate();
	}
}


/*********************************************************************
	InitKitten
	Args:	<none>
	Returns:	<none>
	This routine sets up the aspects of the kitten that are
	irrelevant of the constructor used.
*********************************************************************/
void VuKitten::InitKitten(void)
{
	//	Respect the kitten's transparency
	SetDrawingMode(B_OP_ALPHA);
	
	mMouseMoved = false;
	mMouseOutside = true;
	mSleepTime = kShortDelay;
	mEdgeCount = 0;
	mScratchCount = 0;
	mOutsidePos = kPosInside;
	mCatLoc = BPoint(16,32);
	mMouseLoc = BPoint(16,32);
	mCatBox.Set(0,0,31,31);
	mCatFace = kSitting;
	
	//	Fire up the animation thread
	mThread = spawn_thread(thread_proc,"neko",B_DISPLAY_PRIORITY,this);
	(void)resume_thread(mThread);
}


/*********************************************************************
	ThreadProc
	Args:	<none>
	Returns:	0
	This routine calculates the current image and location for neko
	in an infinite loop. Further documentation is included inline.
*********************************************************************/
int32 VuKitten::ThreadProc(void)
{
	mRunning = true;
	while(mRunning)
	{
		(void)snooze(mSleepTime);

		if(mDistance > 16)
		{
			mSleepTime = kShortDelay;
			mCatLoc.x = mCatLoc.x + (cos(mTheta) * 16);
			mCatLoc.y = mCatLoc.y - (sin(mTheta) * 16);
			mDistance -= 16;
			
			if((mTheta >= -kSmallAngle) && (mTheta <= kSmallAngle))
				mCatFace = (mCatFace == kEast1) ? kEast2 : kEast1;
			if((mTheta > kSmallAngle) && (mTheta < 3*kSmallAngle))
				mCatFace = (mCatFace == kNorthEast1) ? kNorthEast2 : kNorthEast1;
			if((mTheta >= 3*kSmallAngle) && (mTheta <= 5*kSmallAngle))
				mCatFace = (mCatFace == kNorth1) ? kNorth2 : kNorth1;
			if((mTheta > 5*kSmallAngle) && (mTheta < 7*kSmallAngle))
				mCatFace = (mCatFace == kNorthWest1) ? kNorthWest2 : kNorthWest1;
			if((mTheta >= 7*kSmallAngle) || (mTheta <= -7*kSmallAngle))
				mCatFace = (mCatFace == kWest1) ? kWest2 : kWest1;
			if((mTheta > -7*kSmallAngle) && (mTheta <= -5*kSmallAngle))
				mCatFace = (mCatFace == kSouthWest1) ? kSouthWest2 : kSouthWest1;
			if((mTheta >= -5*kSmallAngle) && (mTheta <= -3*kSmallAngle))
				mCatFace = (mCatFace == kSouth1) ? kSouth2 : kSouth1;
			if((mTheta > -3*kSmallAngle) && (mTheta < -kSmallAngle))
				mCatFace = (mCatFace == kSouthEast1) ? kSouthEast2 : kSouthEast1;

			mMouseMoved = false;
		}
		else
		{
			mCatLoc = mMouseLoc;
			mSleepTime = kLongDelay;
			
			switch(mCatFace)
			{
				case kSitting:
					if(mMouseOutside)
					{
						switch(mOutsidePos)
						{
							case kPosInside:					break;
							case kPosAbove:	mCatFace = kAbove1;	break;
							case kPosBelow:	mCatFace = kBelow1;	break;
							case kPosLeft:	mCatFace = kLeft1;	break;
							case kPosRight:	mCatFace = kRight1;	break;
						}
						mOutsidePos = kPosInside;
						break;
					}
					mCatFace = kBathing;
					break;
					
				case kAbove1:
					mCatFace = kAbove2;
					if(++mEdgeCount == 6)
					{
						mCatFace = kScratch1;
						mEdgeCount = 0;
					}
					break;
				case kAbove2:
					mCatFace = kAbove1;
					break;
					
				case kBelow1:
					mCatFace = kBelow2;
					if(++mEdgeCount == 6)
					{
						mCatFace = kScratch1;
						mEdgeCount = 0;
					}
					break;
				case kBelow2:
					mCatFace = kBelow1;
					break;
					
				case kLeft1:
					mCatFace = kLeft2;
					if(++mEdgeCount == 6)
					{
						mCatFace = kScratch1;
						mEdgeCount = 0;
					}
					break;
				case kLeft2:
					mCatFace = kLeft1;
					break;
					
				case kRight1:
					mCatFace = kRight2;
					if(++mEdgeCount == 6)
					{
						mCatFace = kScratch1;
						mEdgeCount = 0;
					}
					break;
				case kRight2:
					mCatFace = kRight1;
					break;
					
				case kBathing:
					mCatFace = kSitting;
					if(++mEdgeCount == 6)
					{
						mCatFace = kScratch1;
						mEdgeCount = 0;
					}
					break;
				
				case kScratch1:
					mCatFace = kScratch2;
					break;
				case kScratch2:
					mCatFace = kScratch1;
					if(++mScratchCount == 4)
					{
						mCatFace = kYawning;
						mScratchCount = 0;
					}
					break;
					
				case kYawning:
					mCatFace = kSleep1;
					break;
					
				case kSleep1:
					mCatFace = kSleep2;
					break;
				case kSleep2:
					mCatFace = kSleep1;
					break;
				
				default:
					mCatFace = kSitting;
					break;
			}
			
			if(mMouseMoved)
			{
				mCatFace = kSurprise;
				mEdgeCount = 0;
				mScratchCount = 0;
				mMouseMoved = false;
			}
		}
		
		BWindow* theWindow = Window();
		if(theWindow != NULL)
		{
			if(theWindow->Lock())
			{
				Invalidate(mCatBox);
				mCatBox.Set(mCatLoc.x-16,mCatLoc.y-32,mCatLoc.x+15,mCatLoc.y-1);
				Invalidate(mCatBox);
				if(mMouseOutside)
				{
					BPoint thePoint;
					uint32 theButtons;
					GetMouse(&thePoint,&theButtons); 
					CalcVector(thePoint);
				}
				theWindow->Unlock();
			}
		}
	}
	return 0;
}

#if 0
#pragma mark -
#endif

/*********************************************************************
	thread_proc
	Args:	inData - a pointer to the kitten object
	Returns:	the result of the thread execution
	This routine calls through to a member function in the object.
	That member does the real work of the thread.
*********************************************************************/
int32 VuKitten::thread_proc(void* inData)
{
	return ((VuKitten*)inData)->ThreadProc();
}
