#include "pins.h"
#include "ultrasonic.h"
#include "motors.h"
#include "led.h"
#include "utility.h"
#include "navigation.h"
#include "mic.h"
#include "phototrans.h"

#include "testing.h"

void setup() {
  // put your setup code here, to run once:
  ultrasonic_setup();
  motor_setup();
  led_setup();
  navigation_setup();
  mic_setup();
  phototrans_setup();

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
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //ultrasonic_test();
  //pid_test();
  //motor_test();
  //toggle_led();
  //mic_test();
  phototrans_test();
}
