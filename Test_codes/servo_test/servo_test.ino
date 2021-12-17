#include <ESP32_Servo.h>

static const int servoPin = 23;

Servo servo1;

void setup() {
    Serial.begin(115200);
    servo1.attach(servoPin); // D27에 서보모터가 연결되었습니다.
    servo1.write(2);
    delay(2000);
}

void loop() {
   servo1.write(120);
   delay(10000);
   servo1.write(0);
   delay(10000);
}
