#include <Arduino.h>

//motor1 setup
#define shuffle_motor1_ain1 4
#define shuffle_motor1_ain2 5

//motor2 setup
#define shuffle_motor2_ain1 6
#define shuffle_motor2_ain2 7

#define pwm_freq 1000
#define pwm_res 8



// put function declarations here:
void motor1Speed(int speed);
void motor2Speed(int speed);

void setup() {
  Serial.begin(115200);

  ledcAttach(shuffle_motor1_ain1, pwm_freq, pwm_res);
  ledcAttach(shuffle_motor1_ain2, pwm_freq, pwm_res);

  ledcAttach(shuffle_motor2_ain1, pwm_freq, pwm_res);
  ledcAttach(shuffle_motor2_ain2, pwm_freq, pwm_res);

  Serial.println("Motor test starting...");
}

void loop() {
  // Forward full speed
  Serial.println("Forward 255");
  motor1Speed(255);
  motor2Speed(255);
  delay(2000);

  // Stop
  Serial.println("Stop");
  motor1Speed(0);
  motor2Speed(0);
  delay(1000);

  // Reverse full speed
  Serial.println("Reverse -255");
  motor1Speed(-255);
  motor2Speed(-255);
  delay(2000);

  // Stop
  Serial.println("Stop");
  motor1Speed(0);
  motor2Speed(0);
  delay(1000);
}

//function definitions

void motor1Speed(int speed) {
  //255 is max speed
  if (speed >= 0) {
    ledcWrite(shuffle_motor1_ain1, speed);
    ledcWrite(shuffle_motor1_ain2, 0);
  } else {
    ledcWrite(shuffle_motor1_ain1, 0);
    ledcWrite(shuffle_motor1_ain2, -speed);
  }
  
}

void motor2Speed(int speed) {
  //255 is max speed
  if (speed >= 0) {
    ledcWrite(shuffle_motor2_ain1, speed);
    ledcWrite(shuffle_motor2_ain2, 0);
  } else {
    ledcWrite(shuffle_motor2_ain1, 0);
    ledcWrite(shuffle_motor2_ain2, -speed);
  }
}
