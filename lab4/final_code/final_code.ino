#include "pins.h"
#include "ultrasonic.h"
#include "motors.h"
#include "led.h"
#include "utility.h"
#include "navigation.h"
#include "mic.h"
#include "phototrans.h"
#include "celebrate.h"
#include "rf.h"

#include "testing.h"

volatile bool manual_override = false;

void manual_start_ISR() {
  manual_override = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ultrasonic_setup();
  motor_setup();
  led_setup();
  navigation_setup();
  phototrans_setup();
  mic_setup();
  rf_setup();

  // attach interrupt for manual start button
  attachInterrupt(digitalPinToInterrupt(MANUAL_START), manual_start_ISR, FALLING);
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

  long treasure_freq;
  pos bot_pos = initial_pos;
  StackArray<int> backtrack_stack;
  StackArray<int> frontier_stack;
  int n;
  
  while(!found_2_treasures()) {

    Serial.println("At square " + String(square_num(bot_pos)));

    // check for treasures, multiple times to clear error
    for(int i = 0; i < 3; i++) {
      treasure_freq = check_treasure();
    }
    if(treasure_freq != -1) {
      on_led();
      update_treasure(treasure_freq);
      //while(!transmit_to_base(treasure_freq)) {}
      off_led();
      continue;
    }

    // add current square to visited
    visited[square_num(bot_pos)] = true;

    calculateDistances();

    Serial.print("Distances: ");
    for(int i = 0; i < 3; i++) {
      Serial.print(String(distanceToObject[i]) + ", ");
    }
    Serial.println();

    for(int i = 0; i < 3; i++) {
      n = neighbor(bot_pos, dir_from_bot(bot_pos, i));

      if(n != -1) {
        if(!near_zero(distanceToObject[i]) && distanceToObject[i] <= max_us_reading) {
          Serial.println("Marked neighbor " + String(n) + " as Inaccessible");
          mark_inaccessible(bot_pos, i);
        } else {
          Serial.println("Marked neighbor " + String(n) + " as Accessible");
          if(!in_stack[n] && !visited[n]) {
            Serial.println("Added neighbor " + String(n) + " to stack");
            in_stack[n] = true;
            frontier_stack.push(n);
          }
        }
      }
    }

    while(!is_accessible(square_num(bot_pos), frontier_stack.peek())) {
      Serial.println("Backtracking to square " + String(backtrack_stack.peek()));
      bot_pos = travel_to(bot_pos, backtrack_stack.peek());
      backtrack_stack.pop();
      delay_ms(250);
    }

    // add current square to backtrack stack
    backtrack_stack.push(square_num(bot_pos));

    Serial.println("Traveling to square " + String(frontier_stack.peek()));

    in_stack[frontier_stack.peek()] = false;
    visited [frontier_stack.peek()] = true;
    bot_pos = travel_to(bot_pos, frontier_stack.peek());
    frontier_stack.pop();

    Serial.println("--------------------------------------------------");
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
  //rf_test();

  final_code();
}
