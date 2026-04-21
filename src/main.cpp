#include <Arduino.h>
#include "motors.h"

void setup() {
  Serial.begin(115200);
  motorSetup();
  Serial.println("Motor test starting...");
}

void loop() {
  Serial.println("Forward 255");
  motor1Speed(255);
  motor2Speed(255);
  delay(2000);

  Serial.println("Stop");
  motor1Speed(0);
  motor2Speed(0);
  delay(1000);

  Serial.println("Reverse -255");
  motor1Speed(-255);
  motor2Speed(-255);
  delay(2000);

  Serial.println("Stop");
  motor1Speed(0);
  motor2Speed(0);
  delay(1000);
}
