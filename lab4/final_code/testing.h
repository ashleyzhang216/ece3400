#ifndef TESTING_H
#define TESTING_H

#include "ultrasonic.h"
#include "motors.h"

void ultrasonic_test() {
  calculateDistances();
  //Serial.println("Left 1: " + String(distanceToObject[0]) + ", Front: " + String(distanceToObject[1]) + ", Right: " + String(distanceToObject[2]) + ", Left 2: " + String(distanceToObject[3]));
  Serial.println("Total: " + String(distanceToObject[0] + distanceToObject[2]) + ", Left: " + String(distanceToObject[0]) + ", Right: " + distanceToObject[2]);
  delay(250);
}

void pid_test() {
  move_forward_pid(10);
}

#endif
