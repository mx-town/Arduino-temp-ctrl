# 🌡️ Arduino Temperature Controller + Web HMI

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Arduino%20Nano-00878F.svg)](https://www.arduino.cc/)
[![Dashboard](https://img.shields.io/badge/Dashboard-Web%20Serial-FF6B35.svg)](#dashboard)
[![Sensors](https://img.shields.io/badge/Sensors-DS18B20%20%2B%20DHT22-4CAF50.svg)](#sensors)

A **real-time temperature monitoring and control system** built on Arduino Nano with a browser-based HMI/SCADA-style dashboard. Features precision DS18B20 temperature sensing, DHT22 humidity monitoring, LED status indicators, relay-driven fan control with configurable hysteresis, and a zero-install Web Serial dashboard.

![Dashboard Preview](docs/dashboard_preview.png)

---

## 📑 Table of Contents

- [Features](#-features)
- [System Architecture](#-system-architecture)
- [Hardware](#-hardware)
  - [Bill of Materials](#bill-of-materials)
  - [Pin Mapping](#pin-mapping)
  - [Relay Wiring](#relay-wiring-ds2e-s-dc24v)
- [Software](#-software)
  - [Installation](#installation)
  - [Configuration](#configuration)
  - [Serial Protocol](#serial-protocol)
- [Dashboard](#-dashboard)
- [Wiring Guide](#-wiring-guide)
- [Project Structure](#-project-structure)
- [Test Utilities](#-test-utilities)
- [Lessons Learned](#-lessons-learned)
- [License](#-license)

---

## ✨ Features

- **Precision Temperature Control** — DS18B20 sensor with raw OneWire read (±0.5°C)
- **Humidity Monitoring** — DHT22/AM2302 for indoor relative humidity
- **3-Zone LED Indication** — Blue (cold), Green (normal), Red (hot) with configurable thresholds
- **Relay-Driven Fan Control** — BC337 transistor driving a 24V DPDT relay with configurable hysteresis
- **Non-Blocking Architecture** — 350ms update cycle, async sensor reads, no `delay()` in main loop
- **Web Serial Dashboard** — SCADA/HMI-style interface, zero install, runs in Chrome/Edge
- **Live Trend Graph** — 300 data points (~2 min history) with color-coded zones
- **Remote Setpoint Control** — Change temperature thresholds + hysteresis from the dashboard
- **Session History** — Automatic session recording with localStorage persistence
- **Quick Presets** — One-click temperature profiles (Cool/Standard/Warm/Hot)

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────────────┐
│                    ARDUINO NANO                          │
│                   (ATmega328P)                           │
│                                                          │
│   ┌─────────┐    ┌──────────┐    ┌────────────────────┐ │
│   │ DS18B20 │◄──►│          │    │   LED Status        │ │
│   │  (D4)   │    │          │───►│  Blue  (D5) = Cold  │ │
│   │ ±0.5°C  │    │  Control │    │  Green (D6) = OK    │ │
│   └─────────┘    │  Logic   │    │  Red   (D7) = Hot   │ │
│                  │          │    └────────────────────┘ │
│   ┌─────────┐    │          │    ┌────────────────────┐ │
│   │  DHT22  │◄──►│          │───►│ BC337 → Relay      │ │
│   │  (D3)   │    │          │    │  (D8)   DS2E-24V   │ │
│   │ %RH     │    └──────────┘    └────────────────────┘ │
│   └─────────┘         ▲                                  │
│                        │ Serial 115200 Baud               │
│                        ▼                                  │
│              ┌──────────────────┐                        │
│              │  USB / COM Port  │                        │
│              └────────┬─────────┘                        │
└───────────────────────┼──────────────────────────────────┘
                        │
                        │ Web Serial API
                        ▼
┌───────────────────────────────────────────────────────────┐
│                  WEB DASHBOARD                            │
│              (Chrome / Edge Browser)                      │
│                                                           │
│  ┌──────────┐  ┌──────────────┐  ┌─────────────────────┐│
│  │ Process  │  │  Trend Chart │  │  Setpoint Controls  ││
│  │ Values   │  │  300 points  │  │  SP Low / SP High   ││
│  │ PV + %RH │  │  Color zones │  │  Hysteresis Slider  ││
│  │ Relay    │  │  SP ref lines│  │  Quick Presets      ││
│  └──────────┘  └──────────────┘  │  Session History    ││
│                                   └─────────────────────┘│
└───────────────────────────────────────────────────────────┘
```

---

## 🔧 Hardware

### Bill of Materials

| # | Component | Specification | Qty | Notes |
|---|-----------|--------------|-----|-------|
| 1 | Arduino Nano | ATmega328P, CH340 USB | 1 | Clone works fine |
| 2 | DS18B20 | Waterproof, 1m cable | 1 | Raw OneWire (no DallasTemp library) |
| 3 | DHT22 / AM2302 | 4-pin, ±0.5°C, ±2%RH | 1 | Indoor humidity sensor |
| 4 | BC337 NPN | TO-92, I_C=800mA | 1 | Pinout: E-B-C or C-B-E (check datasheet!) |
| 5 | DS2E-S-DC24V | Panasonic DPDT relay, 24V coil | 1 | NC contact used (inverted logic) |
| 6 | 1N4004 | Flyback diode | 1 | Cathode (stripe) → +24V |
| 7 | LED Blue | 3mm or 5mm | 1 | Cold indicator |
| 8 | LED Green | 3mm or 5mm | 1 | Normal indicator |
| 9 | LED Red | 3mm or 5mm | 1 | Hot indicator |
| 10 | Resistor 4.7kΩ | 1/4W | 2 | Pull-ups for DS18B20 + DHT22 |
| 11 | Resistor 220Ω | 1/4W | 3 | LED current limiting |
| 12 | Resistor 1kΩ | 1/4W | 1 | BC337 base resistor |
| 13 | Perfboard | Lochrasterplatine | 1 | With IC socket for Nano |
| 14 | IC Socket | 30-pin, 2x15 | 1 | For Nano (removable) |
| 15 | 24V PSU | DC, ≥500mA | 1 | For relay coil |

### Pin Mapping

| Arduino Pin | Component | Function |
|-------------|-----------|----------|
| `D3` | DHT22 DATA | Indoor humidity + temp |
| `D4` | DS18B20 DATA | Precision temperature (controls LEDs + relay) |
| `D5` | LED Blue | Cold indicator (T < SP_Low) |
| `D6` | LED Green | Normal indicator (SP_Low ≤ T ≤ SP_High) |
| `D7` | LED Red | Hot indicator (T > SP_High) |
| `D8` | BC337 Base | Relay control via 1kΩ resistor |
| `5V` | Sensors + Pull-ups | Common VCC rail |
| `GND` | All components | Common ground (incl. 24V GND!) |

### Relay Wiring (DS2E-S-DC24V)

Pin numbering viewed from bottom of relay:

```
Row 1 (R→L): 1, 2, 4, 6, 8
Row 2 (L→R): 9, 11, 13, 15, 16

Pin 1  (Coil+)  ──→ +24V
Pin 16 (Coil-)  ──→ BC337 Collector
Pin 2  (COM1)   ──→ +24V load supply
Pin 6  (NO1)    ──→ Fan / Load

1N4004 flyback diode: Cathode (stripe) → Pin 1, Anode → Pin 16
```

> ⚠️ **Important:** Arduino GND and 24V GND must be connected (common ground for BC337).
> ⚠️ **Relay uses NC contact** — inverted logic: `HIGH` = relay off, `LOW` = relay on.

---

## 💻 Software

### Installation

**1. Required Libraries** (Arduino IDE → Library Manager):

| Library | Author | Purpose |
|---------|--------|---------|
| OneWire | Paul Stoffregen | DS18B20 communication |
| DHT sensor library | Adafruit | DHT22 sensor read |
| Adafruit Unified Sensor | Adafruit | Required by DHT library |

**2. Flash the sketch:**

```bash
# Open in Arduino IDE:
src/temp_controller/temp_controller.ino

# Board:     Arduino Nano
# Processor: ATmega328P (Old Bootloader)
# Baud Rate: 115200
```

**3. Open the dashboard:**

```bash
# Simply double-click in your file browser:
dashboard/TempCTRL_HMI.html

# Or serve locally:
python3 -m http.server 8080
# → open http://localhost:8080/dashboard/TempCTRL_HMI.html
```

> **Note:** Close the Arduino IDE Serial Monitor before connecting the dashboard — only one application can use the COM port at a time.

### Configuration

Default values (configurable via dashboard or serial commands):

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| SP Low (Cold) | 20.0°C | -20 to 80°C | Below this → Blue LED |
| SP High (Hot) | 25.0°C | -20 to 80°C | Above this → Red LED + Fan |
| Hysteresis | 1.0°C | 0.1 to 10°C | Fan turn-off = SP_High - Hyst |

### Serial Protocol

**Baud Rate:** 115200

**Arduino → PC (every 350ms):**
```
Temperatur: 23.5 C -> GRUEN | Luefter: AUS | Kalt:20.0 Heiss:25.0 Hyst:1.0 | Feuchte:45.2 DHTTemp:23.1
```

**PC → Arduino (commands):**

| Command | Example | Response | Description |
|---------|---------|----------|-------------|
| `SET:<low>:<high>` | `SET:18:28` | `OK:18.0:28.0` | Set temperature thresholds |
| `HYST:<value>` | `HYST:0.5` | `OK:HYST:0.5` | Set hysteresis |
| `GET` | `GET` | `CFG:20.0:25.0:1.0` | Query current config |
| `RESET` | `RESET` | `OK:20.0:25.0:1.0` | Reset to defaults |

---

## 📊 Dashboard

The dashboard is a **standalone HTML file** using the Web Serial API — no server, no build tools, no dependencies.

### Features

| Feature | Description |
|---------|-------------|
| **Process Values** | Live DS18B20 temperature + DHT22 humidity |
| **Trend Chart** | Canvas-based graph, 300 data points, color-coded zones |
| **Setpoint Control** | Numeric input + slider for cold/hot thresholds + hysteresis |
| **Quick Presets** | One-click: Cool 15/20, Std 20/25, Warm 25/30, Hot 30/40 |
| **Deviation Bar** | Visual PV vs SP comparison with zone indicators |
| **Relay Status** | Animated fan icon with relay state |
| **Session History** | Click "Protokoll" → overlay with all recorded sessions |
| **Persistent Storage** | Sessions saved to localStorage across browser restarts |

### Browser Support

- ✅ Google Chrome 89+
- ✅ Microsoft Edge 89+
- ❌ Firefox (no Web Serial API)
- ❌ Safari (no Web Serial API)

---

## 🔌 Wiring Guide

See [docs/WIRING.md](docs/WIRING.md) for detailed wiring instructions with diagrams.

### Quick Reference

```
DS18B20 (3 wires):
  Red    → 5V
  Yellow → D4 + 4.7kΩ pull-up to 5V
  Black  → GND

DHT22 (4 pins, grid facing you):
  Pin 1 (left)  → 5V
  Pin 2         → D3 + 4.7kΩ pull-up to 5V
  Pin 3         → not connected
  Pin 4 (right) → GND

LEDs (each with 220Ω series resistor):
  Blue  Anode → D5,  Cathode → GND
  Green Anode → D6,  Cathode → GND
  Red   Anode → D7,  Cathode → GND

BC337 NPN:
  Base      → D8 via 1kΩ resistor
  Collector → Relay Pin 16 (Coil-)
  Emitter   → GND

Relay DS2E-S-DC24V:
  Pin 1  (Coil+) → +24V
  Pin 16 (Coil-) → BC337 Collector
  Pin 2  (COM1)  → Load supply
  Pin 6  (NO1)   → Fan/Load
  1N4004: Stripe/Cathode → Pin 1, Anode → Pin 16
```

---

## 📁 Project Structure

```
Arduino-temp-ctrl/
├── README.md                          # This file
├── LICENSE                            # MIT License
├── .gitignore                         # Arduino/C++ ignores
│
├── src/
│   ├── temp_controller/
│   │   └── temp_controller.ino        # Main sketch (v7)
│   └── tests/
│       ├── sensor_komplett_scanner.ino # Scan all pins for DS18B20
│       ├── sensor_langsam_test.ino    # Raw OneWire debug read
│       ├── dht22_isoliert_test.ino    # Isolated DHT22 test
│       ├── dht11_scanner.ino          # DHT11 pin scanner
│       └── platinen_test.ino          # LED + Relay test (no sensors)
│
├── dashboard/
│   └── TempCTRL_HMI.html             # Web Serial dashboard
│
├── docs/
│   ├── WIRING.md                      # Detailed wiring guide
│   ├── ARCHITECTURE.md                # System architecture
│   └── INSTALL.md                     # Step-by-step installation
│
└── schematics/
    ├── pcb_layout.svg                 # PCB component placement
    ├── circuit_full.svg               # Full schematic
    └── circuit_relay_detail.svg       # Relay + transistor detail
```

---

## 🧪 Test Utilities

| Sketch | Purpose | When to use |
|--------|---------|-------------|
| `sensor_komplett_scanner.ino` | Scans all digital + analog pins for DS18B20 | New sensor, unknown pin |
| `sensor_langsam_test.ino` | Raw OneWire read with address + scratchpad dump | Debugging read errors |
| `dht22_isoliert_test.ino` | Tests DHT22 on D3 in isolation | Before putting on PCB |
| `dht11_scanner.ino` | Tests DHT11 on multiple pins | Finding DHT11 issues |
| `platinen_test.ino` | Tests LEDs + relay without any sensor | Verifying PCB wiring |

---

## 📝 Lessons Learned

> Real-world debugging notes from building this project on perfboard.

### Hardware Pitfalls

- **IC Socket Cold Joints** — Only 0.05V reached the Nano through badly soldered socket pins. Always test continuity on every socket pin after soldering.

- **DS18B20 Pull-Up Wiring** — Must be star topology (resistor, sensor DATA, and MCU pin all at ONE junction). Wiring the pull-up in series instead of parallel = no communication.

- **DS18B20 Clone Incompatibility** — This particular sensor clone does not work with the DallasTemperature library. Raw OneWire reads with hardcoded address are required. Always run the scanner sketch first.

- **BC337 Pinout Confusion** — Pinout varies by manufacturer (E-B-C or C-B-E). When the relay doesn't switch, try flipping the transistor.

- **DHT11 Destruction** — A PCB wiring issue caused the DHT11 to overheat and burn. **Always test with isolated wiring first** (jumper wires directly to Nano pins, not through PCB). If a sensor gets warm → immediately disconnect power.

- **Relay NC Logic** — The DS2E-S-DC24V uses the normally-closed contact, creating inverted logic: `digitalWrite(HIGH)` = relay off, `digitalWrite(LOW)` = relay on.

### Software Insights

- **Non-Blocking DS18B20** — The 750ms conversion time blocks the entire loop if you use `delay()`. Solution: start conversion, check elapsed time, read when ready.

- **DHT22 Read Interval** — Minimum 2 seconds between reads. Reading faster returns stale or NaN values.

- **Web Serial Buffer** — Serial data arrives in chunks, not complete lines. Always buffer and split on `\n`.

---

## 📄 License

[MIT License](LICENSE) — Free to use, modify, and distribute.

---

Built with ☕ and an Arduino Nano on a perfboard in Austria 🇦🇹
