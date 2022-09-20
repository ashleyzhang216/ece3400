#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A1
#define R_SERVO_FB A2

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

void bot_move(move_dir dir, int power, float distance_cm) {
  float degrees_to_turn = distance_cm / (PI * wheel_diam_cm) * 360;

  Serial.println("bot_move() turning " + String(degrees_to_turn) + " degrees");
  
  int fb_threshold_tracker = 0;
  int starting_fb_vals[] = {analogRead(L_SERVO_FB), analogRead(R_SERVO_FB)};
  int prev_fb_readings[2];

  move_L_servo(dir, power);
  move_R_servo(dir, power);

  if(dir == FORWARD) {
    if(analogRead(R_SERVO_FB) != 1023) {




      
    } else if(analogRead(L_SERVO_FB != 1023) {





      
    }
    
  } else {







    
  }

  /*
  if(dir == FORWARD) {
    while(analogRead(R_SERVO_FB) + fb_threshold_tracker - starting_fb_val < (degrees_to_turn * (max_servo_fb/360))) {
      if(analogRead(R_SERVO_FB) < prev_fb_reading) {
        fb_threshold_tracker += max_servo_fb;
      }
      prev_fb_reading = analogRead(R_SERVO_FB);
    }  
  } else {
    while((max_servo_fb - analogRead(R_SERVO_FB)) + fb_threshold_tracker - starting_fb_val < (degrees_to_turn * (max_servo_fb/360))) {
      if(analogRead(R_SERVO_FB) > prev_fb_reading) {
        fb_threshold_tracker += max_servo_fb;
      }
      prev_fb_reading = analogRead(R_SERVO_FB);
    }
  }*/

  bot_stop();
}

void bot_turn(turn_dir dir, int power, float turn_degrees) {
  
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
  
  Serial.println("moving servos forward");
  move_L_servo(FORWARD, 100);
  move_R_servo(FORWARD, 100);

  for(int i = 0; i < 100; i++) {
    print_servo_fb();
    delay(30);
  }

  Serial.println("moving servos backward");
  move_L_servo(BACKWARD, 100);
  move_R_servo(BACKWARD, 100);

  for(int i = 0; i < 100; i++) {
    print_servo_fb();
    delay(30);
  }

  Serial.println("stopping servos");
  bot_stop();
  delay(3000);

  Serial.println("moving forward 20cm");
  bot_move(FORWARD, 50, 20);

  Serial.println("moving backward 20cm");
  bot_move(BACKWARD, 50, 20);

  Serial.println("done!");

  while(1) {}
}
