# Heat Environmental Testing System

## Overview
This repository contains firmware and software for an ESP32-based environmental heating chamber. The system actively monitors temperature utilizing a DS18B20 sensor and regulates environmental conditions by toggling a heater relay. It includes a local TFT SPI display, a 4x4 matrix keypad for parameter configuration, and USB serial data telemetry accompanied by a Python logging script.

## Hardware Specifications and Pin Mapping

### Microcontroller
*   ESP32 Development Board

### Sensor
*   **Model:** DS18B20
*   **Data Pin:** GPIO 15
*   **Plug in:** When plugging in the sensor wires, viewed from the back of the board, keep the red wire (+) on the right, so the black wire (-) is on the right and the data line is between them.


### Relay
*   **Heater Control:** GPIO 17 (Active HIGH)

### 4x4 Matrix Keypad
*   **Row Pins (1-4):** GPIO 27, 14, 13, 4
*   **Column Pins (1-4):** GPIO 32, 33, 25, 26

### TFT SPI Display
*   **Driver:** ILI9488
*   **MOSI:** GPIO 19
*   **SCLK:** GPIO 18
*   **CS:** GPIO 23
*   **DC:** GPIO 21
*   **RST:** GPIO 22
*   **MISO:** Not connected (-1)

## Software Dependencies
This project requires PlatformIO. The following libraries are defined in the `platformio.ini` environment:
*   `paulstoffregen/OneWire` (v2.3.7)
*   `milesburton/DallasTemperature` (v3.11.0)
*   `chris--a/Keypad` (v3.1.1)
*   `bodmer/TFT_eSPI` (v2.5.43)

*Note: The `TFT_eSPI` configuration is handled via build flags in `platformio.ini`.*

## Operation and Controls

### Startup Sequence
1.  System initializes serial communication at 115200 baud.
2.  Initial target setpoint defaults to 50.0 C.

### Keypad Interface
*   **`A`**: Initiate target temperature entry.
*   **`C`**: Insert decimal point.
*   **`*`**: Delete last character, or cancel current input mode.
*   **`#`**: Confirm numerical entry and save to the target setpoint.
*   **`0`-`9`**: Numerical input.

### Control Logic
*   The DS18B20 is polled every 2000 milliseconds.
*   If the current temperature is below the target temperature, the heater relay is energized.

### Fail-Safe Mechanism
*   If the DS18B20 returns an invalid disconnected state, a 10-second timer starts. 
*   The heater relay is commanded LOW only if the error persists for the full 10-second timeout.
*   The TFT display will output "Error" in place of the numerical temperature reading.

## USB Serial Telemetry and Data Logging
The ESP32 continuously exports sensor state data over the physical USB connection via the UART interface.
*   **Baud Rate:** 115200.
*   **Payload Format:** Comma-separated string prefixed with a data identifier: `DATA,<Temp>`.
*   **Transmission Condition:** Packets are only dispatched when valid sensor readings are acquired.

### PC Data Logging Script
A provided Python script intercepts the serial payload and logs it to a local CSV file.
*   **Target File:** `environmental_telemetry_heat.csv`.
*   **Timestamp:** The script generates and prepends a local system timestamp (`%Y-%m-%d %H:%M:%S`) to each data row.
*   **Buffer Management:** The script forces a buffer flush after every row to ensure immediate data saving.