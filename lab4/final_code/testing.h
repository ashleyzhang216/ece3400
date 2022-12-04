#ifndef TESTING_H
#define TESTING_H

#include "ultrasonic.h"
#include "motors.h"
#include "led.h"
#include "mic.h"
#include "phototrans.h"
#include "rf.h"

void ultrasonic_test() {
  calculateDistances();
  Serial.println("Left 1: " + String(distanceToObject[0]) + ", Front: " + String(distanceToObject[1]) + ", Right: " + String(distanceToObject[2]) + ", Left 2: " + String(distanceToObject[3]));
  //Serial.println("Total: " + String(distanceToObject[0] + distanceToObject[2]) + ", Left: " + String(distanceToObject[0]) + ", Right: " + distanceToObject[2]);
  delay_ms(250);
}

void pid_test() {
  move_forward();
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
      delay_ms(500);
      off_led();

      while(1) {}
    }
  }
}

void phototrans_test() {
  double treasure_freq;
  for(int i = 0; i < 3; i++) {
    treasure_freq = check_treasure();
  }
  Serial.println(treasure_freq);
  //Serial.println("Left: " + String(left_pt()) + ", Front: " + String(front_pt()) + ", Right: " + String(right_pt()));
}

void turn_test() {
  for(int i = 0; i < 4; i++) {
    turnR();
    delay_ms(250);
  }
  for(int i = 0; i < 4; i++) {
    turnL();
    delay_ms(250);
  }

  while(1) {}
}

void move_test() {
  for(int i = 0; i < 4; i++) {
    move_forward();
    delay_ms(250);
  }

  while(1) {}
}

void rf_test() {
  float send_val = 1000.5;
  bool send_status;

  for(int i = 0; i < 22; i++) {
    Serial.print("Sending " + String(send_val) + ": ");
    delay_ms(100);
    do {
      send_status = transmit_to_base(send_val);
      Serial.print(send_status ? "1" : "0");
    } while(!send_status);
    Serial.println();
    
    send_val += 1001;

    delay_ms(1000);
  }
}

#endif
