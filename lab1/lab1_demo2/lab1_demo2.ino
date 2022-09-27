#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A2
#define R_SERVO_FB A1

enum move_dir {
  FORWARD, 
  BACKWARD
};

enum turn_dir {
  LEFT, 
  RIGHT
};

// Create the servo objects
Servo L_servo;
Servo R_servo;

const int triggerPIN = 3;// Trigger
const int echoPIN = 7;// Echo
// Variables to calculate time for pulse to come back, and calculate distance to object
float soundDuration, distToObjectInCM;

// Number of interrupts
# define numInterrupts 3

// Speed of sound in meters/sec
float soundSpeed = 343.0;

// Time in microsecs of flight for the farthest possible wall, at 81" :
float timeout = ( 81.0 * 2.0 / ((float)soundSpeed * 39.3701) ) * 1000000.0;

// used to calculate and store distanceToObjects to objects, in cm, for each sensor
float distanceToObject[numInterrupts];

// Pins for US sensors
int triggerPin = 3;
int leftEchoPin = 4; // Pin PC6
int forwardEchoPin = 7; // Pin PA1
int rightEchoPin = 8; // Pin PE3

// Store the travel times of the pulses
volatile unsigned long pulseTravelTime[numInterrupts];
// Variable to keep track of when each Echo pin changes to HIGH
volatile unsigned long timeEchostateOfPinChangedToHigh[numInterrupts];

// These are set to true when a sensor is read
volatile bool leftSensorIsRead = false;
volatile bool forwardSensorIsRead = false;
volatile bool rightSensorIsRead = false;

// Flag for when pulseTravelTime is measured
volatile bool pulseIsMeasured = false;

// Time variables
unsigned long currentTime; // Used to calculate pulse length

void bot_stop() {
  L_servo.write(90); // stop left servo
  R_servo.write(90); // stop right servo
}

// power value chosen to balance left and right servos since their power is not equal
void move_L_servo(move_dir dir) {
  if(dir == FORWARD) {
    L_servo.write(90 - 90*(35/100.0)); // set servo to 35% forward
  } else {
    L_servo.write(90 + 90*(35/100.0)); // set servo to 35% backward
  }
}

// power value chosen to balance left and right servos since their power is not equal
void move_R_servo(move_dir dir) {
  if(dir == FORWARD) {
    R_servo.write(90 + 90*(47.5/100.0)); // set servo to 47.5% forward
  } else {
    R_servo.write(90 - 90*(47.5/100.0)); // set servo to 47.5% backward
  }
}

void turn_L() {
  //move_L_servo(BACKWARD, 25);
  L_servo.write(90 + 90*(25/100.0)); // set left servo to 25% power backward
  //move_R_servo(FORWARD, 25);
  R_servo.write(90 + 90*(25/100.0)); // set right servo to 25% power forward
  
  delay(1650); // wait for bot to turn 90 degrees

  bot_stop();
}

void turn_R() {
  L_servo.write(90 - 90*(25/100.0)); // set left servo to 25% power forward
  R_servo.write(90 - 90*(25/100.0)); // set right servo to 25% power backward

  delay(1020); // wait for bot to turn 90 degrees

  bot_stop(); // stop
}

// Function to calculate distances:
// 1. Enable the US sensors
// 2. Trigger the US sensors
// 3. Wait until all 3 sensors read their distance values OR one or more timeout
// 4. Write/use those distance values.
void calculateDistances() {

  // Define interrupt routines to call for each echo pin
  attachInterrupt(digitalPinToInterrupt(leftEchoPin), ISR_Sensor1_onPin4_Interrupt0, CHANGE );
  attachInterrupt(digitalPinToInterrupt(forwardEchoPin), ISR_Sensor2_onPin7_Interrupt1, CHANGE );
  attachInterrupt(digitalPinToInterrupt(rightEchoPin), ISR_Sensor3_onPin8_Interrupt2, CHANGE );

  // Trigger Sensors (no need to have the initial LOW because this function already takes up time
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); // OK to use delayMicrosecond
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); // // OK to use delayMicrosecond - 10 us is enough!
  digitalWrite(triggerPin, LOW);    // End of pulse

  //
  // Wait until all three sensors are read. This IS blocking, but OK.
  //
  // Take note of time right when interrupts start to avoid timeouts:
  // The farthest wall is at most about 81" away (see timeout variable)
  unsigned long timeWhenInterruptsStart = micros();

  // Loop as long until all 3 sensors measured a distance, and/or timeouts reached
  while ( (leftSensorIsRead * forwardSensorIsRead * rightSensorIsRead) == false)
  {
    if ((micros() - timeWhenInterruptsStart) >=  timeout) {
      // This means that we're in a timeout! One IRQ or more didn't trigger, so let's stop waiting
      // Let's check if one or more sensor didn't read a distance, and if so set it to 0

      if (leftSensorIsRead == false) {// Left sensor didn't measure in time
        detachInterrupt(digitalPinToInterrupt(leftEchoPin));
        pulseTravelTime[0] = 0;
        leftSensorIsRead = true;
      }
      if (forwardSensorIsRead == false) {// Forward sensor didn't measure in time
        detachInterrupt(digitalPinToInterrupt(forwardEchoPin));
        pulseTravelTime[1] = 0;
        forwardSensorIsRead = true;
      }
      if (rightSensorIsRead == false) {// Right sensor didn't measure in time
        detachInterrupt(digitalPinToInterrupt(rightEchoPin));
        pulseTravelTime[2] = 0;
        rightSensorIsRead = true;
      }
    }
  }

  // clear flags so we can read the sensors again later
  leftSensorIsRead = false;
  forwardSensorIsRead = false;
  rightSensorIsRead = false;

  // calculate distances
  for (int k = 0; k < numInterrupts; k++)
  {
    distanceToObject[k] = (pulseTravelTime[k] / 2.0) * (float)soundSpeed / 10000.0; // Calculate distances
  }

}



//
// Interrupt routines, one for each sensor echo pin
//
void ISR_Sensor1_onPin4_Interrupt0() // Left sensor
{
  if (leftSensorIsRead == false) {
    byte readEchoPin = bitRead(PORTC.IN, 6); //  leftEchoPin = 4;
    mainInterruptFunction(readEchoPin, 0);
    if (pulseIsMeasured) { // left sensor value has been read
      // Disable interrupts on left sensor
      detachInterrupt(digitalPinToInterrupt(leftEchoPin));
      leftSensorIsRead = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

void ISR_Sensor2_onPin7_Interrupt1() // Forward sensor
{
  if ( forwardSensorIsRead == false) {
    byte readEchoPin = bitRead(PORTA.IN, 1); // forwardEchoPin = 7;
    mainInterruptFunction(readEchoPin, 1);
    if (pulseIsMeasured) { // forward sensor value has been read
      // Disable interrupts on forward sensor
      detachInterrupt(digitalPinToInterrupt(forwardEchoPin));
      forwardSensorIsRead = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

void ISR_Sensor3_onPin8_Interrupt2() // Right Sensor
{
  if ( rightSensorIsRead == false) {
    byte readEchoPin = bitRead(PORTE.IN, 3); //rightEchoPin = 8;
    mainInterruptFunction(readEchoPin, 2);
    if (pulseIsMeasured) { // right sensor value has been read
      // Disable interrupts on right sensor
      detachInterrupt(digitalPinToInterrupt(rightEchoPin));
      rightSensorIsRead = true;
      // reset pulseIsMeasured to false
      pulseIsMeasured = false;
    }
  }
}

//
// Main Interrupt function
//
void mainInterruptFunction(bool stateOfPin, int IRQ_Number)
{
  currentTime = micros();
  if (stateOfPin)
  {
    // If the pin measured had its state change to HIGH, then save that time (us)!
    timeEchostateOfPinChangedToHigh[IRQ_Number] = currentTime;
  }
  else
  {
    // If the pin measured had its state change to LOW, then calculate how much time has passed (µs)
    pulseTravelTime[IRQ_Number] = currentTime - timeEchostateOfPinChangedToHigh[IRQ_Number];

    // this is the place where the pulse is measured, and so returns true to the ISR that called mainInterruptFunction
    pulseIsMeasured = true;
  }
}

float measure_front_US() {
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

   return distToObjectInCM;
}

void move_forward_until(float distance) {
  move_L_servo(FORWARD); // set left servo to move forward
  move_R_servo(FORWARD); // set right servo to move forward

  // wait until front ultrasonic sensor reads a value below the threshold distance
  while(measure_front_US() > distance && measure_front_US() != 0) {}

  // stop bot
  bot_stop();
}

// used for debugging only
void print_distances() {
  // measure each of the ultrasonic sensors
  calculateDistances();

  // print results
  Serial.println(String(distanceToObject[0]) + " cm : " + String(distanceToObject[1]) + " cm : " + String(distanceToObject[2]) + " cm");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Trigger pin is OUTPUT, all Echo pins are input
  pinMode(triggerPin, OUTPUT);
  pinMode(leftEchoPin, INPUT);
  pinMode(forwardEchoPin, INPUT);
  pinMode(rightEchoPin, INPUT);
  
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo

  pinMode(triggerPIN, OUTPUT); // configure trigger pin for front ultrasonic sensor
  pinMode(echoPIN, INPUT); // configure echo pin for front ultrasonic sensor
}

void loop() {
  // put your main code here, to run repeatedly:

  // sensor threshold we use
  // the robot will move until it is this far or less from the wall in front of it
  // we found by experimentation that this results in the bot stopping in the middle of the square
  float distance_to_wall = 12.5;

  // remain motionless for 1 second
  delay(1000);

  // print robot status and sensor readings
  Serial.print("At position 1 facing North, ");
  print_distances();

  // print robot status
  Serial.println("Moving to position 2");

  // move to point 2
  move_forward_until(distance_to_wall);

  // print robot status and sensor readings
  Serial.print("Stopped at position 2 facing North, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status
  Serial.println("Turning left");

  // turn left to face West
  turn_L();

  // pause before continuing to let servos stop
  delay(250);

  // print robot 
  Serial.println("Moving to position 3");

  // move to point 3
  move_forward_until(distance_to_wall);

  // print robot status and sensor readings
  Serial.print("Stopped at position 3 facing West, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Turning left");

  // turn left to face South
  turn_L();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Moving to position 4 ");

  // move to point 4
  move_forward_until(distance_to_wall);

  // print robot status and sensor readings
  Serial.print("Stopped at position 4 facing South, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Turning 180 degrees right");

  // turn 180 degrees to face North
  turn_R();
  delay(100);
  turn_R();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Moving to position 3");

  // move to point 3
  move_forward_until(distance_to_wall);

  // print robot status and sensor readings
  Serial.print("Stopped at position 3 facing North, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Turning right");

  // turn right to face East
  turn_R();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Moving to position 2");

  // move to point 2 
  move_forward_until(distance_to_wall);

  // print robot status and sensor readings
  Serial.print("Stopped at position 2 facing East, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Turning right");

  // turn right to face South
  turn_R();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Moving to position 1");

  // move to point 1
  move_L_servo(FORWARD); // turn on left servo
  move_R_servo(FORWARD); // turn on right servo
  delay(2000); // wait for bot to reach point 1
  bot_stop(); // stop bot at point 1

  // print robot status and sensor readings
  Serial.print("Stopped at position 1 facing South, ");
  print_distances();

  // pause before continuing to let servos stop
  delay(250);

  // print robot status 
  Serial.println("Turning left 540 degrees");

  // turn left 540 degrees
  turn_L(); delay(100); // turn 90 degrees and pause
  turn_L(); delay(100); // turn 90 degrees and pause
  turn_L(); delay(100); // turn 90 degrees and pause
  turn_L(); delay(100); // turn 90 degrees and pause
  turn_L(); delay(100); // turn 90 degrees and pause
  turn_L(); // turn 90 degrees, totalling 540

  // turn 90 degrees 
  Serial.println("Done!");

  // stop forever
  while(1) {}
}
