# Installation Guide

## Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) 1.8.x or 2.x
- Google Chrome 89+ or Microsoft Edge 89+
- USB cable (Mini-B or Micro-B depending on Nano version)

## Step 1: Install Arduino Libraries

Open Arduino IDE → **Sketch** → **Include Library** → **Manage Libraries**

Search and install:

1. **OneWire** by Paul Stoffregen
2. **DHT sensor library** by Adafruit
3. **Adafruit Unified Sensor** by Adafruit (install when prompted)

## Step 2: Flash the Firmware

1. Open `src/temp_controller/temp_controller.ino`
2. Select Board: **Tools** → **Board** → **Arduino Nano**
3. Select Processor: **Tools** → **Processor** → **ATmega328P (Old Bootloader)**
4. Select Port: **Tools** → **Port** → your COM port
5. Click **Upload**

### Verify in Serial Monitor

1. **Tools** → **Serial Monitor**
2. Set baud rate to **115200**
3. You should see:

```
=== Temp Controller v7 ===
DS18B20:D4 DHT22:D3(innen)
115200 Baud | 350ms Update
```

Followed by periodic data lines.

## Step 3: Open the Dashboard

1. **Close the Serial Monitor** (important — only one app can use the COM port)
2. Open `dashboard/TempCTRL_HMI.html` in **Chrome** or **Edge**
3. Click **Connect** (top right)
4. Select the Arduino COM port from the browser dialog
5. Live data should appear immediately

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No COM port in browser | Close Arduino IDE Serial Monitor first |
| DS18B20 shows FEHLER | Check pull-up wiring, run `src/tests/sensor_komplett_scanner.ino` |
| DHT22 shows NaN | Check pin 3 is not connected, verify 4.7kΩ pull-up |
| Relay doesn't switch | Check BC337 orientation, verify common ground |
| Dashboard won't connect | Use Chrome or Edge (Firefox not supported) |

## DS18B20 Sensor Replacement

This project uses a hardcoded DS18B20 address. If you replace the sensor:

1. Flash `src/tests/sensor_komplett_scanner.ino`
2. Note the 8-byte address from Serial Monitor
3. Update the `addr[8]` array in `temp_controller.ino`
