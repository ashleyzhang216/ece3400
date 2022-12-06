#ifndef LED_H
#define LED_H

#include "pins.h"
#include "utility.h"

// led state
bool led_on;

// setup function
void led_setup() {
  pinMode(LED_CONTROL, OUTPUT);
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

// turn led off
void off_led() {
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

// turn led on
void on_led() {
  digitalWrite(LED_CONTROL, HIGH);
  led_on = true;
}

// toggle led
void toggle_led() {
  if(led_on) {
    digitalWrite(LED_CONTROL, LOW);
    led_on = false;
  } else {
    digitalWrite(LED_CONTROL, HIGH);
    led_on = true;
  }
}

// blink led for ms milliseconds
void blink_led(int ms) {
  digitalWrite(LED_CONTROL, HIGH);
  delay_ms(ms);
  digitalWrite(LED_CONTROL, LOW);
  led_on = false;
}

#endif
