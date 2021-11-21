#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
AWS_IOT testButton;

const char* ssid = "Juni WIFI";
const char* password = "wnsgnlRj";
char HOST_ADDRESS[] = "a3llcbaumch20d-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "ChoiESP32";
char sTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update/delta"; // subscribe topic name
char pTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update"; // publish topic name

int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
const int buttonPin = 15; // pushbutton pin
const int ledPin=16;
unsigned long preMil = 0;
const long intMil = 5000;
const int relayModule = 2;
const int waterPumpA=18;
const int waterPumpB=19;
const int humidPumpA=22;
const int humidPumpB=23;

bool isWaterPumpActivated = 0;
bool isHumidPumpActivated = 0;
int temp;
int humid;
int sound;
int touchPin;
long long gotTopic=0;

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);
  //++choi This is here to force the ESP32 to reset the WiFi and initialize correctly.
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode()); //++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
    if(0==testButton.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
      Serial.println("Subscribe Successfull");
    }
    else { // aws 연결 실패시
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relayModule, OUTPUT);
  pinMode(waterPumpA, OUTPUT);
  pinMode(humidPumpA, OUTPUT);
  delay(2000);
}

void loop() {
  if(msgReceived == 1)
  {
    gotTopic++;
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.println(rcvdPayload);
    // Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);
    JSONVar state = myObj["state"];
    temp = state ["temp"];
    humid = state ["humid"];
    sound = state ["depth"];
    touchPin = state ["touchPin"];

    Serial.println(state);
    Serial.println(temp);
    Serial.println(humid);
    Serial.println(sound);
    Serial.println(touchPin);
    if(temp<30)
    {
      digitalWrite(relayModule, LOW);//ON
    }
    else 
    {
      digitalWrite(relayModule, HIGH);//OFF
    }
  } 
  if(humid<50)
  {
    ActivateHumidWaterPump(1500);
    humid = 60;
  }
  
  if(touchPin<38)
  {
    ActivateWaterPump(3000);
    touchPin = 40;
  }
}

void ActivateHumidWaterPump(int delayTime)
{
  digitalWrite(humidPumpA, HIGH);
  delay(delayTime);
  digitalWrite(humidPumpA, LOW);
  delay(3000);
}

void ActivateWaterPump(int delayTime)
{
    digitalWrite(waterPumpA, HIGH);
    delay(3000);
    digitalWrite(waterPumpA, LOW);
    delay(3000);
}
