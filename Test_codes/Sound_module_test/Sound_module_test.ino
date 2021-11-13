const int pump = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pump, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pump, HIGH);
  delay(7000);
  digitalWrite(pump, LOW);
  delay(3000);
}
