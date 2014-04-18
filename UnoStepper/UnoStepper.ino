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
int servopin1 = 9;
int servopin2 = 10;

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
  
  // Pin setup
  pinMode(dirpin, OUTPUT);     
  pinMode(steppin, OUTPUT);
  
  // Set up the stepper
  digitalWrite(dirpin, stepperDirection);
  stepper.setMaxSpeed(motorSpeed);
  stepper.setSpeed(motorSpeed);
  stepper.setAcceleration(motorAccel);
  
  // Set up servo
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  
  // Test
  if (servo1.attached()) {
    Serial.println("Servo 1 Ready");
  }
  if (servo1.attached()) {
    Serial.println("Servo 2 Ready");
  }
  
  // Go home
  stepper.moveTo(0);
  
  // Phone
  pinMode(phonepin, INPUT_PULLUP);
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
      // Move the pick
      moveRevs(1.5);  
    }
  
    // Must be called as often as possible
    stepper.run();  
  }
}

/**
 * Handle input logic
 */
void dispatchInput() {
  inChar = Serial.read();
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
    moveRevs(1.5);
  } else if (inChar == 'r') {
    // Move right
    stepperDirection = -1;
    moveRevs(1.5); 
  } else if (inChar == 'o') {
    oscillate = !oscillate;
    Serial.print("TOGGLE OSCILLATION: ");
    Serial.println(oscillate);
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
  servo1.write(angle);
  servo2.write(angle);
}
