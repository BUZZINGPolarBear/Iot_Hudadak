#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ESP32_Servo.h>
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
const int secondary_touchPin = 15;
const int servoPin = 23;

bool isWaterPumpActivated = false;
bool isHumidPumpActivated = false;
int temp=40;
int humid=70;
int sound;
int touchPin=70;
int depth = 0;
String str_temp="off";
String str_humid="off";
String str_water="off";
String str_feed="off";
unsigned long long humidPumpNow=0;
unsigned long long waterPumpNow=0;
long long gotTopic=0;
int readPinState=0;

int humidPumpThreshold = 10000;
int waterPumpThreshold = 3000;

Servo servo1;

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
  servo1.attach(servoPin);
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
    JSONVar userSelected = myObj["userSelected"];
    str_temp = userSelected["temp"];
    str_humid = userSelected["humid"];
    str_water = userSelected["water"];
    str_feed = userSelected["feed"];
    depth = state ["depth"];

    Serial.println("state: "+ state);
    Serial.println("temp: " + temp);
    Serial.println("humid: " + humid);
    Serial.println("sound: " + sound);
    Serial.println("touchPin: " + touchPin);
    
    Serial.println("str_temp: " + str_temp);
    Serial.println("str_humid: " + str_humid);
    Serial.println("str_water: " + str_water);
    Serial.println("str_feed: " + str_feed);
    Serial.println("depth: " + depth);
    if(temp<30)digitalWrite(relayModule, LOW);//ON
    else digitalWrite(relayModule, HIGH);//OFF
    
    if((humid<50&&isHumidPumpActivated==false) || str_humid=="on")
    {
        digitalWrite(humidPumpA, 1);
        digitalWrite(relayModule, 0);
        humidPumpNow = millis();
    }
    else if(humid>=50 || str_humid=="off")
    {
      digitalWrite(humidPumpA,0);
      humidPumpNow = millis();
    }
    if(touchPin>=40 || str_water=="on")
    {
      //humidPumpNow = millis();
      digitalWrite(waterPumpA,1);
      while(1)
      {
        readPinState = touchRead(secondary_touchPin);
        Serial.println(readPinState);
        if(readPinState<40){
          digitalWrite(waterPumpA,0);
          break;
        } 
      }      
    }
    if(depth<15 || str_feed=="on")
    {
      for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servo1.write(posDegrees); // 모터의 각도를 설정합니다.
        //Serial.println(posDegrees);
        delay(20);
      }
      for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees); // 모터의 각도를 설정합니다.
        //Serial.println(posDegrees);
        delay(20);
      }
    }
  }  
}
