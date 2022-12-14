#ifndef MOTORS_H
#define MOTORS_H

#include <Servo.h>
#include "ultrasonic.h"
#include "phototrans.h"
#include "led.h"
#include "rf.h"
#include "utility.h"

// milliseconds moving forward to roughly travel 1 square
#define MILLISECS_FOR_FORWARD 2500

// function declaration for use in move_forward()
bool update_treasure(double freq);

//pid values
double k_p = 2;
double k_i = 0;
double k_d = 1.25;

// number of values to sample for I
int num_vals_i = 250;

// distances 
double front_wall_threshold = 12; // distance in front of bot that it should stop
double single_wall_distance = 13.5; // ideal distance from single wall
double max_us_reading = 30; // maximum allowed ultrasonic reading

// enum for motor direction
enum move_dir {
  FORWARD, 
  BACKWARD
};

// constants
const float wheel_diam_cm = 7.0;
const float bot_diam_cm = 147.5;

// max value observed from encoders
const int max_servo_fb = 720;

// Create the servo objects
Servo L_servo;
Servo R_servo;

// stop both servos
void stop_servos() {
  L_servo.write(90);
  R_servo.write(90);
}

// move left servo
void move_L_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;

  if(dir == FORWARD) {
    L_servo.write(90 - 90*(power/100.0));
  } else {
    L_servo.write(90 + 90*(power/100.0));
  }
}

// move right servo
void move_R_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;
  
  if(dir == FORWARD) {
    R_servo.write(90 + 90*(power/100.0));
  } else {
    R_servo.write(90 - 90*(power/100.0));
  }
}

// move forward one square
// we chose to do this partially time-based since we have error correction and alignment with walls in front
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

  long treasure_freq;

  // while we haven't traveled for enough time or if we see a wall in the distance in front
  while( ( millis() - start_time) < MILLISECS_FOR_FORWARD || (!near_zero(frontUS()) && frontUS() > front_wall_threshold && frontUS() < 40 ) ) {
    cur_time = millis();
    elapsed_time = cur_time - prev_time;

    // measure ultrasonic
    calculateDistances();

    // true means there is a wall there and this is a valid reading
    left_valid  = !near_zero(leftUS_1()) && leftUS_1() < max_us_reading;
    right_valid = !near_zero(rightUS()) && rightUS() < max_us_reading;
    front_close = !near_zero(frontUS()) && frontUS() < front_wall_threshold;

    // stop bot
    if(front_close) break;
    
    if(!left_valid && !right_valid) { // see no walls
      error = 0;
    } else if(!left_valid) { // see ony right wall
      error = 1.25*(rightUS() - single_wall_distance);
    } else if(!right_valid) { // see only left wall
      error = 1.25*(single_wall_distance - leftUS_1());
    } else { // see both walls
      error = rightUS() - leftUS_1();
    }

    // d calculation
    cum_error += error * elapsed_time/1000;
    derv_error = (error - prev_error) / elapsed_time;

    // i calculation
    prev_errors[prev_err_ctr] = error;
    prev_err_ctr = (prev_err_ctr == num_vals_i - 1) ? 0 : prev_err_ctr + 1;
    cum_error = 0;
    for(int i = 0; i < num_vals_i; i++) cum_error += prev_errors[i];
    cum_error /= num_vals_i;

    // pid calculation
    out = (k_p*error) + (k_i*cum_error) + (k_d*derv_error);

    // move motors
    move_L_servo(FORWARD, 50 + out);
    move_R_servo(FORWARD, 50 - out);

    // update vals
    prev_error = error;
    prev_time = cur_time;

    // quickly check for treasure while moving
    treasure_freq = quick_check_treasure();
    if(treasure_freq != -1) {
      if(update_treasure(treasure_freq)) {
        // stop bot from moving while transmitting frequency
        stop_servos();
        Serial.print("Transmitting frequency " + String(treasure_freq) + ": ");
        on_led();
        // only attempt 10 times, chances are if we don't send it now, we'll read it again 
        //  after we've reached the destination square
        for(int i = 0; i < 10; i++) {
          if(transmit_to_base(treasure_freq)) {
            break;
          } else Serial.print("0");
        }
        Serial.println("1");
        off_led();
        delay_ms(250);
      }
    }

    // delay pid loop
    delay_ms(1);
  }

  // stop robot
  stop_servos();
}

// turn left 90 degrees
// we chose to do this time-based since our pid tuning is good and can correct for angle error
void turnL() {
  move_L_servo(BACKWARD, 25);
  move_R_servo(FORWARD, 25);
  delay_ms(1635);
  stop_servos();
  delay_ms(250);
}

// turn right 90 degrees
// we chose to do this time-based since our pid tuning is good and can correct for angle error
void turnR() {
  move_L_servo(FORWARD, 15);
  move_R_servo(BACKWARD, 15);
  delay_ms(1760);
  stop_servos();
  delay_ms(250);
}

// setup function
void motor_setup() {
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
}

#endif
