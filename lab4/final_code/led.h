#ifndef LED_H
#define LED_H

#include "pins.h"
#include "utility.h"

bool led_on;

void led_setup() {
  pinMode(LED_CONTROL, OUTPUT);
  led_on = false;
}

void toggle_led() {
  if(led_on) {
    digitalWrite(LED_CONTROL, LOW);
    led_on = false;
  } else {
    digitalWrite(LED_CONTROL, HIGH);
    led_on = true;
  }
}

void found_treasures() {
  while(1) {
    toggle_led();
    delay_ms(250);
  }
}

#endif
