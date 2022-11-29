#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A0
#define R_SERVO_FB A1

enum move_dir {
  FORWARD, 
  BACKWARD
};

enum turn_dir {
  LEFT, 
  RIGHT
};

const float wheel_diam_cm = 7.0;
const float bot_diam_cm = 147.5;

const int max_servo_fb = 720;

// Create the servo objects
Servo L_servo;
Servo R_servo;

void bot_stop() {
  L_servo.write(90);
  R_servo.write(90);
}

void move_L_servo(move_dir dir, int power) {
  assert(power >= 0 && power <= 100);
  if(dir == FORWARD) {
    L_servo.write(90 - 90*(power/100.0));
  } else {
    L_servo.write(90 + 90*(power/100.0));
  }
}

void move_R_servo(move_dir dir, int power) {
  assert(power >= 0 && power <= 100);
  if(dir == FORWARD) {
    R_servo.write(90 + 90*(power/100.0));
  } else {
    R_servo.write(90 - 90*(power/100.0));
  }
}

void print_servo_fb() {
  int L_servo_fb = analogRead(L_SERVO_FB);
  int R_servo_fb = analogRead(R_SERVO_FB);

  Serial.println("Left " + String(L_servo_fb) + " : " + " Right " + String(R_servo_fb));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  L_servo.attach(L_SERVO); // Attach left servo to its pin
  R_servo.attach(R_SERVO); // Attach right servo to its pin
  
  pinMode(L_SERVO_FB, INPUT); // used for the feedback from left servo
  pinMode(R_SERVO_FB, INPUT); // used for the feedback from right servo
}

void loop() {
  // put your main code here, to run repeatedly:

  move_L_servo(FORWARD, 100);
  Serial.println(analogRead(L_SERVO_FB));
}
