const int relayModule = 18;
bool status=1;

void setup() {
  // put your setup code here, to run once:
  //pinMode(relayModule, OUTPUT);
  digitalWrite(relayModule, 0);
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(relayModule, status);
  if(status) Serial.println("켜진다");
  else Serial.println("꺼진다");
  delay(5000);
  
  digitalWrite(relayModule, !status);
  if(status) Serial.println("켜진다");
  else Serial.println("꺼진다");
  delay(5000);
}
