#include <Adafruit_MotorShield.h>
#include <Wire.h>
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
int UPANGLE = 100;
int DOWNANGLE = 70;
int servoAngle = UPANGLE;

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
 * Stepper Timing
 */
int prevMillis = 0;
int currentMillis = 0;

/**
 * Stepper Constants
 */
int SWITCH_INTERVAL_MS = 1000;
int motorSpeed = 40000; // Steps per second
int motorAccel = 90000;
int ONE_REV = 1600;

/**
 * Other
 */
 int inChar;
 int prevPhone = LOW;
 int currPhone = LOW;

boolean paused = false;
boolean oscillate = false;

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
    Serial.println("SLAP DA BUTTON");
    moveRevs(1.5);
  }
  
  // Take serial input
  if (Serial.available() > 0) {
    dispatchInput();
  }
  
  // Switch the direction every so often
  currentMillis = millis();
  if (!paused) {
    if (oscillate && stepperDone()) {
      // Switch stepper direction
      stepperDirection = -1 * stepperDirection;
      
      // Pick up or down
      switchServos();
      
      // Move the pick
      moveRevs(1.5);  
    }
  
    // Must be called as often as possible
    stepper.run();  
    
    // Keep servos in position
    // moveServos(servoAngle);
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
    Serial.print("Moving: ");
    Serial.println(!paused);
  } else if (inChar == 'h') {
    // Go home
    stepper.moveTo(0);
  } else if (inChar == 'l') {
    // Move left
    stepperDirection = 1;
    moveRevs(0.1);
  } else if (inChar == 'r') {
    // Move right
    stepperDirection = -1;
    moveRevs(0.1); 
  } else if (inChar == 'o') {
    // Turn oscillation on/off
    oscillate = !oscillate;
  } else if (inChar == 'p') {    
    // Switch the servo angle
    switchServos();
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
 * Switch the direction of the stepper
 */
void switchStepper() {
    stepperDirection = -1 * stepperDirection;
    prevMillis = currentMillis;  
}

/**
 * Move both servos
 */
void moveServos(int angle) {
  servo1.write(180 - angle);
  servo2.write(angle);
}

void switchServos() {
  if (servoAngle == UPANGLE){
    servoAngle = DOWNANGLE;
  } else {
    servoAngle = UPANGLE;
  } 
}
