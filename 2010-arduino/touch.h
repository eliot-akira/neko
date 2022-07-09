// Pin numbers for touchscreen
#define X1_PIN A4
#define X2_PIN A2
#define Y1_PIN A1
#define Y2_PIN A3

// Touchscreen variable initialization
int pmouseX=0, pmouseY=0, mouseX=0, mouseY=0;
int topLeftXY[] = {830,715};
int bottomRightXY[] = {55,165};

boolean mouseDragged() {
  // Returns true if the previous mouse location is nonzero
  if (pmouseX != 0 && pmouseY != 0 && mouseX != 0 && mouseY != 0) {
    return true;
  }
  else
    return false;
}

boolean clickedRect(int x, int y, int rectWidth, int rectHeight) {
  /* Checks to see if the specified region on the screen is
     clicked (and not dragged) */
  if ( !mouseDragged() &&
       x <= mouseX && mouseX <= x+rectWidth && 
       y <= mouseY && mouseY <= y+rectHeight )
    return true;
  else
    return false;
}

boolean inRect(int x, int y, int rectWidth, int rectHeight) {
  /* Checks to see if the specified region on the screen is
     being touched (dragging counts) */
  if ( x <= mouseX && mouseX <= x+rectWidth && 
       y <= mouseY && mouseY <= y+rectHeight )
    return true;
  else
    return false;
}

boolean getTouch() {  
  /* Gets the touched position on the screen and stores 
     it in mouseX and mouseY, and updates pmouseX and pmouseY
     with previous coordinates. Returns false if the screen
     is not touched. */
  pinMode(X1_PIN,OUTPUT);
  pinMode(X2_PIN,OUTPUT);
  digitalWrite(X1_PIN,LOW);
  digitalWrite(X2_PIN,HIGH);
 
  digitalWrite(Y1_PIN,LOW);
  digitalWrite(Y2_PIN,LOW);
 
  pinMode(Y1_PIN,INPUT);
  pinMode(Y2_PIN,INPUT);
  
  pmouseX = mouseX;
  mouseX = analogRead(Y1_PIN-14); // Subtract 14 to get the analog pin number
  
  pinMode(Y1_PIN,OUTPUT);
  pinMode(Y2_PIN,OUTPUT);
  digitalWrite(Y1_PIN,LOW);
  digitalWrite(Y2_PIN,HIGH);
 
  digitalWrite(X1_PIN,LOW);
  digitalWrite(X2_PIN,LOW);
 
  pinMode(X1_PIN,INPUT);
  pinMode(X2_PIN,INPUT);
 
  pmouseY = mouseY;
  mouseY = analogRead(X1_PIN - 14); // Subtract 14 to get the analog pin number
  
  // Non-zero values get converted to a point on the screen
  if (mouseX != 0 && mouseY != 0) {
    mouseX = (int) map(mouseX, topLeftXY[0], bottomRightXY[0], 0, GLCD.Width);
    mouseY = (int) map(mouseY, topLeftXY[1], bottomRightXY[1], 0, GLCD.Height);
    return true;
  }
  else {
    return false;
  }
}
