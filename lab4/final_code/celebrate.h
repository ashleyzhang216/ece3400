#ifndef CELEBRATE_H
#define CELEBRATE_H

#include "motors.h"
#include "led.h"
#include "utility.h"

void celebrate() {
  Serial.println("Celebrating!");
  while(1) {
    toggle_led();
    delay_ms(500);
  }
}

#endif
