/*
  Blink the Arduino's onboard LED
*/


// The setup function is where you put instructions that will
// be run once when the IDE programs the Arduino. 
// More information is available here:
// https://www.arduino.cc/reference/en/language/structure/sketch/setup/

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}


// The loop function runs without end: it is a loop, and notice that in 
// this case, we don't have a condition that says to stop running the loop,
// so it keeps on running indefinitely on the Arduino as long as the 
// Arduino is powered.

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
