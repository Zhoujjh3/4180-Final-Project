#include "motors.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel statusLED(1, STATUS_LED_PIN, NEO_GRB + NEO_KHZ800);

void setStatusLED(uint8_t r, uint8_t g, uint8_t b) {
    statusLED.setPixelColor(0, statusLED.Color(r, g, b));
    statusLED.show();
}

void motorSetup() {
    statusLED.begin();
    statusLED.setBrightness(50);
    setStatusLED(0, 0, 255);   // blue = booting

    pinMode(SHUFFLE_MOTOR1_AIN1, OUTPUT);
    pinMode(SHUFFLE_MOTOR1_AIN2, OUTPUT);
    pinMode(SHUFFLE_MOTOR2_AIN1, OUTPUT);
    pinMode(SHUFFLE_MOTOR2_AIN2, OUTPUT);
    pinMode(OUTPUT_MOTOR_AIN1,   OUTPUT);
    pinMode(OUTPUT_MOTOR_AIN2,   OUTPUT);

    ledcAttach(PWM_1, PWM_FREQ, PWM_RES);
    ledcAttach(PWM_2, PWM_FREQ, PWM_RES);
    ledcAttach(PWM_O, PWM_FREQ, PWM_RES);

    motor1Speed(0);
    motor2Speed(0);
    outputMotorSpeed(0);

    setStatusLED(0, 255, 0);   // green = ready
    Serial.println("Motors ready");
}

void motor1Speed(int speed) {
    speed = constrain(speed, -255, 255);
    if (speed > 0) {
        digitalWrite(SHUFFLE_MOTOR1_AIN1, HIGH);
        digitalWrite(SHUFFLE_MOTOR1_AIN2, LOW);
    } else if (speed < 0) {
        digitalWrite(SHUFFLE_MOTOR1_AIN1, LOW);
        digitalWrite(SHUFFLE_MOTOR1_AIN2, HIGH);
    } else {
        digitalWrite(SHUFFLE_MOTOR1_AIN1, LOW);
        digitalWrite(SHUFFLE_MOTOR1_AIN2, LOW);
    }
    ledcWrite(PWM_1, abs(speed));
}

void motor2Speed(int speed) {
    speed = constrain(speed, -255, 255);
    if (speed > 0) {
        digitalWrite(SHUFFLE_MOTOR2_AIN1, HIGH);
        digitalWrite(SHUFFLE_MOTOR2_AIN2, LOW);
    } else if (speed < 0) {
        digitalWrite(SHUFFLE_MOTOR2_AIN1, LOW);
        digitalWrite(SHUFFLE_MOTOR2_AIN2, HIGH);
    } else {
        digitalWrite(SHUFFLE_MOTOR2_AIN1, LOW);
        digitalWrite(SHUFFLE_MOTOR2_AIN2, LOW);
    }
    ledcWrite(PWM_2, abs(speed));
}

void outputMotorSpeed(int speed) {
    speed = constrain(speed, -255, 255);
    if (speed > 0) {
        digitalWrite(OUTPUT_MOTOR_AIN1, HIGH);
        digitalWrite(OUTPUT_MOTOR_AIN2, LOW);
        setStatusLED(255, 165, 0);  // orange = dealing
    } else if (speed < 0) {
        digitalWrite(OUTPUT_MOTOR_AIN1, LOW);
        digitalWrite(OUTPUT_MOTOR_AIN2, HIGH);
        setStatusLED(255, 165, 0);  // orange = dealing reverse
    } else {
        digitalWrite(OUTPUT_MOTOR_AIN1, LOW);
        digitalWrite(OUTPUT_MOTOR_AIN2, LOW);
        setStatusLED(0, 255, 0);    // green = idle
    }
    ledcWrite(PWM_O, abs(speed));
}