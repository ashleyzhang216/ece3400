#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// This code triggers 4 Ultrasonic sensors at once (their trigger is connected
// together), waits for the interrupts to happen for the Echo of each US, and/or
// stops after the timeout value. The distance measured by each sensor is then displayed
// with a 0 for a timed out sensor.

#include "pins.h"

// Number of interrupts
# define numInterrupts 4

// Speed of sound in meters/sec
float soundSpeed = 343.0;

// Time in microsecs of flight for the farthest possible wall, at 81" :
float timeout = ( 81.0 * 2.0 / ((float)soundSpeed * 39.3701) ) * 1000000.0;

// used to calculate and store distanceToObjects to objects, in cm, for each sensor
float distanceToObject[numInterrupts];

// Pins for US sensors
int triggerPin = ULTRASONIC_TRIGGER; // 3
int leftEchoPin_1 = ECHO_LEFT_1; // 8
int leftEchoPin_2 = ECHO_LEFT_2; // 2
int forwardEchoPin = ECHO_FRONT; // 4
int rightEchoPin = ECHO_RIGHT; // 7

// Store the travel times of the pulses
volatile unsigned long pulseTravelTime[numInterrupts];
// Variable to keep track of when each Echo pin changes to HIGH
volatile unsigned long timeEchostateOfPinChangedToHigh[numInterrupts];

// These are set to true when a sensor is read
volatile bool leftSensorIsRead_1 = false;
volatile bool leftSensorIsRead_2 = false;
volatile bool forwardSensorIsRead = false;
volatile bool rightSensorIsRead = false;

// Flag for when pulseTravelTime is measured
volatile bool pulseIsMeasured = false;

// Time variables
unsigned long currentTime; // Used to calculate pulse length

void ultrasonic_setup() {
  // Trigger pin is OUTPUT, all Echo pins are input
  pinMode(triggerPin, OUTPUT);
  pinMode(leftEchoPin_1, INPUT);
  pinMode(leftEchoPin_2, INPUT);
  pinMode(forwardEchoPin, INPUT);
  pinMode(rightEchoPin, INPUT);
}

void ISR_Sensor1_left_1();
void ISR_Sensor4_left_2();
void ISR_Sensor2_forward();
void ISR_Sensor3_right();
void mainInterruptFunction(bool stateOfPin, int IRQ_Number);

// Function to calculate distances:
// 1. Enable the US sensors
// 2. Trigger the US sensors
// 3. Wait until all 4 sensors read their distance values OR one or more timeout
// 4. Write/use those distance values.
void calculateDistances() {

  // Define interrupt routines to call for each echo pin
  attachInterrupt(digitalPinToInterrupt(leftEchoPin_1), ISR_Sensor1_left_1, CHANGE );
  attachInterrupt(digitalPinToInterrupt(leftEchoPin_2), ISR_Sensor4_left_2, CHANGE );
  attachInterrupt(digitalPinToInterrupt(forwardEchoPin), ISR_Sensor2_forward, CHANGE );
  attachInterrupt(digitalPinToInterrupt(rightEchoPin), ISR_Sensor3_right, CHANGE );

  // Trigger Sensors (no need to have the initial LOW because this function already takes up time
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); // OK to use delayMicrosecond
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); // // OK to use delayMicrosecond - 10 us is enough!
  digitalWrite(triggerPin, LOW);    // End of pulse

  //
  // Wait until all three sensors are read. This IS blocking, but OK.
  //
  // Take note of time right when interrupts start to avoid timeouts:
  // The farthest wall is at most about 81" away (see timeout variable)
  unsigned long timeWhenInterruptsStart = micros();

  // Loop as long until all 3 sensors measured a distance, and/or timeouts reached
  while ( (leftSensorIsRead_1 * forwardSensorIsRead * rightSensorIsRead) == false)
  {
    if ((micros() - timeWhenInterruptsStart) >=  timeout) {
      // This means that we're in a timeout! One IRQ or more didn't trigger, so let's stop waiting
      // Let's check if one or more sensor didn't read a distance, and if so set it to 0

      if (leftSensorIsRead_1 == false) {// Left sensor 1 didn't measure in time
        detachInterrupt(digitalPinToInterrupt(leftEchoPin_1));
        pulseTravelTime[0] = 0;
        leftSensorIsRead_1 = true;
      }
      if (leftSensorIsRead_2 == false) {// Left sensor 2 didn't measure in time
        detachInterrupt(digitalPinToInterrupt(leftEchoPin_2));
        pulseTravelTime[3] = 0;
        leftSensorIsRead_2 = true;
      }
      if (forwardSensorIsRead == false) {// Forward sensor didn't measure in time
        detachInterrupt(digitalPinToInterrupt(forwardEchoPin));
        pulseTravelTime[1] = 0;
        forwardSensorIsRead = true;
      }
      if (rightSensorIsRead == false) {// Right sensor didn't measure in time
        detachInterrupt(digitalPinToInterrupt(rightEchoPin));
        pulseTravelTime[2] = 0;
        rightSensorIsRead = true;
      }
    }
  }

  // clear flags so we can read the sensors again later
  leftSensorIsRead_1 = false;
  leftSensorIsRead_2 = false;
  forwardSensorIsRead = false;
  rightSensorIsRead = false;

  // calculate distances
  for (int k = 0; k < numInterrupts; k++)
  {
    distanceToObject[k] = (pulseTravelTime[k] / 2.0) * (float)soundSpeed / 10000.0; // Calculate distances
  }
}

//
// Interrupt routines, one for each sensor echo pin
//
void ISR_Sensor1_left_1() // Left sensor 1
{
  if (leftSensorIsRead_1 == false) {
    //byte readEchoPin = bitRead(PORTC.IN, 6); //  leftEchoPin_1 = 4;
    byte readEchoPin = bitRead(PORTE.IN, 3); // leftEchoPin_1 = 8;
    mainInterruptFunction(readEchoPin, 0);
    if (pulseIsMeasured) { // left sensor value has been read
      // Disable interrupts on left sensor
      detachInterrupt(digitalPinToInterrupt(leftEchoPin_1));
      leftSensorIsRead_1 = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

void ISR_Sensor2_forward() // Forward sensor
{
  if ( forwardSensorIsRead == false) {
    //byte readEchoPin = bitRead(PORTA.IN, 1); // forwardEchoPin = 7;
    byte readEchoPin = bitRead(PORTC.IN, 6); // forwardEchoPin = 4;
    mainInterruptFunction(readEchoPin, 1);
    if (pulseIsMeasured) { // forward sensor value has been read
      // Disable interrupts on forward sensor
      detachInterrupt(digitalPinToInterrupt(forwardEchoPin));
      forwardSensorIsRead = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

void ISR_Sensor3_right() // Right Sensor
{
  if ( rightSensorIsRead == false) {
    //byte readEchoPin = bitRead(PORTE.IN, 3); //rightEchoPin = 8;
    byte readEchoPin = bitRead(PORTA.IN, 1); // rightEchoPin = 7;
    mainInterruptFunction(readEchoPin, 2);
    if (pulseIsMeasured) { // right sensor value has been read
      // Disable interrupts on right sensor
      detachInterrupt(digitalPinToInterrupt(rightEchoPin));
      rightSensorIsRead = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

void ISR_Sensor4_left_2() // Left sensor 2
{
  if (leftSensorIsRead_2 == false) {
    //byte readEchoPin = bitRead(PORTC.IN, 6); //  leftEchoPin_2 = 4;
    byte readEchoPin = bitRead(PORTA.IN, 0); //  leftEchoPin_2 = 2;
    mainInterruptFunction(readEchoPin, 3);
    if (pulseIsMeasured) { // left sensor value has been read
      // Disable interrupts on left sensor
      detachInterrupt(digitalPinToInterrupt(leftEchoPin_2));
      leftSensorIsRead_2 = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

//
// Main Interrupt function
//
void mainInterruptFunction(bool stateOfPin, int IRQ_Number)
{
  currentTime = micros();
  if (stateOfPin)
  {
    // If the pin measured had its state change to HIGH, then save that time (us)!
    timeEchostateOfPinChangedToHigh[IRQ_Number] = currentTime;
  }
  else
  {
    // If the pin measured had its state change to LOW, then calculate how much time has passed (µs)
    pulseTravelTime[IRQ_Number] = currentTime - timeEchostateOfPinChangedToHigh[IRQ_Number];

    // this is the place where the pulse is measured, and so returns true to the ISR that called mainInterruptFunction
    pulseIsMeasured = true;
  }
}

#endif ULTRASONIC_H
