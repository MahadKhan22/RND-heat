#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to GPIO 16 (RX2)
#define ONE_WIRE_BUS 16

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

float currentTemp = DEVICE_DISCONNECTED_C;

void sendDataSerial(float temp);
void readSensor();

void setup() {
  Serial.begin(115200);
  while (!Serial){
    delay(10);
  }

  Serial.println("\nSystem Initializing...");
  Serial.println("Telemetry configured for USB Serial. Format: DATA,<Temp>");
  
  // init ds18b20
  sensors.begin();
}

void loop() {
  readSensor();
  delay(READ_INTERVAL);
}

void readSensor(){
  lastRead = millis();

  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);

  currentTemp = t;

  if (t != DEVICE_DISCONNECTED_C){
    sendDataSerial(t);
  
  }
}

void sendDataSerial(float temp){
  Serial.print("DATA,");
  Serial.println(temp);
}