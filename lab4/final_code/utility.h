#ifndef UTILITY_H
#define UTILITY_H

void delay_ms(int m) {
  unsigned long s = millis();
  while(millis() - s < m) {}
}

#endif UTILITY_H
