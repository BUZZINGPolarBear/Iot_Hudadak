#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

#define BME_CS 5 // cs for esp32 vspi
#define SEALEVELPRESSURE_HPA (1013.25)

int delayTime;
int lcdColumns = 16;
int lcdRows = 2;

Adafruit_BME280 bme(BME_CS); // hardware SPI
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));
  bool status;
  // default settings
  status = bme.begin();
  if (!status) {
  Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
  Serial.print(" ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
  Serial.print(" ID of 0x56-0x58 represents a BMP 280,\n");
  Serial.print(" ID of 0x60 represents a BME 280.\n");
  Serial.print(" ID of 0x61 represents a BME 680.\n");
  while (1) delay(10);
  }
  Serial.println("-- Default Test --");
  delayTime = 1000;
  Serial.println();
  lcd.begin();
}

void loop() 
{
  printValues();
}
void printValues()
{
  lcd_print_status(bme.readTemperature(), bme.readHumidity(), 1000);
} 

void lcd_print(String first_line_print, String second_line_print, int delay_time)
{
  lcd.setCursor(0,0);
  lcd.print(first_line_print);
  lcd.setCursor(0,1);
  lcd.print(second_line_print);
  delay(delay_time);
}

void lcd_print_status(int temp, int humid, int delay_time)
{
  lcd.setCursor(0,0);
  lcd.print("temp: " + String(temp)+"*C");
  lcd.setCursor(0,1);
  lcd.print("humid: " + String(humid)+"%");
  delay(delay_time);
}
