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
int stepperDirection = HIGH;      // Direction
 
 /**
  * Stepper Timing
  */
int prevMillis = 0;
int currentMillis = 0;
int stepSpeed;

/**
 * Stepper Constants
 */
int SWITCH_INTERVAL_MS = 500;
int STEP_SPEED_HIGH = 75;
int STEP_SPEED_MID = 105;
int STEP_SPEED_SLOW = 135;

void setup() { 
  // Pin setup
  pinMode(dirpin, OUTPUT);     
  pinMode(steppin, OUTPUT);
  
  // Set up the stepper
  stepSpeed = STEP_SPEED_HIGH;
  digitalWrite(dirpin, stepperDirection);
  digitalWrite(steppin, LOW);
  
  // Accelerate stepper to desired speed
  accelStepper(steppin, stepSpeed);
}

void loop() {
  // Run the stepper at desired speed
  runStepper(steppin, stepSpeed);
  
  // Switch the direction every so often
  currentMillis = millis();
  if (currentMillis - prevMillis > SWITCH_INTERVAL_MS) {
    switchStepper();
  }
}

/**
 * Switch the direction of the stepper
 */
void switchStepper() {
    stepperDirection = !stepperDirection;
    digitalWrite(dirpin, stepperDirection);
    delay(20);
    prevMillis = currentMillis;  
}

/**
 * Accelerate a stepper to a certain speed.
 */
void accelStepper(int stepPinNo, int minDelay) {
  int maxDelay = 3 * minDelay;
  int stepDelay = maxDelay;
  float accelSteps = 1000;
  float i = 0;
 
  while (i < accelSteps) {
    float fraction = i / accelSteps;
    stepDelay = maxDelay - (fraction * (maxDelay - minDelay));
    runStepper(stepPinNo, stepDelay);
    i++;
  }  
}

/**
 * Run a stepper for one step (or microstep, depending on driver settings).
 */
void runStepper(int stepPinNo, int stepDelay) {
  digitalWrite(stepPinNo, HIGH);
  delayMicroseconds(stepDelay);          
  digitalWrite(stepPinNo, LOW); 
  delayMicroseconds(stepDelay);  
}

