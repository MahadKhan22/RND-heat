#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Keypad.h>
#include <TFT_eSPI.h>

// --- Hardware Pin Definitions ---
#define ONE_WIRE_BUS 15 // TX2
#define HEATER_PIN 17 // RX2


// --- Component Initialization ---
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
TFT_eSPI tft = TFT_eSPI();

// Keypad matrix configuration (4x4)
byte ROW_PINS[4] = {27, 14, 13, 4};
byte COL_PINS[4] = {32, 33, 25, 26};

char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), ROW_PINS, COL_PINS, 4, 4);

// --- State Variables ---
float currentTemp = DEVICE_DISCONNECTED_C; 
float targetTemp = 50.0; 

String entryBuffer = "";
enum InputMode { NONE, SET_TEMP };
InputMode currentMode = NONE;

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

// --- Error Tolerance Variables ---
unsigned long errorStartTime = 0;
const unsigned long ERROR_TIMEOUT = 1000; // wait 1 second to turn off if sensor error
bool errorActive = false;

// --- Function Prototypes ---
void handleKeypad();
void readSensorAndUpdate();
void controlRelays();
void updateDisplay();
void sendDataSerial(float temp);

void setup() {
  Serial.begin(115200);
  while (!Serial) { 
    delay(10); 
  }

  Serial.println("\nSystem Initializing...");
  Serial.println("Telemetry configured for USB Serial. Format: DATA,<Temp>");

  // Initialize DS18B20 sensor
  sensors.begin(); 

  // Configure relay control pins
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  // Initialize and configure TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  
  updateDisplay();
}

void loop() {
  handleKeypad();
  readSensorAndUpdate();
  controlRelays();
}

void readSensorAndUpdate() {
  if (millis() - lastRead > READ_INTERVAL) {
    lastRead = millis();
    
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    
    currentTemp = t;
    
    if (t != DEVICE_DISCONNECTED_C) { 
      sendDataSerial(t);
    }
    
    updateDisplay();
  }
}

void sendDataSerial(float temp) {
  Serial.print("DATA,");
  Serial.println(temp);
}

void controlRelays() {
  // Prevent relay activation if sensor data is invalid for 10 continuous seconds
  if (currentTemp == DEVICE_DISCONNECTED_C) { 
    if (!errorActive) {
      errorActive = true;
      errorStartTime = millis();
    }
    
    // Halt heater only if the error duration exceeds the timeout
    if (millis() - errorStartTime >= ERROR_TIMEOUT) {
      digitalWrite(HEATER_PIN, LOW); 
    }
    return; // Bypass normal temperature evaluation
  }

  // Reset error state upon receiving a valid sensor reading
  errorActive = false;

  // Activate heater if current temperature is below target (heating logic)
  if (currentTemp < targetTemp) { 
    digitalWrite(HEATER_PIN, HIGH); 
  } else {
    // Deactivate heater if target temperature is reached
    digitalWrite(HEATER_PIN, LOW); 
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key == NO_KEY) return;

  switch (key) {
    case 'A': 
      currentMode = SET_TEMP;
      entryBuffer = "";
      updateDisplay();
      break;

    case '*': 
      if (entryBuffer.length() > 0) {
        entryBuffer.remove(entryBuffer.length() - 1);
      } else {
        currentMode = NONE;
      }
      updateDisplay();
      break;

    case '#': 
      if (entryBuffer.length() > 0) { 
        float newSetpoint = entryBuffer.toInt();
        if (currentMode == SET_TEMP) {
          targetTemp = newSetpoint;
        }
      }
      entryBuffer = "";
      currentMode = NONE;
      updateDisplay();
      break;

    default:
      if (key >= '0' && key <= '9' && entryBuffer.length() < 4) { 
        entryBuffer += key;
        updateDisplay();
      }
      break;
  }
}

void updateDisplay() {
  tft.setCursor(0, 0);
  
  tft.println("-- Status -- | -- Target --");
  
  tft.print("Temp: ");
  if (currentTemp == DEVICE_DISCONNECTED_C) {
    tft.print("Error  |    "); 
  } else {
    tft.print(currentTemp, 1);
    tft.print(" C |    "); 
  }

  tft.print(targetTemp, 1); 
  tft.println(" C      "); 
  
  tft.print("Relay: ");
  if (digitalRead(HEATER_PIN)) {
    tft.println("ON                ");
  } else {
    tft.println("OFF               ");
  }
  
  tft.println("\n--- Input Mode ---");
  tft.println("A:Set Temp  *:Delete"); 
  tft.println("#:Confirm           "); 
  
  if (currentMode == SET_TEMP) {
    tft.println("\nEntering Target Temp:"); 
    tft.print(entryBuffer);
    tft.println("          "); 
  } else {
    tft.println("\n                     "); 
    tft.println("          "); 
  }
}