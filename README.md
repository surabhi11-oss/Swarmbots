# Swarmbots - Autonomous Robotic Swarm Platform

[![Platform](https://img.shields.io/badge/Platform-ESP32%20WROOM-blue.svg)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/Display-SH1106%20%2F%20SSD1306%20OLED-brightgreen.svg)](https://github.com/olikraus/u8g2)
[![Architecture](https://img.shields.io/badge/Architecture-Modular%20Cooperative%20FSM-orange.svg)]()
[![Web Twin](https://img.shields.io/badge/Web%20Twin-Live%2060FPS%20Digital%20Twin-cyan.svg)]()

Autonomous multi-robot swarm platform featuring distributed mesh coordination, differential-drive motion control, an onboard OLED graphical interface, and a zero-install real-time Web Digital Twin.

---

## Repository Structure

```
Swarmbots/
├── firmware/
│   ├── main/                          # Core autonomous robot firmware & motor drivers
│   │   ├── SwarmBot/                  # Main ESP32 sketch for full swarm operations
│   │   └── README.md
│   ├── oled/                          # Unified OLED Display & Web Subsystem
│   │   ├── SwarmBot_OLED/             # Standalone production ESP32 OLED firmware
│   │   │   ├── SwarmBot_OLED.ino      # Main sketch entry point & cooperative scheduler
│   │   │   ├── config.h               # Global compile-time defaults & pin mappings
│   │   │   └── src/
│   │   │       ├── config/            # Non-Volatile Storage (NVS) & pin validation
│   │   │       ├── controllers/       # Motor safety & autonomous pause hooks
│   │   │       ├── display/           # U8g2 abstraction & runtime I2C scanner
│   │   │       ├── input/             # Single-button multi-gesture state machine
│   │   │       ├── state/             # Centralized robot telemetry & UI state model
│   │   │       ├── ui/                # UI rendering, boot animation & XBM assets
│   │   │       └── web/               # SoftAP Web server, live digital twin & API
│   │   ├── swarmbot_simulator.html    # Standalone browser testing & interaction harness
│   │   └── README.md                  # Comprehensive OLED subsystem documentation
│   ├── display/                       # Display drivers and documentation
│   └── tests/                         # Hardware diagnostic scripts & sensor tests
└── README.md                          # Repository master documentation
```

---

## OLED & Human-Machine Interface (HMI) Subsystem

The OLED subsystem located at [`firmware/oled/SwarmBot_OLED/`](firmware/oled/SwarmBot_OLED/) provides an interactive interface for configuring, monitoring, and debugging swarm robots without requiring a physical serial connection.

### 1. Key Features
- **Official "Hexa Swarm + S" Logo Boot Animation**:
  - Procedural 7-stage boot sequence rendering 6 swarm nodes, hexagon edges, a 3-pixel thick stylized "S" mark, and a typewriter reveal of `SWARM BOT`.
- **Single-Button Multi-Gesture Finite State Machine**:
  - Designed for minimal hardware complexity using a single tactile switch on `GPIO 27`:
    - **Single Tap (< 450 ms)**: Cycle forward / Next option.
    - **Double Tap (< 350 ms)**: Back / Exit submenu.
    - **Select Hold (1.2 s – 2.8 s)**: Confirm / Enter selected utility.
    - **Power Hold (≥ 3.0 s on Home)**: Emergency Power Halt / Confirmation screen.
- **Smartwatch-Style Carousel Menu**:
  - Smooth 5-item horizontal carousel featuring monochrome XBM icons: `MODES`, `SETTINGS`, `CREDITS`, `POWER`, and `RETURN`.
- **Comprehensive Screen Hierarchy**:
  - `Boot Screen`: Procedural logo reveal and initialization.
  - `Home Screen`: Bot ID, live battery %, dynamic battery icon, textual peer connectivity (`4/4 CONNECTED`), and operational mode chip.
  - `Mode Select`: `Follow Leader` and `Manual Control`.
  - `Sub-Utilities`: Leader tracking distance, speed overrides, and manual drive telemetry.
  - `Settings`: Live I2C address, driver selection, GPIO mappings, and SoftAP IP.
  - `Credits`: Rolling department and team credits (`ECS DEPT - VESIT`).
  - `Power Confirmation & Safe Shutdown`: Hardware latch power-down sequence.
- **Safety Lock (Autonomous Motion Pause)**:
  - While navigating the menu system, autonomous motor actuation is paused automatically to prevent physical collision while handling the robot.

---

## Web Remote & Live 128×64 Digital Twin

The ESP32 broadcasts a self-hosted Wi-Fi Access Point (`SSID: SwarmBot-01`, default IP `192.168.4.1`):

- **Bi-Directional Digital Twin**: Renders a 1:1 pixel-perfect 128×64 HTML5 Canvas representation of the OLED display in real-time.
- **Tactile Button Simulation**:
  - Includes a real-time **60 FPS live hold timer** (`⏱️ 1450 ms`).
  - Smooth dynamic progress bar filling from 0 ms to 3000 ms.
  - Dynamic visual feedback: glowing emerald green ring at 1.5s (Select), pulsing crimson red ring at 3s (Power).
- **Quick Test Shortcuts**: Instant 1-click test chips for `Tap`, `Double-Tap`, `Hold 1.5s`, and `Hold 3s`.
- **Runtime I2C Bus Scanner**: Scan connected I2C peripherals directly from the web browser.
- **NVS Settings Editor**: Change Bot ID, pin assignments, and display driver without recompiling.

---

## Default Pin Mapping

| Peripheral | Signal | ESP32 GPIO | Description |
|---|---|---|---|
| **OLED Display** | SDA | `GPIO 21` | I2C Data Line (SH1106 / SSD1306) |
| **OLED Display** | SCL | `GPIO 22` | I2C Clock Line (400 kHz Fast Mode) |
| **Tactile Button** | SW | `GPIO 27` | Active-LOW pushbutton with internal pull-up |
| **Battery ADC** | V_BAT | `GPIO 34` | ADC1 analog input (resistor divider) |
| **Left Motor** | PWM / DIR | `GPIO 18 / 19` | Differential drive left channel |
| **Right Motor** | PWM / DIR | `GPIO 23 / 5` | Differential drive right channel |
| **Power Latch** | EN | `GPIO -1` (Opt) | Optional auto-power-off latch |

> **Safe Recovery Mode:** If an invalid GPIO configuration is saved to NVS, hold `GPIO 27` down for 5 seconds during power-up to restore all factory defaults.

---

## Quick Start & Flashing Guide

### Prerequisites
- [Arduino IDE 2.x](https://www.arduino.cc/en/software) or [arduino-cli](https://arduino.github.io/arduino-cli/)
- ESP32 Board Package: `esp32 by Espressif Systems` (v2.0.x or v3.0.x)
- Library: `U8g2 by olikraus` (Install via Arduino Library Manager)

### Upload via Arduino IDE
1. Open [`firmware/oled/SwarmBot_OLED/SwarmBot_OLED.ino`](firmware/oled/SwarmBot_OLED/SwarmBot_OLED.ino).
2. Under **Tools > Board**, select **ESP32 Dev Module**.
3. Under **Tools > Port**, select your active COM port.
4. Click **Upload**.
5. Connect your phone or laptop to Wi-Fi `SwarmBot-01` and navigate to `http://192.168.4.1/`.

### Upload via Command Line (`arduino-cli`)
```bash
# Compile firmware
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/oled/SwarmBot_OLED

# Upload to ESP32 (replace COM_PORT with your port, e.g. COM3 or /dev/ttyUSB0)
arduino-cli upload -p COM_PORT --fqbn esp32:esp32:esp32 firmware/oled/SwarmBot_OLED
```
