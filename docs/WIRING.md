# Wiring Guide

Complete wiring documentation for the TempCTRL system.

## Overview

```
                        +5V Rail
                          │
              ┌───────────┼───────────┐
              │           │           │
            R1=4.7k     R2=4.7k    VCC Sensors
              │           │
    D4 ───────┤           ├──────── D3
              │           │
          DS18B20       DHT22
           DATA        Pin 2
              │           │
             GND         GND
```

## DS18B20 (Waterproof, 3 Wires)

**Cable colors (standard):**

| Wire | Color | Connect to |
|------|-------|-----------|
| VCC | Red | +5V rail |
| DATA | Yellow | D4 + 4.7kΩ pull-up to +5V |
| GND | Black | GND rail |

**Pull-up wiring (star topology):**

```
        +5V
         │
       [4.7kΩ]
         │
    D4 ──┼── DS18B20 DATA (Yellow)
         │
   (all three meet at ONE point)
```

> ⚠️ Do NOT wire the pull-up in series. All three connections (resistor, sensor DATA, Arduino pin) must share a single junction point.

## DHT22 / AM2302 (4 Pins)

**Pin numbering (grid/vent side facing you):**

```
  ┌─────────────┐
  │  Grid Side  │
  │             │
  │    DHT22    │
  │             │
  └┬──┬──┬──┬──┘
   1  2  3  4
   │  │     │
  VCC DQ   GND
   │  │     │
  +5V D3   GND
```

| Pin | Function | Connect to |
|-----|----------|-----------|
| 1 (left) | VCC | +5V |
| 2 | DATA | D3 + 4.7kΩ pull-up to +5V |
| 3 | NC | **Not connected** |
| 4 (right) | GND | GND |

## LEDs

Each LED with 220Ω series resistor:

```
D5 ──[220Ω]──►│── GND   (Blue  = Cold)
D6 ──[220Ω]──►│── GND   (Green = Normal)
D7 ──[220Ω]──►│── GND   (Red   = Hot)
```

## BC337 + Relay Circuit

```
                    +24V
                     │
              ┌──────┤
              │      │
           Relay   1N4004
           Coil    (stripe
           Pin 1    toward
              │     +24V)
              │      │
           Relay ────┘
           Coil
           Pin 16
              │
              C (Collector)
              │
    D8 ─[1kΩ]─B (Base)       BC337 NPN
              │
              E (Emitter)
              │
             GND ─────────── 24V GND
                              (common ground!)
```

### Relay Contact Wiring (Load Side)

```
Pin 2  (COM1)  ──→ +24V (or load supply voltage)
Pin 6  (NO1)   ──→ Fan / Load
```

### BC337 Pinout

**Check your specific transistor!** Pinout varies:

```
Variant A:        Variant B:
 E  B  C          C  B  E
 │  │  │          │  │  │
 └──┴──┘          └──┴──┘
  (flat)           (flat)
```

If the relay doesn't switch: try flipping the transistor 180°.

## Common Ground

The Arduino GND and the 24V power supply GND **must be connected together**. The BC337 emitter connects to this shared ground. Without common ground, the transistor cannot properly switch the relay.

```
Arduino GND ───┬─── 24V PSU GND
               │
          BC337 Emitter
```

## Future: Outdoor DHT22 on Long Cable (10m)

For a future outdoor sensor on a 10m Cat5 cable:

| Parameter | Value |
|-----------|-------|
| Pin | D2 |
| Pull-up | 3.3kΩ at sensor end |
| Cable | Cat5/Cat5e UTP |
| Pair 1 | DATA + GND (twisted) |
| Pair 2 | VCC + GND (twisted) |
| Optional | 100nF ceramic cap at sensor (VCC-GND) |
