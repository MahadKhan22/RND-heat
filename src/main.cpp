#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TFT_eSPI.h>

// Data wire is connected to GPIO 16 (RX2)
#define ONE_WIRE_BUS 16

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
TFT_eSPI tft = TFT_eSPI();

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

float currentTemp = DEVICE_DISCONNECTED_C;

void sendDataSerial(float temp);
void readSensor();
void updateDisplay();

void setup() {
  Serial.begin(115200);
  while (!Serial){
    delay(10);
  }

  Serial.println("\nSystem Initializing...");
  Serial.println("Telemetry configured for USB Serial. Format: DATA,<Temp>");
  
  // init ds18b20
  sensors.begin();

  // init display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);

  updateDisplay();
}

void loop() {
  // Enforce non-blocking interval
  if (millis() - lastRead > READ_INTERVAL) {
    readSensor();
  }
}

void readSensor(){
  lastRead = millis();

  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);

  currentTemp = t;

  if (t != DEVICE_DISCONNECTED_C){
    sendDataSerial(t);
  }

  updateDisplay();
}

void sendDataSerial(float temp){
  Serial.print("DATA,");
  Serial.println(temp);
}

void updateDisplay(){
  tft.setCursor(0, 0);
  
  tft.println("--- Heat System ---");
  tft.println();
  
  tft.print("Temp: ");
  if (currentTemp == DEVICE_DISCONNECTED_C) {
    tft.println("Error       "); 
    tft.println();
    tft.println("                  "); 
  } else {
    tft.print(currentTemp);
    tft.println(" C       "); 
    tft.println();
    tft.println("Logging over USB  "); 
  }
}