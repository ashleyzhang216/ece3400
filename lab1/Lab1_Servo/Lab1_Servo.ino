#include <Servo.h>

// Create the servo object
Servo myservo;

// Global variable for reading feedback value
int servoFeedbackValue;



void setup() {
  Serial.begin(9600);
  myservo.attach(9); // Attach the servo to Digital pin 6
  pinMode(A1, INPUT); // Pin A1 will be used for the feedback from the servo
  myservo.write(90); // Stop servo
}



void loop() {
  // Rotate servo's wheels forward (or backward) at max speed for 2 secs
  myservo.write(180);
  Serial.print("Servo speed value: ");
  Serial.println("180");
  delay(2000);

  // Stop the servo for 2 secs
  myservo.write(90);
  // Read servo's position
  servoFeedbackValue = analogRead(A1);
  // Display the value
  Serial.print("Servo position after going forward (backward) for 2 secs: ");
  Serial.println(servoFeedbackValue);
  Serial.println();
  delay(2000);

  // Rotate servo's wheels backward (or forward) at max speed for 2 secs
  myservo.write(0);
  Serial.print("Servo speed value: ");
  Serial.println("0");
  Serial.println();
  delay(2000);

  // Stop the servo for 2 secs
  myservo.write(90);
  // Read servo's position
  servoFeedbackValue = analogRead(A1);
  // Display the value
  Serial.print("Servo position after going backward (forward) for 2 secs: ");
  Serial.println(servoFeedbackValue);
  Serial.println();
  delay(2000);
}
