#ifndef MOTORS_H
#define MOTORS_H

#include <Servo.h>
#include "ultrasonic.h"
#include "utility.h"

#define SECS_FOR_FORWARD 3

double k_p = 2.5;
double k_i = 0;
double k_d = 1;

int num_vals_i = 250;

double front_wall_threshold = 10;
double single_wall_distance = 13.5;
double max_us_reading = 30;

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

void move_forward() {
  unsigned long cur_time, prev_time = millis();
  unsigned long start_time = millis();
  double error, prev_error = 0;
  double elapsed_time;
  double cum_error, derv_error;
  double out;
  bool left_valid, right_valid, front_close;
  
  int prev_errors[num_vals_i];
  int prev_err_ctr = 0;

  unsigned long left_acc = 0, right_acc = 0;
  int left_progress, right_progress;
  int prev_left_enc = analogRead(L_SERVO_FB);
  int prev_right_enc = analogRead(R_SERVO_FB);

  while( ( millis() - start_time) / 1000 < SECS_FOR_FORWARD || (!near_zero(frontUS()) && frontUS() > front_wall_threshold && frontUS() < max_us_reading ) ) {
    cur_time = millis();
    elapsed_time = cur_time - prev_time;

    calculateDistances();
    left_valid  = !near_zero(leftUS_1()) && leftUS_1() < max_us_reading;
    right_valid = !near_zero(rightUS()) && rightUS() < max_us_reading;
    front_close = !near_zero(frontUS()) && frontUS() < front_wall_threshold;

    if(front_close) break;
    
    if(!left_valid && !right_valid) {
      error = 0;
    } else if(!left_valid) {
      error = 2*(rightUS() - single_wall_distance);
    } else if(!right_valid) {
      error = 2*(single_wall_distance - leftUS_1());
    } else {
      error = rightUS() - leftUS_1();
    }
    
    left_progress = analogRead(L_SERVO_FB) - prev_left_enc;
    right_progress = analogRead(R_SERVO_FB) - prev_right_enc;

    if(left_progress > 0 && left_progress < 50) {
      left_acc += left_progress;
    }
    if(right_progress > 0 && right_progress < 50) {
      right_acc += right_progress;
    }

    prev_left_enc = analogRead(L_SERVO_FB);
    prev_right_enc = analogRead(R_SERVO_FB);
    
    cum_error += error * elapsed_time/1000;
    derv_error = (error - prev_error) / elapsed_time;
    
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

  return;
  delay_ms(250);
  
  Serial.println(String(left_acc) + ", " + String(right_acc));
  
  if(left_acc > right_acc) {
    move_R_servo(FORWARD, 50);
    delay_ms( (left_acc - right_acc) / max_servo_fb * 1.44225 * 1000 );
    stop_servos();
  } else if(left_acc < right_acc) {
    move_L_servo(FORWARD, 50);
    delay_ms( (right_acc - left_acc) / max_servo_fb * 1.44225 * 1000 );
    stop_servos();
  }
}

void turnL() {
  move_L_servo(BACKWARD, 25);
  move_R_servo(FORWARD, 25);
  delay_ms(1665);
  stop_servos();
  delay_ms(250);
}

void turnR() {
  move_L_servo(FORWARD, 15);
  move_R_servo(BACKWARD, 15);
  delay_ms(1825);
  stop_servos();
  delay_ms(250);
}

void motor_setup() {
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
}

#endif
