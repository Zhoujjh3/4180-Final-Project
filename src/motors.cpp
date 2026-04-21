#include "motors.h"

void motorSetup() {
  ledcAttach(SHUFFLE_MOTOR1_AIN1, PWM_FREQ, PWM_RES);
  ledcAttach(SHUFFLE_MOTOR1_AIN2, PWM_FREQ, PWM_RES);
  ledcAttach(SHUFFLE_MOTOR2_AIN1, PWM_FREQ, PWM_RES);
  ledcAttach(SHUFFLE_MOTOR2_AIN2, PWM_FREQ, PWM_RES);
}

void motor1Speed(int speed) {
  if (speed >= 0) {
    ledcWrite(SHUFFLE_MOTOR1_AIN1, speed);
    ledcWrite(SHUFFLE_MOTOR1_AIN2, 0);
  } else {
    ledcWrite(SHUFFLE_MOTOR1_AIN1, 0);
    ledcWrite(SHUFFLE_MOTOR1_AIN2, -speed);
  }
}

void motor2Speed(int speed) {
  if (speed >= 0) {
    ledcWrite(SHUFFLE_MOTOR2_AIN1, speed);
    ledcWrite(SHUFFLE_MOTOR2_AIN2, 0);
  } else {
    ledcWrite(SHUFFLE_MOTOR2_AIN1, 0);
    ledcWrite(SHUFFLE_MOTOR2_AIN2, -speed);
  }
}
