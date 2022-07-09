/*
 * oneko  -  Neko runs Over the windows.
 * 
 * state.c: 
 */

#ifndef	lint
static char     rcsid[] = "$Id: state.c,v 1.3 1992/03/17 14:08:41 kato Exp kato $";
#endif

#include <X11/Xlib.h>
#include <stdio.h>

#include "oneko.h"
#include "state.h"


/*
 * Neko status
 */

#define	NEKO_STOP	INITIAL_STATE	/* stoped (this is initial state) */
#define	NEKO_AWAKE	NOTICE_STATE	/* awaked (this is notice state) */
#define	NEKO_U_MOVE	2		/* moving up		 */
#define	NEKO_D_MOVE	3		/* moving down		 */
#define	NEKO_L_MOVE	4		/* moving to the left	 */
#define	NEKO_R_MOVE	5		/* moving to the right	 */
#define	NEKO_UL_MOVE	6		/* moving to the left up */
#define	NEKO_UR_MOVE	7		/* moving to the right up */
#define	NEKO_DL_MOVE	8		/* moving to the left down */
#define	NEKO_DR_MOVE	9		/* moving to the right down */
#define	NEKO_U_WALL	10		/* on the top wall	 */
#define	NEKO_D_WALL	11		/* on the bottom wall	 */
#define	NEKO_L_WALL	12		/* on the left wall	 */
#define	NEKO_R_WALL	13		/* on the right wall	 */
#define	NEKO_JARE	14		/* washing face		 */
#define	NEKO_KAKI	15		/* scratching his head	 */
#define	NEKO_AKUBI	16		/* yawning		 */
#define	NEKO_SLEEP	17		/* sleeping		 */
#define	NEKO_YAWN1	18		/* yawning 1		 */
#define	NEKO_YAWN2	19		/* yawning 2		 */
#define	NEKO_YAWN3	20		/* yawning 3		 */
#define	NEKO_YAWN4	21		/* yawning 4		 */
#define	NEKO_YAWN5	22		/* yawning 5		 */
#define	NEKO_MANEKI	23		/* invitation		 */
#define	NEKO_POST	24		/* scratching post	 */


/*
 * Status table.
 */

extern Bool     OntheWall(), Running(), Notice();

Animation       AnimationTable[] = {
    /* base_name, draw_GC, mask_pattern, width, height, x_hot, y_hot */
    {"mati1", None, None, 0, 0, 0, 0},		/*  0 */
    {"jare2", None, None, 0, 0, 0, 0},		/*  1 */
    {"kaki1", None, None, 0, 0, 0, 0},		/*  2 */
    {"kaki2", None, None, 0, 0, 0, 0},		/*  3 */
    {"mati2", None, None, 0, 0, 0, 0},		/*  4 */
    {"sleep1", None, None, 0, 0, 0, 0},		/*  5 */
    {"sleep2", None, None, 0, 0, 0, 0},		/*  6 */
    {"awake", None, None, 0, 0, 0, 0},		/*  7 */
    {"up1", None, None, 0, 0, 0, 0},		/*  8 */
    {"up2", None, None, 0, 0, 0, 0},		/*  9 */
    {"down1", None, None, 0, 0, 0, 0},		/* 10 */
    {"down2", None, None, 0, 0, 0, 0},		/* 11 */
    {"left1", None, None, 0, 0, 0, 0},		/* 12 */
    {"left2", None, None, 0, 0, 0, 0},		/* 13 */
    {"right1", None, None, 0, 0, 0, 0},		/* 14 */
    {"right2", None, None, 0, 0, 0, 0},		/* 15 */
    {"upleft1", None, None, 0, 0, 0, 0},	/* 16 */
    {"upleft2", None, None, 0, 0, 0, 0},	/* 17 */
    {"upright1", None, None, 0, 0, 0, 0},	/* 18 */
    {"upright2", None, None, 0, 0, 0, 0},	/* 19 */
    {"dwleft1", None, None, 0, 0, 0, 0},	/* 20 */
    {"dwleft2", None, None, 0, 0, 0, 0},	/* 21 */
    {"dwright1", None, None, 0, 0, 0, 0},	/* 22 */
    {"dwright2", None, None, 0, 0, 0, 0},	/* 23 */
    {"utogi1", None, None, 0, 0, 0, 0},		/* 24 */
    {"utogi2", None, None, 0, 0, 0, 0},		/* 25 */
    {"dtogi1", None, None, 0, 0, 0, 0},		/* 26 */
    {"dtogi2", None, None, 0, 0, 0, 0},		/* 27 */
    {"ltogi1", None, None, 0, 0, 0, 0},		/* 28 */
    {"ltogi2", None, None, 0, 0, 0, 0},		/* 29 */
    {"rtogi1", None, None, 0, 0, 0, 0},		/* 30 */
    {"rtogi2", None, None, 0, 0, 0, 0},		/* 31 */
    {"akubi1", None, None, 0, 0, 0, 0},		/* 32 */
    {"akubi2", None, None, 0, 0, 0, 0},		/* 33 */
    {"akubiL", None, None, 0, 0, 0, 0},		/* 34 */
    {"akubiR", None, None, 0, 0, 0, 0},		/* 35 */
    {"maneki1", None, None, 0, 0, 0, 0},	/* 36 */
    {"maneki2", None, None, 0, 0, 0, 0},	/* 37 */
};

int             Nanimes = sizeof(AnimationTable) / sizeof(AnimationTable[0]);

StateMap        StatusTable[] = {
    /* next_state, repeat_count, state_when_moved,
				action, flame_ticks, animation_index[2] */
    {NEKO_JARE, 4, NEKO_AWAKE, OntheWall, 1, 0, 0},	/* NEKO_STOP	*/
    {NEKO_AWAKE, 3, NO_STATE, Notice, 1, 7, 7},		/* NEKO_AWAKE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 8, 9},	/* NEKO_U_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 10, 11},	/* NEKO_D_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 12, 13},	/* NEKO_L_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 14, 15},	/* NEKO_R_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 16, 17},	/* NEKO_UL_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 18, 19},	/* NEKO_UR_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 20, 21},	/* NEKO_DL_MOVE	*/
    {NO_STATE, NO_TIME, NO_STATE, Running, 1, 22, 23},	/* NEKO_DR_MOVE	*/
    {NEKO_JARE, 10, NEKO_AWAKE, NO_FUNC, 1, 24, 25},	/* NEKO_U_WALL	*/
    {NEKO_JARE, 10, NEKO_AWAKE, NO_FUNC, 1, 26, 27},	/* NEKO_D_WALL	*/
    {NEKO_JARE, 10, NEKO_AWAKE, NO_FUNC, 1, 28, 29},	/* NEKO_L_WALL	*/
    {NEKO_JARE, 10, NEKO_AWAKE, NO_FUNC, 1, 30, 31},	/* NEKO_R_WALL	*/
    {NEKO_KAKI, 10, NEKO_AWAKE, NO_FUNC, 1, 1, 0},	/* NEKO_JARE	*/
    {NEKO_AKUBI, 4, NEKO_AWAKE, NO_FUNC, 1, 2, 3},	/* NEKO_KAKI	*/
    {NEKO_SLEEP, 3, NEKO_AWAKE, NO_FUNC, 2, 4, 4},	/* NEKO_AKUBI	*/
    {NEKO_YAWN1, 100, NEKO_AWAKE, NO_FUNC, 4, 5, 6},	/* NEKO_SLEEP	*/
    {NEKO_YAWN2, 1, NEKO_AWAKE, NO_FUNC, 2, 32, 32},	/* NEKO_YAWN1	*/
    {NEKO_YAWN3, 2, NEKO_AWAKE, NO_FUNC, 2, 34, 35},	/* NEKO_YAWN2	*/
    {NEKO_YAWN4, 1, NEKO_AWAKE, NO_FUNC, 2, 32, 32},	/* NEKO_YAWN3	*/
    {NEKO_YAWN5, 2, NEKO_AWAKE, NO_FUNC, 2, 33, 33},	/* NEKO_YAWN4	*/
    {NEKO_SLEEP, 1, NEKO_AWAKE, NO_FUNC, 1, 32, 32},	/* NEKO_YAWN5	*/
    {NEKO_POST, 2, NEKO_AWAKE, NO_FUNC, 1, 36, 37},	/* NEKO_MANEKI	*/
    {NEKO_JARE, 10, NEKO_AWAKE, NO_FUNC, 1, 28, 29},	/* NEKO_POST	*/
};


/*
 * extern variables
 */

extern int      NekoState;		/* status for Neko	 */

extern int      NekoX;			/* Neko X		 */
extern int      NekoY;			/* Neko Y		 */

extern int      NekoMoveDx;		/* Moving distance X	 */
extern int      NekoMoveDy;		/* Moving distance Y	 */

extern int      NekoLastX;		/* last Neko X		 */
extern int      NekoLastY;		/* last Neko Y		 */


/*
 * Determin where neko moves
 */

static void
NekoDirection()
{
    int             new_state;

    if (NekoMoveDx == 0 && NekoMoveDy == 0) {
	new_state = NEKO_STOP;
    } else {
	if (NekoMoveDx > 0) {
	    if (NekoMoveDy > 0) {
		if ((NekoMoveDx * 2) < NekoMoveDy) {
		    new_state = NEKO_D_MOVE;
		} else if ((NekoMoveDy * 2) < NekoMoveDx) {
		    new_state = NEKO_R_MOVE;
		} else {
		    new_state = NEKO_DR_MOVE;
		}
	    } else {
		if ((NekoMoveDx * 2) < -NekoMoveDy) {
		    new_state = NEKO_U_MOVE;
		} else if (-(NekoMoveDy * 2) < NekoMoveDx) {
		    new_state = NEKO_R_MOVE;
		} else {
		    new_state = NEKO_UR_MOVE;
		}
	    }
	} else {
	    if (NekoMoveDy > 0) {
		if (-(NekoMoveDx * 2) < NekoMoveDy) {
		    new_state = NEKO_D_MOVE;
		} else if (NekoMoveDy * 2 < -NekoMoveDx) {
		    new_state = NEKO_L_MOVE;
		} else {
		    new_state = NEKO_DL_MOVE;
		}
	    } else {
		if (-(NekoMoveDx * 2) < -NekoMoveDy) {
		    new_state = NEKO_U_MOVE;
		} else if (-(NekoMoveDy * 2) < -NekoMoveDx) {
		    new_state = NEKO_L_MOVE;
		} else {
		    new_state = NEKO_UL_MOVE;
		}
	    }
	}
    }

    if (NekoState != new_state) {
	SetNekoState(new_state);
    }
}


/*
 * Check frame
 */

static Bool
IsWindowOver(animation)
Animation      *animation;
{
#if 0
    if (NekoY <= animation->y_hot) {
	NekoY = animation->y_hot;
	return True;
    } else if (NekoY >= theRootHeight
	       - (animation->height - animation->y_hot)) {
	NekoY = theRootHeight - (animation->height - animation->y_hot);
	return True;
    }
    if (NekoX <= animation->x_hot) {
	NekoX = animation->x_hot;
	return True;
    } else if (NekoX >= theRootWidth - (animation->width - animation->x_hot)) {
	NekoX = theRootWidth - (animation->width - animation->x_hot);
	return True;
    }
#endif
    return ClipInWindow(&NekoX, &NekoY, animation->width, animation->height,
			animation->x_hot, animation->y_hot);
}


/*
 * Don't Neko move?
 */

static Bool
IsNekoDontMove()
{
    if (NekoX == NekoLastX && NekoY == NekoLastY) {
	return True;
    } else {
	return False;
    }
}


/*
 * Neko on the wall
 */

Bool
OntheWall(animation)
Animation      *animation;
{
    int             i;

    if (Post.state == TOY_YES) {
	SetNekoState(NEKO_MANEKI);
	return False;
    }
    if (NekoMoveDx <= 0 && NekoX <= animation->width) {
	SetNekoState(NEKO_L_WALL);
	return False;
    }
    if (NekoMoveDx >= 0 && NekoX >= theRootWidth - animation->width) {
	SetNekoState(NEKO_R_WALL);
	return False;
    }
    if (NekoMoveDy <= 0 && NekoY <= animation->height) {
	SetNekoState(NEKO_U_WALL);
	return False;
    }
    if (NekoMoveDy >= 0 && NekoY >= theRootHeight - animation->height) {
	SetNekoState(NEKO_D_WALL);
	return False;
    }
    return True;
}


/*
 * Notice
 */

/*ARGSUSED*/
Bool
Notice(animation)
Animation      *animation;
{
    NekoDirection();
    return True;
}


/*
 * Running
 */

Bool
Running(animation)
Animation      *animation;
{
    NekoX += NekoMoveDx;
    NekoY += NekoMoveDy;
    NekoDirection();
    if (IsWindowOver(animation)) {
	if (IsNekoDontMove()) {
	    SetNekoState(NEKO_STOP);
	    return False;
	}
    }
    return True;
}
