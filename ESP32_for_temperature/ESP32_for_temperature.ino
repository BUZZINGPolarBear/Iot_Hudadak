#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
AWS_IOT testButton;
#include <LiquidCrystal_I2C.h>

#define BME_CS 5 // cs for esp32 vspi
#define SEALEVELPRESSURE_HPA (1013.25)


const int touchPin =4;
int readPinState;
const int trigPin = 26;
const int echoPin = 25;

const char* ssid = "Juni WIFI";
const char* password = "wnsgnlRj";
char HOST_ADDRESS[] = "a3llcbaumch20d-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "ESP32ForTemperature";
char sTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update/delta"; // subscribe topic name
char pTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update"; // publish topic name

int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
const int buttonPin = 15; // pushbutton pin
const int soundModule = 34;
unsigned long preMil = 0;
const long intMil = 5000; 
int delayTime;
int lcdColumns = 16;
int lcdRows = 2;

Adafruit_BME280 bme(BME_CS); // hardware SPI
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  bool status;
  // default settings
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
  else {
    Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
    while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
  status = bme.begin();
  lcd.begin();
}

void loop() 
{
  printValues();
}

void printValues()
{
  int temperature = bme.readTemperature();
  int humid = bme.readHumidity();
  lcd_print_status(temperature, humid, 5000);
  
} 

void lcd_print(String first_line_print, String second_line_print, int delay_time)
{
  lcd.setCursor(0,0);
  lcd.print(first_line_print);
  lcd.setCursor(0,1);
  lcd.print(second_line_print);
  delay(delay_time);
}

void lcd_print_status(int temperature, int humid, int delay_time)
{
  lcd.setCursor(0,0);
  lcd.print("temp: " + String(temperature)+"*C");
  //lcd.print("온도: " + String(temperature)+"*C");
  lcd.setCursor(0,1);
  lcd.print("humid: " + String(humid)+"%");
  //lcd.print("습도: " + String(humid)+"%");
  delay(delay_time);
  //publishStatusTopic(temperature, humid);
  TESTpublishStatusTopic(temperature, humid);
}

void publishStatusTopic(int temperature, int humid)
{
  int chickSound = analogRead(soundModule);

  String temp = "{\"state\":{\"reported\": {\"temp\":" + String(temperature) + ",\"humid\":" + String(humid)+ ",\"sound\":" + String(chickSound) + "}}}";
  Serial.println(temp);
  char toChar[1000];
  strcpy(toChar, temp.c_str());
  sprintf(payload,toChar);
  if(testButton.publish(pTOPIC_NAME,payload) == 0) {
    Serial.print("Publish Message:");
    Serial.println(payload);
  }
  else
    Serial.println("Publish failed");
}

void TESTpublishStatusTopic(int temperature, int humid)
{

  long duration, distance;
  digitalWrite(trigPin, LOW); // trig low for 2us
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // trig high for 10us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 17 / 1000;
  readPinState = touchRead(touchPin);

  int chickSound = analogRead(soundModule);
  String temp = "{\"state\":{\"reported\": {\"temp\":" + String(temperature) + ",\"humid\":" + String(humid)+ ",\"sound\":" + String(chickSound)+ ",\"depth\":" + String(distance)+ ",\"touchPin\":" + String(readPinState) + "}}}";
  Serial.println(temp);
  char toChar[1000];
  strcpy(toChar, temp.c_str());
  sprintf(payload,toChar);
  Serial.print("Publish Message:");
  Serial.println(payload);
}
