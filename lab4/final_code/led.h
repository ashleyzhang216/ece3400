#ifndef LED_H
#define LED_H

#include "pins.h"
#include "utility.h"

bool led_on;

void led_setup() {
  pinMode(LED_CONTROL, OUTPUT);
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

void off_led() {
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

void on_led() {
  digitalWrite(LED_CONTROL, HIGH);
  led_on = true;
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

void blink_led(int ms) {
  digitalWrite(LED_CONTROL, HIGH);
  delay_ms(ms);
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

#endif
