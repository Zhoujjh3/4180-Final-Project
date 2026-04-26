#pragma once
#include <Arduino.h>

#define SHUFFLE_MOTOR1_AIN1  0
#define SHUFFLE_MOTOR1_AIN2  1
#define PWM_1                3

#define SHUFFLE_MOTOR2_AIN1  10
#define SHUFFLE_MOTOR2_AIN2  11
#define PWM_2                2

#define OUTPUT_MOTOR_AIN1    19
#define OUTPUT_MOTOR_AIN2    18
#define PWM_O                21

#define STATUS_LED_PIN       23  // plain LED + 330Ω to GND

#define PWM_FREQ  1000
#define PWM_RES   8

void motorSetup();
void motor1Speed(int speed);
void motor2Speed(int speed);
void outputMotorSpeed(int speed);
void setStatusLED(bool on);