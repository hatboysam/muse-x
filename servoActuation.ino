#include <Servo.h>

/*
 * Servo Motor Control for pick actuation
 */


// Initializations
Servo servo1;
int serPin   = 9;
int servoDir = 1;
boolean paused = true;
int startPos = 125;      // in degrees

// Timing
int currMillis = 0;
int prevMillis = 0;
int SWITCH_INTERVAL_MS = 800;  // (msec)
int ONE_REV = 15;              // total degrees in movement


void setup() {
  Serial.begin(9600);
  servo1.attach(serPin);
  if (servo1.attached()) {
    Serial.println("The servo motor was successfully attached.");
  }
  servo1.write(startPos);     // Start from home, at 90 degrees
  if (servo1.read() > 90) {
    Serial.println("The servo motor starting position is obtuse.");
  }
  else if (servo1.read() < 90) {
    Serial.println("The servo motor starting position is acute.");
  }
}


void loop() {
  
  // Wait for Serial Input to Control
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    
    // Start code "S"
    if (inChar == 'S') {    
      Serial.println("Get ready to rumble.");
      paused = false;
    }
    
    // Stop Code = "P"
    else if (inChar == 'P') { 
      Serial.println("Stopping, bru.");
      paused = true;
    }
    
    // Go to user-input angle
    else if (inChar == 'C') {
      paused = true;
      int angler = readFromSerial();  // still got the 'C'
      Serial.println(angler);
      goToAngle(angler);
      //paused = false;
      }
  }
      
  // Condition for movement
  if (paused == false) {  
    currMillis = millis();
    if (currMillis - prevMillis > SWITCH_INTERVAL_MS) {  // time condition for switch direction
      Serial.println("You're about to switch is, bru!!!");
      switchServo();
      servo1.write(startPos + servoDir * 1 * ONE_REV);  // moves servo motor
//      delay(5);
    }
  }
}

/*
 * Switch direction of the servo motion
 */
void switchServo() {
  servoDir   = -1 * servoDir;
  prevMillis = currMillis;
}

/*
 * Go to angle
 */
void goToAngle(int angle) {
  servo1.write(angle);
}

/*
 * Read from Serial
 */
int readFromSerial() {
  int result = 0;
  int numChars = Serial.available();
  for(int i = numChars; i > 0; i--) {
    int digitChar = Serial.read();
    int digitVal = digitChar - '0';
    int scaleFactor = pow(10, i - 1);
    result = result + (digitVal * scaleFactor);
  }
  return result;
}
