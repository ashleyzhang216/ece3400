#ifndef UTILITY_H
#define UTILITY_H

#include <StackArray.h>

#define DEBUG 1
#define ZERO_TOLERANCE 0.01

// our replacement for delay(), should function identically
void delay_ms(int m) {
  unsigned long s = millis();
  double elapsed_time;
  do {
    elapsed_time = millis() - s;
  } while(elapsed_time < m);
}

void throw_error(String message) {
  Serial.println(message);
  if(DEBUG) {
    while(1);
  }
}

bool near_zero(double v) {
  return abs(v) < ZERO_TOLERANCE;
}

void print_int_stack(StackArray<int> stack, String stack_name) {
  StackArray<int> copy;

  Serial.print(stack_name + ": ");
  
  while(!stack.isEmpty()) {
    Serial.print(String(stack.peek()) + ", ");
    copy.push(stack.pop());
  }

  while(!copy.isEmpty()) {
    stack.push(copy.pop());
  }

  Serial.println();
}

#endif UTILITY_H
