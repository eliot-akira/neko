// Neko touchscreen program by Walfas 
//   http://ee.walfas.org/

// GLCD library can be found here:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1279128237/0
#include <glcd.h>
#include "touch.h"
#include "neko.h"

int nekoX=GLCD.Width/2, nekoY=GLCD.Height/2;      // Neko's current location (top left corner of sprite)
int targetX=GLCD.Width/2, targetY=GLCD.Height/2;  // Neko's target location
int dispX=0, dispY=0;  // Displacement from target
int delayTime = 20;    // Time between frames
int vel = 1;           // How quickly Neko moves
int animDelay = delayTime/4;       // Delay between frames of animation
int animDelayIdle = delayTime/2;   // Same as above but for idle animations
int alertTime = delayTime*8;       // How long Neko remains surprised after a new click
int idleActionTime = delayTime*5;  // If no actions are done after this amount, Neko is idle
int idleActionID = 0;              // Which idle animation to play
int idleActionFrames = 0;          // Counts how many frames the idle action has played for
int maxIdleActionFrames = delayTime*5;  // Idle animation stops after this many frames
int idleAnimations = 0;       // Counts how many idle animations have passed
int maxIdleAnimations = 2;    // After this many idle animations, Neko moves on its own
boolean frame = 0;            // Used as an array offset for two-frame animations
boolean idle = true;
boolean performingIdleAction = false;
unsigned long idleTime = 0;
unsigned long frameCount = 0;

void setup() {
  GLCD.Init(INVERTED);
  GLCD.ClearScreen();
  randomSeed(analogRead(0));
}

void loop() {
  GLCD.ClearScreen();
  
  frameCount++;
  // Cycle through two frames of animation
  if (!idle) {
    if (frameCount > animDelay) {
      frame = !frame;
      frameCount = 0;
    }
  }
  // If idle, slower 2-frame animation
  else if (frameCount > animDelayIdle) {
    frame = !frame;
    frameCount = 0;
  }
  
  // Set a new target coordinate for Neko
  if ( getTouch() ) {
    /* If it's a new click and Neko is idle, go to 
       Neko's alert frame before continuing */
    if ( !mouseDragged() && idle) {
      idle = false;
      idleTime = 0;
      performingIdleAction = false;
      GLCD.DrawBitmap(neko[NEKO_ALERT], nekoX, nekoY);
      delay(alertTime);
    }
    
    // Set target coordinate, contrained by screen boundaries
    targetX = constrain(mouseX,HALF_NEKO_SIZE-1,GLCD.Width-HALF_NEKO_SIZE+1);
    targetY = constrain(mouseY,HALF_NEKO_SIZE-1,GLCD.Height-HALF_NEKO_SIZE+1);
  }
  
  /* Calculate Neko's displacement from target */
  dispX = nekoX+HALF_NEKO_SIZE-targetX;
  dispY = nekoY+HALF_NEKO_SIZE-targetY;  
  
  // Neko moves toward the target
  if (dispX+vel < 0)
    nekoX += vel;
  if (dispX-vel > 0)
    nekoX -= vel;
  if (dispY+vel < 0)
    nekoY += vel;
  if (dispY-vel > 0)
    nekoY -= vel;
  
  // If Neko is at the target location, it is idle
  if (abs(dispX) <= vel && abs(dispY) <= vel) {
    idle = true;
    idleTime++;
    /* If no action is taken in a certain amount 
       of time, Neko performs an idle animation */
    if (idleTime > idleActionTime) {
      // If not already in an idle animation, begin one
      if (!performingIdleAction) { 
        frame = 0;
        performingIdleAction = true;
        idleActionFrames = 0;
        idleActionID = whichIdleAction();  // Decides which idle animation to do
        idleAnimations++;  // Increments the number of idle animations done
      }
      // If already in an idle animation, continue until time is up
      else if (idleActionFrames < maxIdleActionFrames) {
        // The yawn animation is only one frame and lasts half as long
        if (idleActionID == NEKO_YAWN) {
          frame = 0;
          idleActionFrames++;
        }
        idleActionFrames++;
        GLCD.DrawBitmap(neko[idleActionID+frame], nekoX, nekoY);      
      }
      // If done performing an idle animation
      else {
        /* If a certain number of idle animations is
           performed, Neko moves to a new location.*/
        if (idleAnimations >= maxIdleAnimations) {
          idle = false;
          idleTime = 0;
          performingIdleAction = false;
          GLCD.DrawBitmap(neko[NEKO_ALERT], nekoX, nekoY);
          delay(alertTime);  // Wait before moving
          targetX = random(HALF_NEKO_SIZE-1,GLCD.Width-HALF_NEKO_SIZE+1);
          targetY = random(HALF_NEKO_SIZE-1,GLCD.Height-HALF_NEKO_SIZE+1);
          idleAnimations = 0;  // Reset the number of idle animations performed
        }
        
        // Reset idle animation variables
        idleActionID = 1;
        performingIdleAction = false;
        idleActionFrames = 0;
        idleTime = 0;
      }
    } 
    // If not performing (or about to perform) an idle animation
    else {
      GLCD.DrawBitmap(neko[NEKO_IDLE], nekoX, nekoY);
    }
  }
  // If Neko has not reached his destination
  else {
    GLCD.DrawBitmap(neko[whichRunFrame()+frame], nekoX, nekoY);
  }
  
  delay(delayTime);
}

// Decides which idle animation to play
int whichIdleAction() {
  // If Neko is at an edge of the screen, its scratches
  if (nekoX-vel <= 0)
    return NEKO_SCRATCH_W;
  else if (nekoX+vel >= GLCD.Width-NEKO_SIZE)
    return NEKO_SCRATCH_E;  
  else if (nekoY-vel <= 0)
    return NEKO_SCRATCH_N;
  else if (nekoY+vel >= GLCD.Height-NEKO_SIZE)
    return NEKO_SCRATCH_S;
    
  // Otherwise, choose randomly from the rest
  else {
    long rand = random(0,4);
    if (rand == 0)
      return NEKO_YAWN;
    if (rand == 1)
      return NEKO_SLEEP;
    if (rand == 2)
      return NEKO_SCRATCH;
    if (rand == 3)
      return NEKO_IDLE;
  }
}

/* Determines which sprite to use based on where 
   Neko is relative to its target destination*/
int whichRunFrame() {
  if (dispX < -HALF_NEKO_SIZE/4) { 
    if (dispY < -HALF_NEKO_SIZE/4)
      return NEKO_RUN_SE;
    if (dispY > HALF_NEKO_SIZE/4)
      return NEKO_RUN_NE;
    else
      return NEKO_RUN_E;
  }
  else if (dispX > HALF_NEKO_SIZE/4) {
    if (dispY < -HALF_NEKO_SIZE/4)
      return NEKO_RUN_SW;
    if (dispY > HALF_NEKO_SIZE/4)
      return NEKO_RUN_NW;
    else
      return NEKO_RUN_W;
  }
  else { 
    if (dispX < 0) {
      if (abs(dispX) > abs(dispY))
        return NEKO_RUN_E;
      else if (dispY < 0)
        return NEKO_RUN_S;
      else
        return NEKO_RUN_N;
    }
    if (dispX > 0) {
      if (abs(dispX) > abs(dispY))
        return NEKO_RUN_W;
      else if (dispY < 0)
        return NEKO_RUN_S;
      else
        return NEKO_RUN_N;
    }
  }
}
