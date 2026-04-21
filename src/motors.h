#pragma once
#include <Arduino.h>

#define SHUFFLE_MOTOR1_AIN1 4
#define SHUFFLE_MOTOR1_AIN2 5
#define SHUFFLE_MOTOR2_AIN1 6
#define SHUFFLE_MOTOR2_AIN2 7

#define PWM_FREQ 1000
#define PWM_RES  8

void motorSetup();
void motor1Speed(int speed);
void motor2Speed(int speed);
