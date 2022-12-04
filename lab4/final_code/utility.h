#ifndef UTILITY_H
#define UTILITY_H

#include <StackArray.h>

#define DEBUG 0
#define ZERO_TOLERANCE 0.01

// our replacement for delay(), should function identically
void delay_ms(int m) {
  unsigned long s = millis();
  double elapsed_time;
  do {
    elapsed_time = millis() - s;
  } while(elapsed_time < m);
}

// for debug
void throw_error(String message) {
  Serial.println(message);
  if(DEBUG) {
    while(1);
  }
}

// for comparing double values, where rounding might mean inequality
bool near_zero(double v) {
  return abs(v) < ZERO_TOLERANCE;
}

#endif UTILITY_H
