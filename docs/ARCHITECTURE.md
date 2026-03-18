# System Architecture

## Overview

The system consists of two main components: the **Arduino Firmware** (real-time sensor reading and control) and the **Web Dashboard** (visualization and remote configuration).

## Firmware Architecture

### Non-Blocking Main Loop

The firmware uses a non-blocking architecture with three independent timing loops:

```
loop()
  │
  ├── verarbeiteSerial()          ← every cycle (~1µs)
  │     Parse incoming commands
  │
  ├── DS18B20 Conversion Check    ← every 750ms
  │     if (converting && elapsed >= 750ms):
  │       read temperature
  │       start next conversion
  │
  ├── DHT22 Read                  ← every 2500ms
  │     read humidity + temperature
  │
  └── Serial Output               ← every 350ms
        format + send data line
        update LEDs
        check relay hysteresis
```

### Sensor Read Strategy

**DS18B20 (OneWire, raw read):**
- Start conversion command (`0x44`) → runs in background for 750ms
- Read scratchpad (`0xBE`) → get 9 bytes, extract 16-bit raw temperature
- Convert: `temp = raw / 16.0`
- Hardcoded address: `28 7C 0E BF 00 00 00 52`

**DHT22 (Adafruit library):**
- Minimum 2s between reads
- Returns `NaN` on communication failure
- Only valid values are stored (last-known-good pattern)

### Relay Control with Hysteresis

```
            Hysteresis Zone
         ◄──────────────────►
         │                  │
─────────┼──────────────────┼──────────► Temperature
    Fan OFF point      Fan ON point
  (SP_High - Hyst)      (SP_High)

State Machine:
  Fan OFF + Temp > SP_High     → Fan ON
  Fan ON  + Temp < (SP_High-H) → Fan OFF
  Otherwise                    → no change
```

## Dashboard Architecture

### Web Serial Communication

```
Browser                              Arduino
  │                                     │
  │  navigator.serial.requestPort()     │
  │ ──────────────────────────────────► │
  │        port.open({baudRate})        │
  │ ──────────────────────────────────► │
  │                                     │
  │    TextDecoderStream pipeline       │
  │ ◄────────────── continuous ───────► │
  │                                     │
  │    "Temperatur: 23.5 C -> ..."     │
  │ ◄────────────────────────────────── │
  │                                     │
  │    parse() → update UI + chart      │
  │                                     │
  │    "SET:20:25\n"                    │
  │ ──────────────────────────────────► │
  │                                     │
  │    "OK:20.0:25.0"                  │
  │ ◄────────────────────────────────── │
```

### Data Flow

```
Serial Input → parse() → state variables (ct, hum, fo)
                              │
                    ┌─────────┼─────────┐
                    ▼         ▼         ▼
                 uUI()     draw()   aLog()
                 Update    Canvas    Protocol
                 DOM       Chart     History
```

### Session Persistence

Sessions are stored in `localStorage` under key `tempctrl_sessions`:

```json
{
  "start": "2024-01-15T14:30:00Z",
  "end": "2024-01-15T15:45:00Z",
  "entries": 142,
  "minTemp": "21.5",
  "maxTemp": "26.3",
  "log": [
    {"t": "14:30:05", "m": "Temperatur: 22.1 C -> GRUEN ...", "temp": 22.1}
  ]
}
```

- Maximum 20 sessions stored
- Maximum 200 log entries per session
- Auto-saved on disconnect and page close (`beforeunload`)

## Technology Stack

| Layer | Technology |
|-------|-----------|
| MCU | Arduino Nano (ATmega328P, 16MHz) |
| Firmware | C/C++ (Arduino framework) |
| Communication | USB Serial (115200 baud) |
| Dashboard | Vanilla HTML/CSS/JS (single file) |
| Fonts | IBM Plex Mono + IBM Plex Sans |
| Charts | HTML5 Canvas (custom renderer) |
| Serial API | Web Serial API (Chrome 89+) |
| Storage | localStorage |
