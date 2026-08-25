

# Edge-to-Cloud Thermo-Humidity Monitoring & Control System

[![ARM7 LPC2148](https://img.shields.io/badge/Microcontroller-NXP_LPC2148_(ARM7TDMI--S)-00599C?style=for-the-badge&logo=arm)](https://www.nxp.com)
[![ESP8266 Wi-Fi](https://img.shields.io/badge/Wireless-ESP--01_(ESP8266)-red?style=for-the-badge&logo=wi-fi)](https://www.espressif.com)
[![ThingSpeak IoT](https://img.shields.io/badge/Cloud-ThingSpeak_IoT-3776AB?style=for-the-badge&logo=mathworks)](https://thingspeak.com)
[![Keil uVision](https://img.shields.io/badge/IDE-Keil_uVision4/5-107C41?style=for-the-badge)](https://www.keil.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

An industrial-grade embedded Edge-to-Cloud IoT monitoring and threshold control system developed on the **NXP LPC2148 (ARM7TDMI-S)** microcontroller using **Vector's Advanced ARM7 Development Board**. 

The system continuously acquires real-time ambient temperature and humidity data using a **DHT11 sensor**, displays formatted dual-telemetry on a **16x2 LCD**, enables dynamic threshold configuration for **both Temperature and Humidity setpoints** via a **4x4 Matrix Keypad** triggered by an **External Interrupt (EINT0)**, retains setpoints across power cycles in an **AT24C256 I2C EEPROM**, drives an active **Buzzer alert** when thresholds are breached, and periodically uploads live sensor telemetry to **ThingSpeak Cloud** strictly every **3 minutes using the LPC2148 Hardware RTC**.

---
## 📊 Block Diagram

<img width="1403" height="1121" alt="Edge-to-Cloud Thermo Monitoring System" src="https://github.com/user-attachments/assets/2a2e30a3-8cbb-4031-842c-f6fe7e5ee32a" />

## 📷 Hardware Showcase & Real-Time Operation
![alt text](image1_system_boot.jpg)
![alt text](image2_connecting_cloud.jpg)
![alt text](image3_lcd_telemetry.jpg)
![alt text](image4_setpoint_entry.jpg)
![alt text](image5_thingspeak_ok.jpg)
## ✨ Key System Features

- **High-Precision Environmental Sensing**:
  - Samples temperature (`0°C - 50°C`) and relative humidity (`20% - 90% RH`) via single-wire **DHT11 sensor** protocol.
  - Implements **8-bit checksum verification** (`Humidity_Int + Humidity_Dec + Temp_Int + Temp_Dec == Checksum`) with hardware timeout guards to filter invalid noise spikes.
- **Dynamic Dual Threshold Configuration**:
  - Pressing the interrupt push button on `P0.16 (EINT0)` instantly pauses the main loop and triggers an interactive keypad menu.
  - Configures **both Temperature and Humidity Setpoints** dynmically using a **4x4 Matrix Keypad** (`0-9` digit input, `*` for backspace, `#` to confirm).
- **Non-Volatile I2C EEPROM Storage**:
  - Persists configured setpoints in an **AT24C256 I2C EEPROM** (Slave Addr `0x50`):
    - Memory Address `0x0000`: Saved Temperature Setpoint (Default: `35°C`)
    - Memory Address `0x0001`: Saved Humidity Setpoint (Default: `70%`)
  - Ensures automatic recovery of setpoints after power failure.
- **Hardware RTC 3-Minute Cloud Transmission**:
  - Leverages internal **LPC2148 Hardware RTC** peripheral (`CCR`, `PREINT`, `PREFRAC`) to compute precise total elapsed seconds.
  - Posts data to **ThingSpeak Cloud** strictly every **180 RTC seconds (3 minutes)**, preventing API rate limit penalties while optimizing power/bandwidth consumption.
- **Robust ESP-01 Wi-Fi Driver**:
  - Non-blocking UART0 interface operating at `9600 bps`.
  - Performs pre-connect TCP socket cleanups (`AT+CIPCLOSE`) to avoid stale socket deadlocks.
  - Supports multi-pattern response matching (`CONNECT`, `OK`, `ALREADY`, `CONNECTED`, `Linked`, `SEND OK`).
- **Audible Hazard Alerting System**:
  - Triggers active **Buzzer alert** (`P0.7`) with dual beep pulses whenever `Temperature > Temp_SP` OR `Humidity > Hum_SP`.

---

## ⚡ Hardware Architecture & LPC2148 Pin Mapping

### Complete Interfacing Schematic Table

| Peripheral Module | Signal Name | LPC2148 Pin | Pin Function / Mode | Description |
| :--- | :--- | :--- | :--- | :--- |
| **DHT11 Sensor** | DATA | `P0.4` | GPIO Input/Output | Single-wire bidirection data line |
| **16x2 LCD Display** | D0 - D7 | `P0.8 - P0.15` | GPIO Output | 8-bit Parallel Data Bus |
| | RS | `P0.17` | GPIO Output | Register Select (0=CMD, 1=DATA) |
| | EN | `P0.18` | GPIO Output | Enable Strobe Signal |
| | RW | `P0.19` | GPIO Output | Read/Write Select (GND/Write) |
| **Buzzer** | Active High | `P0.7` | GPIO Output | Transistor-driven Piezo Buzzer |
| **Push Button** | EINT0 | `P0.16` | `EINT0` (FUNC2) | External Interrupt 0 (Edge Triggered) |
| **AT24C256 EEPROM** | SCL | `P0.2` | `SCL0` (FUNC1) | Hardware I2C Clock Line |
| | SDA | `P0.3` | `SDA0` (FUNC1) | Hardware I2C Data Line |
| **4x4 Keypad** | Rows 0-3 | `P1.16 - P1.19` | GPIO Output | Keypad Row Drive Lines |
| | Cols 0-3 | `P1.20 - P1.23` | GPIO Input | Keypad Column Sense Lines |
| **ESP-01 Wi-Fi** | TXD -> RX | `P0.0` | `TXD0` (FUNC1) | UART0 Transmit Line |
| | RXD <- TX | `P0.1` | `RXD0` (FUNC1) | UART0 Receive Line |

---

```

---
## 🌐 Cloud Integration Details (ThingSpeak)

The system automatically connects to your mobile hotspot and uploads sensor data to your ThingSpeak channel.

- **Hotspot Credentials**:
  - **SSID**: `Santhi`
  - **Password**: `santhi235`
- **ThingSpeak Channel Details**:
  - **Channel ID**: `3464502`
  - **Write API Key**: `COM3WNTIAZSA0EM2`
  - **Read API Key**: `PKMYJKK5MCI8FZ82`
- **Channel Field Definitions**:
  - `Field 1`: Temperature (°C)
  - `Field 2`: Humidity (% RH)
  - `Field 3`: Alert Status (`0` = Normal, `1` = Alert Triggered)
---
## 📁 Software Component Architecture

major_Edge_cloud/
├── main.c                      # System entry point, main loop & LCD state machine
├── startup_LPC2148.c           # Pure C system bootstrapper
├── rtc.h / rtc.c               # LPC2148 hardware RTC driver (3-min interval timer)
├── esp01.c / esp01.h           # ESP-01 Wi-Fi driver & ThingSpeak HTTP client
├── dht11.c / dht11.h           # DHT11 sensor driver & 8-bit checksum calculation
├── external_interrupts_test.c  # EINT0 interrupt handler & dual set-point menu
├── i2c.c / i2c.h               # LPC2148 hardware I2C driver
├── i2c_eeprom.c / i2c_eeprom.h # AT24C256 EEPROM read/write functions
├── kpm.c / kpm.h               # 4×4 Matrix Keypad scanner (supports 0-9, *, #)
├── lcd.c / lcd.h               # 16×2 Character LCD driver (8-bit mode)
├── uart0.c / uart0.h           # UART0 interrupt-driven serial driver
├── delay.c / delay.h            # Millisecond and microsecond delay utilities
├── global.h                    # Global state variables (setpoint, humidity_setpoint, alert_flag)
├── Edge_cloud.uvproj           # Keil uVision Project file
└── Edge_cloud.hex              # Compiled Intel HEX binary ready for flashing

## 📁 Firmware Repository Structure

```
india_pro/
├── assets/                             # Real-time hardware demonstration images
│   ├── hardware_setup.jpg              # Board overview with peripherals
│   ├── system_boot.jpg                 # Initialization LCD screen
│   ├── wifi_at_test.jpg                # ESP-01 AT command verification
│   ├── lcd_telemetry.jpg               # Live temperature/humidity LCD display
│   └── thingspeak_ok.jpg               # ThingSpeak HTTP transmission OK
└── vector_project/
    └── major_Edge_cloud/
        ├── main.c                      # Main execution loop & system state machine
        ├── dht11.c / dht11.h           # Single-wire DHT11 sensor driver & checksum logic
        ├── esp01.c / esp01.h           # ESP-01 Wi-Fi & ThingSpeak HTTP GET client
        ├── rtc.c / rtc.h               # LPC2148 Hardware RTC initialization & timers
        ├── external_interrupts_test.c  # EINT0 ISR & dynamic setpoint keypad scanner
        ├── i2c.c / i2C.h               # Hardware I2C Master driver
        ├── i2c_eeprom.c / i2c_eeprom.h # AT24C256 non-volatile read/write routines
        ├── kpm.c / kpm.h               # 4x4 Matrix Keypad scanning & number reading
        ├── lcd.c / lcd.h               # 16x2 Character LCD driver (8-bit mode)
        ├── uart0.c / uart0.h           # UART0 interrupt serial handler
        ├── delay.c / delay.h           # Microsecond and millisecond delay helpers
        ├── global.h                    # Shared global setpoint & alert variables
        ├── defines.h                   # Bit-manipulation macros (SETBIT, WRITEBIT)
        ├── Startup.s                   # LPC2148 ARM assembly startup & vector table
        ├── Edge_cloud.uvproj           # Keil uVision project file
        └── Edge_cloud.hex              # Compiled Intel HEX firmware binary
```

---

## 🛠️ Building & Flashing Guide

### 1. Build using Keil uVision
1. Launch **Keil uVision4** or **Keil uVision5**.
2. Open project file: `vector_project/major_Edge_cloud/Edge_cloud.uvproj`.
3. Select **Project -> Rebuild all target files** (or press `F7`).
4. Ensure build output reports: `0 Error(s), 0 Warning(s)`. The target binary [`Edge_cloud.hex`](file:///c:/Users/HP/Downloads/india_pro/vector_project/major_Edge_cloud/Edge_cloud.hex) will be generated.

### 2. Flashing via Flash Magic (ISP)
1. Connect Vector ARM7 Development Board to PC via USB-to-UART converter.
2. Launch **Flash Magic** and set:
   - **Device**: LPC2148
   - **COM Port**: Select your active COM Port (e.g. `COM3`)
   - **Baud Rate**: `9600`
   - **Interface**: None (ISP)
   - **Oscillator Frequency (MHz)**: `12.0`
3. Load binary file: `Edge_cloud.hex`.
4. Select **Erase all Flash + Code Rd Protect**.
5. Hold the **ISP Button** on the development board, hit **Reset**, and click **Start** in Flash Magic.

---

## 💻 Developer & Profile Info

- **GitHub Profile**: [@santhidaarlanka](https://github.com/santhidaarlanka)
- **Repository**: [santhidaarlanka - Edge-Cloud Thermo Monitoring](https://github.com/santhidaarlanka)
- **Platform**: Embedded C / NXP LPC2148 ARM7 / IoT Edge Computing

---

## 📜 License

This project is open-source and released under the **[MIT License](LICENSE)**. Feel free to use, modify, and distribute it for academic or industrial embedded projects.
