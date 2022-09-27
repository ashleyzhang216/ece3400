// This code triggers 3 Ultrasonic sensors at once (their trigger is connected
// together), waits for the interrupts to happen for the Echo of each US, and/or
// stops after the timeout value. The distance measured by each sensor is then displayed
// with a 0 for a timed out sensor.

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


//Setup
void setup() {
  Serial.begin(115200); // Serial Monitor

  // Trigger pin is OUTPUT, all Echo pins are input
  pinMode(triggerPin, OUTPUT);
  pinMode(leftEchoPin, INPUT);
  pinMode(forwardEchoPin, INPUT);
  pinMode(rightEchoPin, INPUT);
}


void loop()
{
  // Read US sensors ON demand: only when I call the following function will
  // the US Sensors be used!
  calculateDistances();

  // Display distances
  for (int k = 0; k < numInterrupts; k++) {
    Serial.print(distanceToObject[k]);
    Serial.print("; ");
  }
  Serial.println();
  Serial.println();
  Serial.println();

  delay(2000); // So that the Serial Monitor displays things not too fast.
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
