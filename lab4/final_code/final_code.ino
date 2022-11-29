#include "pins.h"
#include "ultrasonic.h"
#include "motors.h"

#include "testing.h"

void setup() {
  // put your setup code here, to run once:
  ultrasonic_setup();
  motor_setup();

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //ultrasonic_test();
  pid_test();
}
