#ifndef UTILITY_H
#define UTILITY_H

// our replacement for delay(), should function identically
void delay_ms(int m) {
  unsigned long s = millis();
  double elapsed_time;
  do {
    elapsed_time = millis() - s;
  } while(elapsed_time < m);
}

#endif UTILITY_H
