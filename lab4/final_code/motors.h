#ifndef MOTORS_H
#define MOTORS_H

#include <Servo.h>
#include "ultrasonic.h"
#include "utility.h"

double k_p = 5;
double k_i = 0;
double k_d = 2;

double total_wall_distance = 24;
int num_vals_i = 250;

enum move_dir {
  FORWARD, 
  BACKWARD
};

const float wheel_diam_cm = 7.0;
const float bot_diam_cm = 147.5;

const int max_servo_fb = 720;

// Create the servo objects
Servo L_servo;
Servo R_servo;

void stop_servos() {
  L_servo.write(90);
  R_servo.write(90);
}

void move_L_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;

  if(dir == FORWARD) {
    L_servo.write(90 - 90*(power/100.0));
  } else {
    L_servo.write(90 + 90*(power/100.0));
  }
}

void move_R_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;
  
  if(dir == FORWARD) {
    R_servo.write(90 + 90*(power/100.0));
  } else {
    R_servo.write(90 - 90*(power/100.0));
  }
}

void move_forward_pid(double secs) {
  unsigned long cur_time, prev_time = millis();
  unsigned long start_time = millis();
  double error, prev_error = 0;
  double elapsed_time;
  double cum_error, derv_error;
  double out;
  
  int prev_errors[num_vals_i];
  int prev_err_ctr = 0;

  while( ( millis() - start_time) / 1000 < secs) {
    cur_time = millis();
    elapsed_time = cur_time - prev_time;

    calculateDistances();
    error = distanceToObject[2] - distanceToObject[0];// - abs(distanceToObject[0] + distanceToObject[2] - total_wall_distance);
    
    cum_error += error * elapsed_time/1000;
    derv_error = (error - prev_error) / elapsed_time;

    Serial.println(String(prev_err_ctr) + ": " + String(error));
    
    prev_errors[prev_err_ctr] = error;
    prev_err_ctr = (prev_err_ctr == num_vals_i - 1) ? 0 : prev_err_ctr + 1;

    cum_error = 0;
    for(int i = 0; i < num_vals_i; i++) cum_error += prev_errors[i];
    cum_error /= num_vals_i;

    out = (k_p*error) + (k_i*cum_error) + (k_d*derv_error);

    move_L_servo(FORWARD, 50 + out);
    move_R_servo(FORWARD, 50 - out);

    //Serial.println(error);

    prev_error = error;
    prev_time = cur_time;

    delay_ms(1);
  }
  
  stop_servos();
}

void motor_setup() {
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
}

#endif
