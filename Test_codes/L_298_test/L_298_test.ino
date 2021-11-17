const int enable_pinA = 4;
const int motorA_inputA = 18;
const int motorA_inputB = 19; 

void setup(){
  ledcAttachPin(enable_pinA, 0);
  pinMode(motorA_inputA, OUTPUT);
  pinMode(motorA_inputB, OUTPUT);
}

void loop(){
  digitalWrite(motorA_inputA, LOW);
  digitalWrite(motorA_inputB, LOW);
  delay(5000);
  
  digitalWrite(motorA_inputA, HIGH);
  digitalWrite(motorA_inputB, LOW);
  delay(10000); 
}
