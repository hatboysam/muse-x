#include <AccelStepper.h>
#include <Servo.h>

/**
 * Phone Jack
 */
int phonepin = 9;

/** 
 * Servo motor control 
 **/
Servo servo1, servo2;
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
int beats[] = {0, 1, 1, 1, 1};
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
        pickUp();
      } else {
        pickDown();
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
    // TODO: Calibrate
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
void moveServos(int angle) {
  servo1.write(180 - angle);
  servo2.write(angle);
}

/**
 * Move the pick up
 */
void pickUp() {
  servoAngle = HIGH;
  moveServos(UPANGLE);
}

/**
 * Move the pick down
 */
void pickDown() {
  servoAngle = LOW;
  moveServos(DOWNANGLE);
}

void switchServos() {
  if (servoAngle == HIGH){
    servoAngle = LOW;
  } else {
    servoAngle = HIGH;
  } 
}
