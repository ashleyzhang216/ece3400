#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A2
#define R_SERVO_FB A1

enum dir {
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

// Global variables for reading feedback value
int L_servo_fb;
int R_servo_fb;

const float robot_diam = 5.75;
const float wheel_diam = 2.75;

const int max_fb_value = 720;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  
  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
  
  L_servo.write(90); // Stop left servo
  R_servo.write(90); // Stop right servo
}

void print_servo_fb() {
  L_servo_fb = analogRead(L_SERVO_FB);
  R_servo_fb = analogRead(R_SERVO_FB);

  Serial.println("Left " + String(L_servo_fb) + " : " + " Right " + String(R_servo_fb));
}

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

// left increasing right decreasing

// UNUSED
void turn(turn_dir d, int power, int turn_deg) {

  int wheel_turn_deg = PI*robot_diam*(turn_deg/360.0)/wheel_diam*360;
  
  if(d == LEFT) {
    L_servo.write(90);
    R_servo.write(90);

    
  } else { //d == RIGHT
    
  }
}

void turn_L() {
  move_L(BACKWARD, 25);
  move_R(FORWARD, 25);

  delay(1750);

  L_servo.write(90);
  R_servo.write(90);
}

void turn_R() {
  move_L(FORWARD, 25);
  move_R(BACKWARD, 25);

  delay(1020);

  L_servo.write(90);
  R_servo.write(90);
}

void stop_LR() {
  L_servo.write(90);
  R_servo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(5000);

  move_L(FORWARD, 50);
  move_R(FORWARD, 50);

  delay(2000);

  stop_LR();

  delay(1000);

  turn_R();

  delay(1000);

  turn_L();
  turn_L();
  turn_L();

  delay(1000);

  move_L(FORWARD, 50);
  move_R(FORWARD, 50);

  delay(2000);

  stop_LR();

  delay(1000);

  turn_R();
  turn_R();

  while(1) {}
}
