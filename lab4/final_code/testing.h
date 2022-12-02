#ifndef TESTING_H
#define TESTING_H

#include "ultrasonic.h"
#include "motors.h"
#include "led.h"
#include "mic.h"
#include "phototrans.h"

void ultrasonic_test() {
  calculateDistances();
  Serial.println("Left 1: " + String(distanceToObject[0]) + ", Front: " + String(distanceToObject[1]) + ", Right: " + String(distanceToObject[2]) + ", Left 2: " + String(distanceToObject[3]));
  //Serial.println("Total: " + String(distanceToObject[0] + distanceToObject[2]) + ", Left: " + String(distanceToObject[0]) + ", Right: " + distanceToObject[2]);
  delay_ms(250);
}

void pid_test() {
  move_forward_pid(10);
}

void motor_test() {
  move_L_servo(FORWARD, 100);
  move_R_servo(FORWARD, 100);
}

void mic_test() {
  while(1) {
    if(listen_for_440()) {
      Serial.println("Heard 440hz!");
      on_led();
      Serial.println("before delay");
      delay_ms(500);
      Serial.println("after delay");
      off_led();

      while(1) {}
    }
  }
}

void phototrans_test() {
  Serial.println(check_treasure());
  //Serial.println("Left: " + String(left_pt()) + ", Front: " + String(front_pt()) + ", Right: " + String(right_pt()));
}

#endif
