#include <AccelStepper.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

// Connect a servo motor
Servo servo;

// AccelStepper
AccelStepper stepper(forwardStep, backwardStep);

void setup() {
  Serial.begin(9600);
  Serial.println("BEGIN");
  
  // Begin motor shield
  AFMS.begin();
  
  // Stepper Motor
  myMotor->setSpeed(20000);
  
  // Servo Motor
  servo.attach(10);
  
  // Accel Stepper
  stepper.setMaxSpeed(100000);
  stepper.setSpeed(1000000);
  
}

void loop() {
  
  if (Serial.available() > 0) {
    int inChar = Serial.read();
    if (inChar == 'l') {
      myMotor->step(400, FORWARD, INTERLEAVE);
    } else if (inChar == 'r') {
      myMotor->step(400, BACKWARD, INTERLEAVE);
    }
  }  
  //myMotor->step(100, FORWARD, DOUBLE);
  //delay(10);
  //myMotor->step(100, BACKWARD, DOUBLE);
  
}

void forwardStep() {
  myMotor->step(1, FORWARD, INTERLEAVE);
}

void backwardStep() {
  myMotor->step(1, BACKWARD, INTERLEAVE);
}
