// Basic sketch to measure distance from ultrasonic sensor to object
 
const int triggerPIN = 3;// Trigger
const int echoPIN = 7;// Echo
// Variables to calculate time for pulse to come back, and calculate distance to object
float soundDuration, distToObjectInCM;
 
void setup() {
  // Set Trigger and Echo pins as outputs
   pinMode(triggerPIN, OUTPUT);
   pinMode(echoPIN, INPUT);
   Serial.begin(9600);
   
}
 
void loop() {
  // Pulse triggering sequence
   digitalWrite(triggerPIN, LOW);
   delay(10);
   digitalWrite(triggerPIN, HIGH);
   delay(100);
   digitalWrite(triggerPIN, LOW);

    // Time it took for pulse (Echo) to come back to US
   soundDuration = pulseIn(echoPIN, HIGH);
  // Calculate distance to object in cm
   distToObjectInCM = (soundDuration * .0343) / 2;
   Serial.print("distToObjectInCM: ");
   Serial.println(distToObjectInCM);
   delay(100);
}
