#include <AccelStepper.h>
#include <Servo.h>

/** 
 * Servo motor control 
 **/
Servo servo1;

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
int motorSpeed = 9600;
int motorAccel = 80000;
int ONE_REV = 1600;

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
}

void loop() { 
  // Switch the direction every so often
  currentMillis = millis();
  if (currentMillis - prevMillis > SWITCH_INTERVAL_MS) {
    Serial.println("SWITCH");
    switchStepper();
    stepper.moveTo(stepperDirection * 1 * ONE_REV);
  }
  
  // Must be called as often as possible
  stepper.run();
}

/**
 * @Deprecated
 * Switch the direction of the stepper
 */
void switchStepper() {
    stepperDirection = -1 * stepperDirection;
    prevMillis = currentMillis;  
}

