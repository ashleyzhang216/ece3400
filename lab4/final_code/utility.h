#ifndef UTILITY_H
#define UTILITY_H

#define DEBUG 1

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

#endif UTILITY_H
