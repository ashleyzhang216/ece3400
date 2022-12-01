#include "pins.h"
#include "ultrasonic.h"
#include "motors.h"
#include "led.h"
#include "utility.h"
#include "navigation.h"
#include "mic.h"
#include "phototrans.h"
#include "celebrate.h"

#include "testing.h"

volatile bool manual_override = false;

void manual_start_ISR() {
  manual_override = true;
}

void setup() {
  // put your setup code here, to run once:
  ultrasonic_setup();
  motor_setup();
  led_setup();
  navigation_setup();
  phototrans_setup();
  mic_setup();

  // attach interrupt for manual start button
  attachInterrupt(digitalPinToInterrupt(MANUAL_START), manual_start_ISR, FALLING);

  Serial.begin(9600);
}

void final_code() {
  // while not heard note
  //   listen for note
  // 
  // while not found 2 treasures
  //   check for treasures
  //   add current square to visited
  //   add current square to backtrack stack
  //   for each neighbor
  //     if not in stack and not visited, add to stack
  //     mark neighbor as accessible from current
  //     mark current as accessible from neighbor
  //  
  //   while top of stack not accessible
  //     travel to top of backtrack stack
  //  
  //   travel to target
  //   update position & stack
  //  
  // celebrate

  while(!listen_for_440() && !manual_override);

  blink_led(500);

  while(1) {}

  double treasure_freq;
  pos bot_pos = initial_pos;
  StackArray<int> backtrack_stack;
  StackArray<int> frontier_stack;
  
  while(!found_2_treasures()) {

    // check for treasures
    treasure_freq = check_treasure();
    if(treasure_freq != -1) {
      // TODO: transmit freq
      update_treasure(treasure_freq);
      continue;
    }

    // add current square to visited
    visited[square_num(bot_pos)] = true;

    // add current square to backtrack stack
    backtrack_stack.push(square_num(bot_pos));

    calculateDistances();

    
  }

  celebrate();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //ultrasonic_test();
  //pid_test();
  //motor_test();
  //toggle_led();
  //mic_test();
  //phototrans_test();

  final_code();
}
