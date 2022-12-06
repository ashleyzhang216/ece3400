#include "pins.h" // pin assignments on robot
#include "ultrasonic.h" // ultrasonic code
#include "motors.h" // servo control code
#include "led.h" // led control code
#include "utility.h" // general-use functions and macros
#include "navigation.h" // DFS and navigation
#include "mic.h" // microphone code
#include "phototrans.h" // phototransistor code
#include "celebrate.h" // celebration code
#include "rf.h" // rf communication code

#include "testing.h" // testing functions, included to show our design process

// if manual start button was pressed
volatile bool manual_override = false;

// ISR for interrupt attached to manual start button
void manual_start_ISR() {
  manual_override = true;
}

void setup() {
  // put your setup code here, to run once:

  // for debug
  Serial.begin(9600);

  // setup each individual module's code
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
  // PSUEDOCODE: 
  // while not heard note
  //   listen for note
  // 
  // while not found 2 treasures
  //   check for treasures
  //   add current square to visited
  //   for each neighbor
  //     if not in stack and not visited, add to stack
  //     mark neighbor as accessible from current
  //     mark current as accessible from neighbor
  //  
  //   while top of stack not accessible
  //     travel to top of backtrack stack
  //
  //   add current square to backtrack stack
  //  
  //   travel to target
  //   update position & stack
  //  
  // celebrate

  // listen for 440hz
  while(!listen_for_440() && !manual_override);

  // blink LED to indicate start
  blink_led(500);

  // local variables
  long treasure_freq;
  pos bot_pos = initial_pos; // bot always assumes we start in bottom-right corner
  StackArray<int> backtrack_stack; // contains path robot took from start
  StackArray<int> frontier_stack;  // contains next squares to visit
  int n;

  // continue until both treasures found
  while(!found_2_treasures()) {

    Serial.println("At square " + String(square_num(bot_pos)));

    // check for treasures, multiple times to clear error
    for(int i = 0; i < 3; i++) {
      treasure_freq = check_treasure();
    }
    // if a treasure was detected
    if(treasure_freq != -1) {
      // if this is a new, distinct frequency OR we are in debug mode
      if(update_treasure(treasure_freq)) {
        Serial.print("Transmitting frequency " + String(treasure_freq) + ": ");
        on_led(); // turn on LED to indicate start of transmission
        manual_override = false; // reset manual button so we can press it to override bad tx transmission
        while(!transmit_to_base(treasure_freq) && !manual_override) {
          Serial.print("0");
        }
        Serial.println("1");
        off_led(); // turn off LED to indicate end of transmission
      }

      if(found_2_treasures()) {
        continue; // will break out of loop and celebrate
      }
    }

    // add current square to visited
    visited[square_num(bot_pos)] = true;

    // page ultrasonic sensors to detect walls around this square
    calculateDistances();

    // iterate through left, forward, right directions
    for(int i = 0; i < 3; i++) {
      // get square num of neighbor
      n = neighbor(bot_pos, dir_from_bot(bot_pos, i));

      // if not outside of maze
      if(n != -1) {
        if(!near_zero(distanceToObject[i]) && distanceToObject[i] <= max_us_reading) {
          // see a wall in that direction --> neighbor is not accessible from this square
          Serial.println("Marked neighbor " + String(n) + " as Inaccessible");
          mark_inaccessible(bot_pos, i);
        } else {
          // don't see a wall, neighbor is accessible from this square
          Serial.println("Marked neighbor " + String(n) + " as Accessible");
          // if not already in frontier stack and we haven't visited it, add to frontier stack
          if(!in_stack[n] && !visited[n]) {
            Serial.println("Added neighbor " + String(n) + " to stack");
            in_stack[n] = true;
            frontier_stack.push(n);
          }
        }
      }
    }

    // if we cannot access the next frontier square, backtrack until we can
    // operates on the observation that ALL squares in frontier stack must be accessible from 
    //    a square along the backtrack stack trail
    while(!is_accessible(square_num(bot_pos), frontier_stack.peek())) {
      Serial.println("Backtracking to square " + String(backtrack_stack.peek()));
      bot_pos = travel_to(bot_pos, backtrack_stack.peek());
      backtrack_stack.pop();
      delay_ms(250);
    }

    // add current square to backtrack stack
    backtrack_stack.push(square_num(bot_pos));

    Serial.println("Traveling to square " + String(frontier_stack.peek()));

    // update stack and visited data
    in_stack[frontier_stack.peek()] = false;
    visited [frontier_stack.peek()] = true;
    bot_pos = travel_to(bot_pos, frontier_stack.peek());
    frontier_stack.pop();

    Serial.println("--------------------------------------------------");
  }

  // detected both treasures, celebrate
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
  //turn_test();
  //move_test();

  final_code();
}
