/*********************************************************************
	Neko the Replicat 1.06 
	by Greg Weston <gweston@home.com> 
	and John Yanarella <yanarejm@muohio.edu>
	Inspired by Neko for X-windows by Masayuki Koba
	
	VuKitten.h
	
	This file contains the class declaration for the neko kitten
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

#ifndef _VUKITTEN_H_
#define _VUKITTEN_H_

//	Superclass
#include <View.h>

class __declspec(dllexport) VuKitten : public BView {
public:
	//	Instantiation
	static BArchivable* Instantiate(BMessage* inArchive);
	
	//	Construction
	VuKitten(BRect inRect);
	VuKitten(BMessage* inArchive);
	virtual ~VuKitten(void);
	
	//	Overrides
	virtual status_t Archive(BMessage* inMessage, bool inDeep =true) const;
	virtual void Draw(BRect inUpdate);
	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage* inMessage);
	virtual void MouseMoved(BPoint inPoint, uint32 inTransit, const BMessage* inMessage);

private:
	//	Constants
	enum OutsidePos {kPosInside, kPosAbove, kPosBelow, kPosLeft, kPosRight};
	enum CatFace {kNorth1, kNorth2, kNorthEast1, kNorthEast2, kEast1, kEast2,
		kSouthEast1, kSouthEast2, kSouth1, kSouth2, kSouthWest1, kSouthWest2,
		kWest1, kWest2, kNorthWest1, kNorthWest2, kAbove1, kAbove2, kRight1, kRight2,
		kBelow1, kBelow2, kLeft1, kLeft2, kSitting, kYawning, kScratch1, kScratch2,
		kSleep1, kSleep2, kBathing, kSurprise};
	static const bigtime_t kShortDelay;
	static const bigtime_t kLongDelay;
	static const double kSmallAngle;

	//	Data
	bool mMouseMoved;
	bool mMouseOutside;
	bool mRunning;
	int mEdgeCount;
	int mScratchCount;
	double mDistance;
	double mTheta;
	bigtime_t mSleepTime;
	thread_id mThread;
	OutsidePos mOutsidePos;
	BBitmap* mKittens;
	BBitmap* mBackground;
	BPoint mCatLoc;
	BPoint mMouseLoc;
	BRect mCatBox;
	CatFace mCatFace;

	//	Utilities
	void CalcVector(BPoint inPoint);
	void DoPaste(BMessage* inMessage);
	void InitKitten(void);
	int32 ThreadProc(void);
	
	//	Statics
	static int32 thread_proc(void* inData);
};

#endif
