const int touchPin =4;
int readPinState;
long tempTime = 0;//버튼이 눌린 시간 저장
void setup() {
  Serial.begin(115200);
}

void loop() {
  readPinState = touchRead(touchPin);
  Serial.println(readPinState);
  delay(100);
}
