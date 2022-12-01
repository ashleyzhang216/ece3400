#ifndef UTILITY_H
#define UTILITY_H

#define DEBUG 1

void delay_ms(int m) {
  unsigned long s = millis();
  while(millis() - s < m) {}
}

void throw_error(String message) {
  Serial.println(message);
  if(DEBUG) {
    while(1);
  }
}

#endif UTILITY_H
