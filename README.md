# Edge-to-Cloud Thermo-Humidity Monitoring & Control System

[![ARM7 LPC2148](https://img.shields.io/badge/Microcontroller-NXP_LPC2148_(ARM7TDMI--S)-00599C?style=for-the-badge&logo=arm)](https://www.nxp.com)
[![ESP8266 Wi-Fi](https://img.shields.io/badge/Wireless-ESP--01_(ESP8266)-red?style=for-the-badge&logo=wi-fi)](https://www.espressif.com)
[![ThingSpeak IoT](https://img.shields.io/badge/Cloud-ThingSpeak_IoT-3776AB?style=for-the-badge&logo=mathworks)](https://thingspeak.com)
[![Keil uVision](https://img.shields.io/badge/IDE-Keil_uVision4/5-107C41?style=for-the-badge)](https://www.keil.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

An end-to-end embedded Edge-to-Cloud IoT monitoring system built entirely in **C language** for the **NXP LPC2148 (ARM7TDMI-S)** microcontroller. The system samples ambient temperature and humidity using a **DHT11 sensor**, displays live telemetry on a **16x2 LCD** with temperature & temperature setpoint on Line 1 (`T:28°C  SP:35°C`) and relative humidity & humidity setpoint on Line 2 (`H:55%   SP:70%`), allows dynamic threshold configuration for **BOTH Temperature and Humidity Setpoints** via a **4x4 Matrix Keypad** triggered by an **External Interrupt (EINT0)**, persists thresholds in an **AT24C256 I2C EEPROM**, and periodically transmits feeds to **ThingSpeak Cloud** strictly every **3 minutes using the LPC2148 Hardware RTC** via an **ESP-01 (ESP8266) Wi-Fi module**.

---

## 🖼️ Real-Time Hardware Operation & Demonstration Flow

Below is the step-by-step visual demonstration of the system during bootup, network initialization, live telemetry monitoring, dynamic setpoint configuration, and cloud data transmission.

### Step 1: System Boot & Initialization Sequence
![Step 1: System Boot Sequence](assets/image1_system_boot.jpg)

*Power-on boot sequence on 16x2 LCD displaying system title: `Edge-Cloud Therm / Monitoring Sys`.*

---

### Step 2: ESP-01 Wi-Fi & Cloud Socket Connection
![Step 2: Connecting Cloud](assets/image2_connecting_cloud.jpg)

*Establishing TCP socket connection to ThingSpeak server via ESP-01 Wi-Fi module: `Connecting Cloud`.*

---

### Step 3: Real-Time Dual-Telemetry & Setpoint Display
![Step 3: Live LCD Telemetry](assets/image3_lcd_telemetry.jpg)

*Continuous live sensing telemetry: Line 1 (`T: 28°C SP: 49°C`), Line 2 (`H: 55% SP: 99%`).*

---

### Step 4: Dynamic Threshold Configuration via Keypad (EINT0)
![Step 4: Dynamic Setpoint Entry](assets/image4_setpoint_entry.jpg)

*Interactive 4x4 matrix keypad menu triggered by EINT0 push button interrupt: `Set Hum SP (%): 99`.*

---

### Step 5: ThingSpeak Cloud Data Transmission
![Step 5: ThingSpeak Upload Confirmation](assets/image5_thingspeak_ok.jpg)

*Hardware RTC 3-minute interval trigger executing HTTP GET feed upload to ThingSpeak Cloud API: `ThingSpeak OK! T:28°C H:55 RH`.*

---

## ✨ Key System Features

- **100% C Codebase Architecture**: Operates exclusively on C language source code including startup routines (`startup_LPC2148.c`), removing assembly and HTML dependencies.
- **Real-Time Environmental Sensing**: Samples temperature and relative humidity from a DHT11 sensor with strict **8-bit checksum verification** (`hum_int + hum_dec + temp_int + temp_dec == checksum`) and hardware timeout protection.
- **Hardware RTC 3-Minute Cloud Updates**: Utilizes the internal **LPC2148 Real-Time Clock (RTC)** peripheral (`CCR`, `PREINT`, `PREFRAC`) to trigger cloud feed transmissions strictly every **3 minutes (180 RTC seconds)**.
- **16x2 LCD Dual Set-Point Layout**:
  - **Line 1**: `T:28°C  SP:35°C` (Live Temperature with `°C` symbol and Temperature Setpoint).
  - **Line 2**: `H:55%   SP:70%` (Live Humidity with `%` symbol and Humidity Setpoint).
- **Dynamic Dual Set-Point Configuration**: Pressing the button connected to `P0.16 (EINT0)` triggers an interactive menu to set **BOTH Temperature Setpoint and Humidity Setpoint** via the 4x4 Matrix Keypad (digits `0`-`9`, `'*'` for backspace, `'#'` to confirm).
- **Non-Volatile EEPROM Storage**:
  - `0x0000`: Temperature Setpoint byte (Default: `35°C`)
  - `0x0001`: Humidity Setpoint byte (Default: `70%`)
- **Audible Alert System**: Sounds an active buzzer alert immediately whenever ambient temperature exceeds the configured temperature setpoint or humidity exceeds the configured humidity setpoint.
- **Robust ESP-01 Wi-Fi & Cloud Integration**: Issues `AT+CIPCLOSE` pre-socket reset, supports multi-response matching (`CONNECT`, `OK`, `ALREADY`, `CONNECTED`, `Linked`), and enforces non-blocking timeouts to prevent microcontroller lockups.

---

## ⚡ Hardware Architecture & Pin Mapping

| Peripherals / Module | LPC2148 Pin | Pin Function | Description |
| :--- | :--- | :--- | :--- |
| **DHT11 Sensor** | `P0.4` | GPIO Output/Input | Single-wire digital data signal |
| **16x2 LCD Data** | `P0.8 - P0.15` | GPIO Output | 8-bit Data Bus (D0 - D7) |
| **16x2 LCD RS** | `P0.17` | GPIO Output | Register Select (Command/Data) |
| **16x2 LCD EN** | `P0.18` | GPIO Output | Enable Strobe Signal |
| **16x2 LCD RW** | `P0.19` | GPIO Output | Read/Write Select (GND for Write) |
| **Buzzer** | `P0.7` | GPIO Output | Active High Buzzer Driver |
| **External Interrupt** | `P0.16` | `EINT0` (FUNC2) | Push Button Interrupt Trigger |
| **I2C EEPROM SCL** | `P0.2` | `SCL0` (FUNC1) | I2C Clock Line (AT24C256) |
| **I2C EEPROM SDA** | `P0.3` | `SDA0` (FUNC1) | I2C Data Line (AT24C256) |
| **Keypad Rows** | `P1.16 - P1.19` | GPIO Output | 4x4 Keypad Row Drive Lines |
| **Keypad Columns** | `P1.20 - P1.23` | GPIO Input | 4x4 Keypad Column Sense Lines |
| **ESP-01 TXD** | `P0.0` | `TXD0` (FUNC1) | UART0 Transmit Line -> ESP-01 RX |
| **ESP-01 RXD** | `P0.1` | `RXD0` (FUNC1) | UART0 Receive Line <- ESP-01 TX |

---

## 🌐 Cloud Integration Details (ThingSpeak)

The system automatically connects to your mobile hotspot and uploads sensor data to your ThingSpeak channel.

- **Hotspot Credentials**:
  - **SSID**: `Sandhya`
  - **Password**: `sandhya@123`
- **ThingSpeak Channel Details**:
  - **Channel ID**: `3421650`
  - **Write API Key**: `7JY9E3QDN7XTQ2RJ`
  - **Read API Key**: `OYWEAUD1UIQYP7O1`
- **Channel Field Definitions**:
  - `Field 1`: Temperature (°C)
  - `Field 2`: Humidity (% RH)
  - `Field 3`: Alert Status (`0` = Normal, `1` = Alert Triggered)

---

## 📁 Software Component Architecture

```
major_Edge_cloud/
├── main.c                      # System entry point, main loop & LCD state machine
├── startup_LPC2148.c           # Pure C system bootstrapper
├── rtc.h / rtc.c               # LPC2148 Hardware RTC driver (3-min interval timer)
├── esp01.c / esp01.h           # ESP-01 Wi-Fi driver & ThingSpeak HTTP client
├── dht11.c / dht11.h           # DHT11 sensor driver & 8-bit checksum calculation
├── external_interrupts_test.c  # EINT0 interrupt handler & dual set-point menu
├── i2c.c / i2C.h               # LPC2148 hardware I2C driver
├── i2c_eeprom.c / i2c_eeprom.h # AT24C256 EEPROM read/write functions
├── kpm.c / kpm.h               # 4x4 Matrix Keypad scanner (supports 0-9, *, #)
├── lcd.c / lcd.h               # 16x2 Character LCD driver (8-bit mode)
├── uart0.c / uart0.h           # UART0 interrupt-driven serial driver
├── delay.c / delay.h           # Millisecond and microsecond delay utilities
├── global.h                    # Global state variables (setpoint, humidity_setpoint, alert_flag)
├── Edge_cloud.uvproj           # Keil uVision Project file
└── Edge_cloud.hex              # Compiled Intel HEX binary ready for flashing
```

---

## 🛠️ Compilation & Flashing Instructions

### 1. Build the Project in Keil uVision
1. Open [`Edge_cloud.uvproj`](file:///c:/Users/HP/Downloads/india_pro/vector_project/major_Edge_cloud/Edge_cloud.uvproj) in **Keil uVision4** or **Keil uVision5**.
2. Click **Project -> Rebuild all target files** (`F7`).
3. Verify that the compilation completes with `0 Error(s), 0 Warning(s)`.
4. The output binary [`Edge_cloud.hex`](file:///c:/Users/HP/Downloads/india_pro/vector_project/major_Edge_cloud/Edge_cloud.hex) will be generated in the project folder.

### 2. Flash to LPC2148 Development Kit
1. Connect your LPC2148 board to your PC using a USB-to-UART converter (COM port).
2. Open **Flash Magic**.
3. Configure settings:
   - **Device**: LPC2148
   - **COM Port**: Select your COM Port (e.g., COM3)
   - **Baud Rate**: 9600 or 19200
   - **Interface**: None (ISP)
   - **Oscillator Frequency (MHz)**: 12.0
4. Browse and select [`Edge_cloud.hex`](file:///c:/Users/HP/Downloads/india_pro/vector_project/major_Edge_cloud/Edge_cloud.hex).
5. Check **Erase all Flash+Code Rd Protect**.
6. Press the **ISP/Boot button** on your LPC2148 kit and click **Start** in Flash Magic.

---

## 💻 Profile & Author Info

- **GitHub Profile**: [@sandhya2684](https://github.com/sandhya2684)
- **Repository**: [Edge-to-Cloud-Thermo-Monitoring-System-with-Dynamic-Set-Point-Control](https://github.com/sandhya2684/Edge-to-Cloud-Thermo-Monitoring-System-with-Dynamic-Set-Point-Control)
