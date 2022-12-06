#ifndef CELEBRATE_H
#define CELEBRATE_H

#include "motors.h"
#include "led.h"
#include "utility.h"

// If robot succesfully navigates maze,
// Nano's onboard LED turns ON/OFF in 1/2s intervals

void celebrate() {
  Serial.println("Celebrating!");
  while(1) {
    toggle_led(); // function defined in led.h
    delay_ms(500);
  }
}

#endif
