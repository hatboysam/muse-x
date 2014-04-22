#include <Servo.h>
#include "ServoRange.h"

Servo servo1, servo2;
ServoRange range1, range2;

int fPin1 = A1;
int fPin2 = A2;

void setup() {
  Serial.begin(9600);

  // Setup
  pinMode(fPin1, INPUT);
  pinMode(fPin2, INPUT);
  
  // Calibrate the positions
  calibrateRange(&range1, fPin1, &range2, fPin2);
  
  // Attach servos
  servo1.attach(11);
  servo2.attach(10);
  
  // Find angles
  findAngles(&range1, servo1, fPin1);
  findAngles(&range2, servo2, fPin2);
  
  // Show results
  Serial.println("Servo 1:");
  printRange(&range1);
  Serial.println("Servo 2:");
  printRange(&range2);
}


void loop() {
  // Write to high
  servo1.write(range1.maxDegrees);
  servo2.write(range2.minDegrees);
  delay(1000);
  
  // Write to low
  servo1.write(range2.minDegrees);
  servo2.write(range2.maxDegrees);
  delay(1000);
}

/**
 * Calibrate the positions
 */
void calibrateRange(ServoRange *r1, int pin1, ServoRange *r2, int pin2) {
 // Wait
 Serial.println("Move to low position, then enter any key");
 while (Serial.available() < 1) {}
 Serial.read();
 
 // Read low voltages
 int low1 = analogRead(pin1);
 int low2 = analogRead(pin2);
 
 // Wait
 Serial.println("Move to high position, then enter any key");
 while (Serial.available() < 1) {}
 Serial.read();
 
 // Read high voltages
 int high1 = analogRead(pin1);
 int high2 = analogRead(pin2);
 
 // Set
 r1->minFeedback = low1;
 r1->maxFeedback = high1;
  
 r2->minFeedback = low2;
 r2->maxFeedback = high2;
 
 // Wait
 Serial.println("Remove bar to finish calibration");
 while (Serial.available() < 1) {}
 Serial.read();
}

void findAngles(ServoRange *range, Servo servo, int pin) {
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
}

void calibrate(Servo servo, int fPin, int minDeg, int maxDeg, ServoRange* range) {
  Serial.print("Calibrating...");
  servo.write(minDeg);
  delay(2000);
  int minFeed = analogRead(fPin);
  
  servo.write(maxDeg);
  delay(2000);
  int maxFeed = analogRead(fPin);
  
  Serial.println("done");
  range->minDegrees = minDeg;
  range->maxDegrees = maxDeg;
  range->minFeedback = minFeed;
  range->maxFeedback = maxFeed;
}

void printRange(ServoRange* range) {
  Serial.print(range->minDegrees);
  Serial.print(" to ");
  Serial.print(range->maxDegrees);
  Serial.print(" = ");
  Serial.print(range->minFeedback);
  Serial.print(" to ");
  Serial.println(range->maxFeedback);
}

void writeAngle(int a1, int a2) {
  servo1.write(180 - a1);
  servo2.write(a2);
}

int toAngle(int voltage) {
  return map(voltage, 0, 1023, 0, 180);
}

