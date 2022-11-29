#include <Servo.h>
#include <Assert.h>

#define L_SERVO 5
#define R_SERVO 6

#define L_SERVO_FB A0
#define R_SERVO_FB A1

#define P_COEFF 1.0
#define I_COEFF 0

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

void stop_servos() {
  L_servo.write(90);
  R_servo.write(90);
}

void move_L_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;

  if(dir == FORWARD) {
    L_servo.write(90 - 90*(power/100.0));
  } else {
    L_servo.write(90 + 90*(power/100.0));
  }
}

void move_R_servo(move_dir dir, int power) {
  if(power < 0) power = 0;
  if(power > 100) power = 100;
  
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

void move_forward_pid(float distance_cm) {
  int target_enc_val = (distance_cm / wheel_diam_cm) * max_servo_fb;
  
  int error;
  int p_error = 0;
  
  int l_traveled = 0;
  int r_traveled = 0;
  int prev_l = analogRead(L_SERVO_FB);
  int prev_r = analogRead(R_SERVO_FB);

  Serial.println("Target: " + String(target_enc_val));

  while(l_traveled < target_enc_val && r_traveled < target_enc_val) {
    if(prev_l > 600 && analogRead(L_SERVO_FB) < 200) {
      l_traveled += analogRead(L_SERVO_FB) + max_servo_fb - prev_l;
      Serial.println("Reset: " + String(analogRead(L_SERVO_FB)) + ", " + String(prev_l) + ", " + String(l_traveled));
    } else {
      l_traveled += analogRead(L_SERVO_FB) - prev_l;
      Serial.println("Norml: " + String(analogRead(L_SERVO_FB)) + ", " + String(prev_l) + ", " + String(l_traveled));
    }

    if(prev_r < 600 && analogRead(R_SERVO_FB) > 200) {
      r_traveled += prev_l + max_servo_fb - analogRead(R_SERVO_FB);
    } else {
      r_traveled += prev_l - analogRead(R_SERVO_FB);
    }

    prev_l = analogRead(L_SERVO_FB);
    prev_r = analogRead(R_SERVO_FB);

    error = r_traveled - l_traveled;
    p_error += error;

    move_L_servo(FORWARD, 50 + (P_COEFF * error) + (I_COEFF * p_error));
    move_R_servo(FORWARD, 50 - (P_COEFF * error) - (I_COEFF * p_error));
    
    //Serial.println("Left: " + String(analogRead(L_SERVO_FB)) + ", " + String(l_traveled) + " - Right: " + String(analogRead(R_SERVO_FB)) + ", " + String(r_traveled));
  }

  stop_servos();
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

  move_forward_pid(50.0);
  Serial.println("Done!");
  while(1) {}
}
