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
int UPANGLE = 100;
int DOWNANGLE = 70;
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
int beats[] = {0, 1, 1, 1, 0};
int currBeat = 1;

/**
 * Other
 */
int inChar;
int prevPhone = LOW;
int currPhone = LOW;
boolean paused = false;
boolean oscillate = false;
boolean singleOscillate = false;

void setup() { 
  Serial.begin(9600);
  
  // SERVO: Set up
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  pinMode(fPin1, INPUT);
  pinMode(fPin2, INPUT);
  range1.minDegrees = 180 - DOWNANGLE;
  range1.maxDegrees = 180 - UPANGLE;
  range2.minDegrees = DOWNANGLE;
  range2.maxDegrees = UPANGLE;
  
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
    singleOscillate = true;
  }
  
  // Take serial input
  if (Serial.available() > 0) {
    dispatchInput();
  }
  
  // Move everything, if not paused
  if (!paused) {
    if ((oscillate || singleOscillate) && stepperDone()) {
      // Switch stepper direction
      stepperDirection = -1 * stepperDirection;
      
      // Change the beat
      currBeat = currBeat + 1;
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
      
      // End single oscillation, always
      singleOscillate = false;
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
    moveRevs(0.1);
  } else if (inChar == 'r') {
    // Move right
    stepperDirection = 1;
    moveRevs(0.1); 
  } else if (inChar == 'o') {
    // Turn oscillation on/off
    oscillate = !oscillate;
  } else if (inChar == 'p') {    
    // Switch the servo angle
    switchServos();
  } else if (inChar == 'c') {
    // Calibrate servo motors
    calibrationRoutine();
  } else {
    // Don't know what it is
    Serial.println("Error: Unknown input.");
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
  
 r2->minFeedback = low2;
 r2->maxFeedback = high2;
 
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
