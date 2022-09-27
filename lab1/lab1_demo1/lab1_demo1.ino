#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A2
#define R_SERVO_FB A1

// enum to describe movement directions of servos
enum dir {
  FORWARD, 
  BACKWARD
};

// Create the servo objects
Servo L_servo;
Servo R_servo;

// Global variables for reading feedback value
int L_servo_fb;
int R_servo_fb;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT); // configure onboard LED to be able to turn on

  Serial.begin(9600); // allow for debug console
  
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
  
  L_servo.write(90); // Stop left servo
  R_servo.write(90); // Stop right servo
}

void print_servo_fb() {
  L_servo_fb = analogRead(L_SERVO_FB); // read feedback value from left servo
  R_servo_fb = analogRead(R_SERVO_FB); // read feedback value from right servo

  Serial.println("Left " + String(L_servo_fb) + " : " + " Right " + String(R_servo_fb)); // print out feedback values
}

// move left servo in specified direction at specified power
void move_L(dir d, int power) {
  assert(power >= 0 && power <= 100);
  if(d == FORWARD) {
    L_servo.write(90 - 90*(power/100.0));
  } else if(d == BACKWARD) {
    L_servo.write(90 + 90*(power/100.0));
  } else {
    assert(false);
  }
}

// move right servo in specified direction at specified power
void move_R(dir d, int power) {
  assert(power >= 0 && power <= 100);
  if(d == FORWARD) {
    R_servo.write(90 + 90*(power/100.0));
  } else if(d == BACKWARD) {
    R_servo.write(90 - 90*(power/100.0));
  } else {
    assert(false);
  }
}

// make the robot turn left 90 degrees
void turn_L() {
  move_L(BACKWARD, 25);
  move_R(FORWARD, 25);

  delay(1750);

  L_servo.write(90);
  R_servo.write(90);
}

// make the robot turn right 90 degrees
void turn_R() {
  move_L(FORWARD, 25);
  move_R(BACKWARD, 25);

  delay(1020);

  L_servo.write(90);
  R_servo.write(90);
}

// stop both the left and right servos
void stop_LR() {
  L_servo.write(90);
  R_servo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:

  // stay motionless for 5 seconds
  delay(5000);

  // turn on both servos to move forward
  move_L(FORWARD, 50);
  move_R(FORWARD, 50);

  // wait for bot to travel 20 cm
  delay(2000);

  // stop the bot
  stop_LR();

  // pause 1 second
  delay(1000);

  // turn 90 degrees right
  turn_R();

  // pause 1 second
  delay(1000);

  // pivot 270 degrees left, by pivoting 90 degrees 3 times
  turn_L();
  turn_L();
  turn_L();

  // pause 1 second
  delay(1000);

  // turn on both servos to move forward
  move_L(FORWARD, 50);
  move_R(FORWARD, 50);

  // wait for bot to travel 20 cm
  delay(2000);

  // stop the bot
  stop_LR();

  // pause 1 second
  delay(1000);

  // pivot right 180 degrees by pivoting 90 degrees right twice
  turn_R();
  turn_R();

  // stop, forever
  while(1) {}
}
