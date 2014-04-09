#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <Servo.h>

/** 
 * Servo motor control 
 **/
Servo servo1;
int servopin = A0;

/**
 * Stepper Control
 */
int dirpin = 2;                   // Direction pin
int steppin = 3;                  // Stepping pin
int stepperDirection = 1;      // Direction

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

boolean paused = false;
boolean oscillate = false;

void setup() { 
  Serial.begin(9600);
  
  // Pin setup
  pinMode(dirpin, OUTPUT);     
  pinMode(steppin, OUTPUT);
  
  // Set up the stepper
  digitalWrite(dirpin, stepperDirection);
  stepper.setMaxSpeed(motorSpeed);
  stepper.setSpeed(motorSpeed);
  stepper.setAcceleration(motorAccel);
  
  // Set up servo
  servo1.attach(14);
  
  // Go home
  stepper.moveTo(0);
}

void loop() { 
  // Take serial input
  if (Serial.available() > 0) {
    int inChar = Serial.read();
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
      moveRevs(0.5);
    } else if (inChar == 'r') {
      // Move right
      stepperDirection = -1;
      moveRevs(0.5);        
    } else if (inChar == 'o') {
      oscillate = !oscillate;
      Serial.print("TOGGLE OSCILLATION: ");
      Serial.println(oscillate);
    }
  }
  
  // Switch the direction every so often
  currentMillis = millis();
  if (!paused) {
    if (oscillate && stepperDone()) {
      stepperDirection = -1 * stepperDirection;
      // Move the pick
      if (stepperDirection < 0) {
        servo1.write(90);
      } else {
        servo1.write(0);
      }
      moveRevs(2);  
    }
  
    // Must be called as often as possible
    stepper.run();  
  }
}

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

