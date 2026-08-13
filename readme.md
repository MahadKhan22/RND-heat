# Heat System (Temperature Logger)

## Overview
This repository contains firmware for a ESP32-based environmental monitoring node. The system actively reads temperature data utilizing a DS18B20 sensor and transmits it over a physical USB serial connection for PC-side data logging. It does not include hardware control logic.

## Hardware Specifications and Pin Mapping

### Microcontroller
*   ESP32 Development Board

### Sensor
*   **Model:** DS18B20 (requires pull-up resistor)
*   **Data Pin:** GPIO 16 (RX2 on board)

## Software Dependencies
This project requires PlatformIO. The following libraries must be defined in the `platformio.ini` environment:
*   `paulstoffregen/OneWire`
*   `milesburton/DallasTemperature`

## Operation and Telemetry
*   **Polling Rate:** The DS18B20 is read every 2000 milliseconds.
*   **Protocol:** USB Serial (UART).
*   **Baud Rate:** 115200.
*   **Payload Format:** Comma-separated string prefixed with a data identifier: `DATA,<Temperature_C>`.
*   **Transmission Condition:** Packets are only dispatched when valid sensor readings are acquired. Disconnected sensor states are ignored.

## PC Data Logging
The telemetry data is designed to be captured by a PC running a serial listening script (e.g., Python utilizing the `pyserial` library and `csv` module). The script intercepts the `DATA,` identifier, extracts the numerical payload, and logs the value alongside a system timestamp into a local CSV file.