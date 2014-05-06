#include <AccelStepper.h>
#include <Servo.h>

#include "ServoRange.h"

/**
 * Phone Jack
 */
int phonepin = 9;

/** 
 * Servo motor control 
 **/
Servo servo1, servo2;
ServoRange range1, range2;
int servopin1 = 11;
int servopin2 = 10;
int fPin1 = A1;
int fPin2 = A2;
int servoAngle = HIGH;

/**
 * Stepper Control
 */
int dirpin = 2;                   // Direction pin
int steppin = 3;                  // Stepping pin
int stepperDirection = 1;         // Direction

/**
 * AccelStepper Control
 */
AccelStepper stepper(1, steppin, dirpin);

/**
 * Stepper Constants
 */
int motorSpeed = 40000; // Steps per second
int motorAccel = 90000;
int ONE_REV = 1600;

/**
 * Beat Tracking
 */
int beats[] = {0, 1, 1, 1, 0}; // 1-indexed
int currBeat = 1;

/**
 * Other
 */
int inChar;
int prevPhone = LOW;
int currPhone = LOW;
boolean paused = false;
boolean oscillate = false;
int oscillateCount = 0;
int buttonMode = 1;

void setup() { 
  Serial.begin(9600);
  
  // SERVO: Set up
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  pinMode(fPin1, INPUT);
  pinMode(fPin2, INPUT);
  
  // SERVO: Empirical initialization
  range1.minDegrees = 172;
  range1.maxDegrees = 163;
  range1.sign = -1;
  range2.minDegrees = 29;
  range2.maxDegrees = 36;
  range2.sign = 1;
  
  // STEPER: Set up pins
  pinMode(dirpin, OUTPUT);     
  pinMode(steppin, OUTPUT);
  
  // STEPPER: Set up speeds
  digitalWrite(dirpin, stepperDirection);
  stepper.setMaxSpeed(motorSpeed);
  stepper.setSpeed(motorSpeed);
  stepper.setAcceleration(motorAccel);
  
  // STEPPER: Go home
  stepper.moveTo(0);
  
  // SERVO: Go home
  pickDown();
   
  // PHONEJACK: Set up
  pinMode(phonepin, INPUT_PULLUP);
  
  Serial.println("READY TO BEGIN");
}

void loop() { 
  // Check for phone input
  prevPhone = currPhone;
  currPhone = digitalRead(phonepin);
  if (currPhone == LOW && prevPhone == HIGH) {
    Serial.println("Button press registered.");
    if (buttonMode == 1) {
      // Single strum
      oscillateCount = 1;
    } else if (buttonMode == 2) {
      // One measure
      oscillateCount = 4;
    } else if (buttonMode == 3) {
      // Infinite on/off
      oscillate = !oscillate;
    }
  }
  
  // Take serial input
  if (Serial.available() > 0) {
    dispatchInput();
  }
  
  // Move everything, if not paused
  if (!paused) {
    if ((oscillate || (oscillateCount > 0)) && stepperDone()) {
      // Switch stepper direction
      stepperDirection = -1 * stepperDirection;
      
      if (currBeat > 4) {
        currBeat = 1;
      }
      
      // Move the servo
      if (beats[currBeat] == 1) {
        pickDown();
      } else {
        pickUp();
      }
      
      // Move the pick one strum in the new direction
      moveRevs(1.5);  
      
      // Reduce single oscillation, always
      oscillateCount = max(oscillateCount - 1, 0);
      
      // Change the beat
      currBeat = currBeat + 1;
    }
  
    // Must be called as often as possible
    stepper.run();  
    
    // Keep servos in position
    if (servoAngle == HIGH) {
      pickUp();
    } else {
      pickDown();
    }
  }
}

/**
 * Handle input logic
 */
void dispatchInput() {
  inChar = Serial.read();
  // Serial.println(inChar);
  if (inChar == 's') {
    // Start or stop motion
    paused = !paused;
  } else if (inChar == 'h') {
    // Go home
    stepper.moveTo(0);
  } else if (inChar == 'l') {
    // Move left
    stepperDirection = -1;
    moveRevs(0.2);
  } else if (inChar == 'r') {
    // Move right
    stepperDirection = 1;
    moveRevs(0.2); 
  } else if (inChar == 'o') {
    // Turn oscillation on/off
    oscillate = !oscillate;
  } else if (inChar == 'p') {    
    // Switch the servo angle
    switchServos();
  } else if (inChar == 'c') {
    // Calibrate servo motors
    calibrationRoutine();
  } else if (inChar == 'u') {
    // Raise both servos one two degrees
    range1.minDegrees = range1.minDegrees + (range1.sign * 2);
    range2.minDegrees = range2.minDegrees + (range2.sign * 2);
    Serial.println("Raised.");
  } else if (inChar == 'b') {
    // Change the beat pattern
    readBeatChange();
  } else if (inChar == 'm') {
    // Change the button action
    readButtonMode();
  } else if (inChar == 'd') {
    // Lower both servos one two degrees
    range1.minDegrees = range1.minDegrees - (range1.sign * 2);
    range2.minDegrees = range2.minDegrees - (range2.sign * 2);
    Serial.println("Lowered");
  } else {
    // Don't know what it is
    Serial.print("Error: Unknown input: ");
    //char inString[] = { inChar };
    Serial.println(inChar);
  }
}

/**
 * Check if the stepper is done moving
 */
boolean stepperDone() {
  return (stepper.distanceToGo() == 0.0); 
}

/**
 * Move the stepper by revolutions
 */
void moveRevs(float revs) {
  stepper.move(revs * ONE_REV * stepperDirection);
}

/**
 * Read a beat change from serial
 */
void readBeatChange() {
  // Wait for it
  int startMillis = millis();
  while (Serial.available() < 2) {
//    if ((millis() - startMillis) > 4000) {
//      // Give up after 4 seconds
//      Serial.println("Error: bad input");
//      return;
//    }
  };
  
  // Read
  int beatNum = Serial.read() - '0';
  int beatStatus = Serial.read() - '0';
  beatSwitch(beatNum, beatStatus); 
}

void readButtonMode() {
  // Wait for it
  int startMillis = millis();
  while (Serial.available() < 1) {
//    if ((millis() - startMillis) > 4000) {
//      // Give up after 4 seconds
//      Serial.println("Error: bad input");
//      return;
//    }
  };
  
  // Read
  buttonMode = Serial.read() - '0';
}
      

/**
 * Turn a beat on or off
 */
void beatSwitch(int ind, int on) {
   beats[ind] = on; 
}

/**
 * Move both servos
 */
void moveServos() {
  if (servoAngle == HIGH) {
    servo1.write(range1.maxDegrees);
    servo2.write(range2.maxDegrees);
  } else {
    servo1.write(range1.minDegrees);
    servo2.write(range2.minDegrees);
  }
}

/**
 * Move the pick up
 */
void pickUp() {
  //Serial.println("UP");
  servoAngle = HIGH;
  moveServos();
}

/**
 * Move the pick down
 */
void pickDown() {
  //Serial.println("DOWN");
  servoAngle = LOW;
  moveServos();
}

void switchServos() {
  if (servoAngle == HIGH){
    servoAngle = LOW;
  } else {
    servoAngle = HIGH;
  } 
}

/*********************************************************************************************************
 ***************************** SERVO CALIBRATION CODE ****************************************************
 ********************************************************************************************************/
 
void calibrationRoutine() {
  // Detach servos
  servo1.detach();
  servo2.detach();
  
  // Ask for user calibration
  calibrateRange(&range1, fPin1, &range2, fPin2);
  
  // Attach servos again
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  
  // Find angles
  findAngles(&range1, servo1, fPin1);
  findAngles(&range2, servo2, fPin2);
  
  // Debug
  printRange(&range1);
  printRange(&range2);
  
  // Tell user to put bar back in
  Serial.println("Replace bar, then press OK");
  while (Serial.available() < 1) {}
  Serial.read();
}
 
void findAngles(ServoRange *range, Servo servo, int pin) {
  Serial.print("Locating chosen angles...");
  int tolerance = 2;
  
  // Make guesses
  int minFeed = range->minFeedback;
  int maxFeed = range->maxFeedback;
  int lowAngle = 10;
  int highAngle = 170;
  
  do {
    lowAngle = lowAngle + 1;
    servo.write(lowAngle);
    delay(100);
    
  } while (abs(analogRead(pin) - minFeed) > tolerance);
  
  do {
    highAngle = highAngle - 1;
    servo.write(highAngle);
    delay(100);
    
  } while (abs(analogRead(pin) - maxFeed) > tolerance);
  
  // Set and return
  range->minDegrees = lowAngle;
  range->maxDegrees = highAngle;
  Serial.println("done.");
}


/**
 * Calibrate the positions
 */
void calibrateRange(ServoRange *r1, int pin1, ServoRange *r2, int pin2) {
 // Wait
 Serial.println("Move to low position, then press OK");
 while (Serial.available() < 1) {}
 Serial.read();
 
 // Read low voltages
 int low1 = analogRead(pin1);
 int low2 = analogRead(pin2);
 
 // Wait
 Serial.println("Move to high position, then press OK");
 while (Serial.available() < 1) {}
 Serial.read();
 
 // Read high voltages
 int high1 = analogRead(pin1);
 int high2 = analogRead(pin2);
 
 // Set
 r1->minFeedback =low1;
 r1->maxFeedback = high1;
 r1->minDegrees = toAngle(low1);
 r1->maxDegrees = toAngle(high1);
  
 r2->minFeedback = low2;
 r2->maxFeedback = high2;
 r2->minDegrees = toAngle(low2);
 r2->maxDegrees = toAngle(high2);
 
 // Sign (servo may operate "backwards")
 if (low1 > high1) {
   r1->sign = -1;
 } else {
   r1->sign = 1;
 }
 
 if (low2 > high2) {
   r2->sign = -1;
 } else {
   r2->sign = 1;
 }
 
 // Wait
 Serial.println("Remove bar to finish calibration, then press OK");
 while (Serial.available() < 1) {}
 Serial.read();
}

/**
 * Debug
 */
void printRange(ServoRange* range) {
  Serial.print(range->minDegrees);
  Serial.print(" to ");
  Serial.print(range->maxDegrees);
  Serial.print(" = ");
  Serial.print(range->minFeedback);
  Serial.print(" to ");
  Serial.println(range->maxFeedback);
}

/**
 * Conversion
 */
int toAngle(int feed) {
  return map(feed, 0, 1023, 0, 179);
}
