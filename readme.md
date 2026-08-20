# Heat Environmental Testing System

## Overview
This repository contains firmware and software for an ESP32-based environmental heating chamber[cite: 16, 17]. The system actively monitors temperature utilizing a DS18B20 sensor and regulates environmental conditions by toggling a heater relay[cite: 17]. It includes a local TFT SPI display, a 4x4 matrix keypad for parameter configuration, and USB serial data telemetry[cite: 17] accompanied by a Python logging script[cite: 15].

## Hardware Specifications and Pin Mapping

### Microcontroller
*   ESP32 Development Board[cite: 16]

### Sensor
*   **Model:** DS18B20[cite: 17]
*   **Data Pin:** GPIO 15[cite: 17]

### Relay
*   **Heater Control:** GPIO 17 (Active HIGH)[cite: 17]

### 4x4 Matrix Keypad
*   **Row Pins (1-4):** GPIO 27, 14, 13, 4[cite: 17]
*   **Column Pins (1-4):** GPIO 32, 33, 25, 26[cite: 17]

### TFT SPI Display
*   **Driver:** ILI9488[cite: 16]
*   **MOSI:** GPIO 19[cite: 16]
*   **SCLK:** GPIO 18[cite: 16]
*   **CS:** GPIO 23[cite: 16]
*   **DC:** GPIO 21[cite: 16]
*   **RST:** GPIO 22[cite: 16]
*   **MISO:** Not connected (-1)[cite: 16]

## Software Dependencies
This project requires PlatformIO[cite: 16]. The following libraries are defined in the `platformio.ini` environment:
*   `paulstoffregen/OneWire` (v2.3.7)[cite: 16]
*   `milesburton/DallasTemperature` (v3.11.0)[cite: 16]
*   `chris--a/Keypad` (v3.1.1)[cite: 16]
*   `bodmer/TFT_eSPI` (v2.5.43)[cite: 16]

*Note: The `TFT_eSPI` configuration is handled via build flags in `platformio.ini`[cite: 16].*

## Operation and Controls

### Startup Sequence
1.  System initializes serial communication at 115200 baud[cite: 17].
2.  Initial target setpoint defaults to 50.0 C[cite: 17].

### Keypad Interface
*   **`A`**: Initiate target temperature entry[cite: 17].
*   **`C`**: Insert decimal point[cite: 17].
*   **`*`**: Delete last character, or cancel current input mode[cite: 17].
*   **`#`**: Confirm numerical entry and save to the target setpoint[cite: 17].
*   **`0`-`9`**: Numerical input[cite: 17].

### Control Logic
*   The DS18B20 is polled every 2000 milliseconds[cite: 17].
*   If the current temperature is below the target temperature, the heater relay is energized[cite: 17].

### Fail-Safe Mechanism
*   If the DS18B20 returns an invalid disconnected state, a 10-second timer starts[cite: 17]. 
*   The heater relay is commanded LOW only if the error persists for the full 10-second timeout[cite: 17].
*   The TFT display will output "Error" in place of the numerical temperature reading[cite: 17].

## USB Serial Telemetry and Data Logging
The ESP32 continuously exports sensor state data over the physical USB connection via the UART interface[cite: 17].
*   **Baud Rate:** 115200[cite: 15, 17].
*   **Payload Format:** Comma-separated string prefixed with a data identifier: `DATA,<Temp>`[cite: 17].
*   **Transmission Condition:** Packets are only dispatched when valid sensor readings are acquired[cite: 17].

### PC Data Logging Script
A provided Python script intercepts the serial payload and logs it to a local CSV file[cite: 15].
*   **Target File:** `environmental_telemetry_heat.csv`[cite: 15].
*   **Timestamp:** The script generates and prepends a local system timestamp (`%Y-%m-%d %H:%M:%S`) to each data row[cite: 15].
*   **Buffer Management:** The script forces a buffer flush after every row to ensure immediate data saving[cite: 15].