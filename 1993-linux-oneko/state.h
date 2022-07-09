/*
 * oneko  -  Neko runs Over the windows.
 * 
 * $Id: state.h,v 1.2 1992/03/17 14:08:41 kato Exp kato $
 */


/*
 * general status const.
 */

#define	INITIAL_STATE	0		/* initial status	 */
#define	NOTICE_STATE	1		/* notice status	 */

#define	NO_STATE	-1		/* not change status	 */
#define	NO_FUNC		NULL		/* not defind action	 */
#define	NO_TIME		0		/* no repeats		 */


/*
 * typedef
 */

typedef struct {
    char           *base_name;
    GC              draw_GC;
    Pixmap          mask_pattern;
    unsigned int    width, height;
    int             x_hot, y_hot;
}               Animation;

typedef struct {
    int             next_state;
    int             repeat_count;
    int             state_when_moved;
    int             (*action)();
    int             flame_ticks;
    int             animation_index[2];
}               StateMap;


/*
 * external variables
 */

extern Animation AnimationTable[];
extern StateMap StatusTable[];
extern int      Nanimes;
