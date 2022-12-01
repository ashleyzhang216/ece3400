#ifndef CELEBRATE_H
#define CELEBRATE_H

#include "motors.h"
#include "led.h"
#include "utility.h"

void celebrate() {
  move_L_servo(BACKWARD, 25);
  move_R_servo(FORWARD, 25);

  while(1) {
    toggle_led();
    delay_ms(500);
  }
}

#endif
