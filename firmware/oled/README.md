# Swarm Bot - Unified OLED & HMI Subsystem

## Overview

The `firmware/oled/` module provides a comprehensive, production-grade OLED display and Human-Machine Interface (HMI) for the Swarm Bot platform. It consolidates both hardware display handling (via U8g2) and a zero-install real-time Web Digital Twin accessible over ESP32 SoftAP Wi-Fi.

This subsystem provides a multi-gesture single-button engine, procedural "Hexa Swarm + S" boot animation, smartwatch carousel menu navigation, and a 60 FPS live web digital twin.

---

## Architecture & Directory Layout

```
firmware/oled/
├── SwarmBot_OLED/                    # Standalone ESP32 Arduino Sketch
│   ├── SwarmBot_OLED.ino             # Main loop, setup, and non-blocking task scheduler
│   ├── config.h                      # Pin definitions, default timings & constants
│   └── src/
│       ├── config/                   # Persistent storage & settings validation
│       │   ├── config_manager.h      # NVS Preferences API
│       │   └── config_manager.cpp
│       ├── controllers/              # Physical robot hooks & safety locks
│       │   ├── robot_controllers.h   # Motion pause during UI navigation
│       │   └── robot_controllers.cpp
│       ├── display/                  # U8g2 abstraction & I2C scanner
│       │   ├── display_manager.h     # Dynamic SH1106 / SSD1306 driver init
│       │   └── display_manager.cpp
│       ├── input/                    # Multi-gesture single button engine
│       │   ├── input_manager.h       # Debounce, double-tap, and hold detection
│       │   └── input_manager.cpp
│       ├── state/                    # Central state definition
│       │   ├── bot_state.h           # BotState struct, page enum, telemetry
│       │   └── bot_state.cpp
│       ├── ui/                       # Visual rendering & assets
│       │   ├── assets.h              # Monochrome XBM icons (Progmem)
│       │   ├── assets.cpp
│       │   ├── ui.h                  # Screen renderers & boot sequence
│       │   └── ui.cpp
│       └── web/                      # SoftAP Dashboard & Digital Twin
│           ├── web_page.h            # HTML/CSS/JS with 60 FPS live hold timer
│           ├── web_server.h          # REST endpoints
│           └── web_server.cpp
├── swarmbot_simulator.html           # Standalone browser canvas test harness
└── README.md                         # This documentation file
```

---

## 1. Official "Hexa Swarm + S" Boot Animation

### Geometry
- **Hexagon Outer Boundary**: 6 vertices centered at $(64, 25.5)$ with radius $R=21.5$:
  `[ (64, 4), (84, 15), (84, 36), (64, 47), (44, 36), (44, 15) ]`
- **Central Stylized S**: 9 vertices drawn with a 3-pixel thick line (`drawLogoThickLine`):
  `[ (74, 15), (63, 11), (53, 17), (53, 23), (67, 28), (75, 32), (75, 37), (66, 42), (54, 38) ]`

### Procedural 7-Stage Timeline
| Stage | Range (ms) | Description |
|---|---|---|
| **Nodes** | $0 \to 780$ | 6 outer swarm nodes appear sequentially with growing radius ($R=1 \to 2$). |
| **Edges** | $780 \to 1500$ | Hexagon boundary edges connect adjacent nodes progressively. |
| **S-Path** | $1500 \to 2380$ | Stylized central "S" draws sequentially in 8 segments with 3px stroke. |
| **Lock** | $2380 \to 2470$ | Full logo solidifies in high contrast. |
| **Text** | $2470 \to 3280$ | Typewriter reveal of `SWARM BOT` wordmark below the logo ($Y=63$). |
| **Ping** | $3280 \to 3380$ | Radar ping pulse ripples across the 6 hex nodes. |
| **Hold** | $3380 \to 4780$ | Final frame holds cleanly before transitioning to `PAGE_HOME`. |

---

## 2. Single-Button Multi-Gesture State Machine

The input engine uses a single active-low momentary switch on **GPIO 27** (internal pull-up enabled). To ensure reliability without accidental misfires, gestures are evaluated strictly on **release**:

| Gesture | Physical Action | Evaluation Condition | Resulting Nav Event |
|---|---|---|---|
| **Single Tap** | Quick press | Held $< 450\text{ ms}$, no second tap within $350\text{ ms}$ | `NAV_NEXT` (Cycle option) |
| **Double Tap** | Two quick presses | Second press begins within $350\text{ ms}$ of first release | `NAV_BACK` (Cancel / Back) |
| **Select Hold** | Press and hold | Held for $1200\text{ ms} \le t < 2800\text{ ms}$, then released | `NAV_SELECT` (Confirm / Enter) |
| **Power Hold** | Long sustained hold | Held for $t \ge 3000\text{ ms}$ while on Home Screen | `NAV_POWER` (Direct Power confirmation) |

> **Real-time Visual Feedback on Hardware**: When holding on `PAGE_HOME`, an active progress bar fills at the bottom of the OLED ($Y=54..61$) indicating hold duration toward menu access or power shutdown.

---

## 3. UI Navigation & Page Hierarchy

```
[PAGE_BOOT]
     │ (Boot sequence finishes)
     ▼
[PAGE_HOME] ──(Hold 1.5s)──► [PAGE_MAIN_MENU] (5-Item Carousel)
     │                             ├── MODES    ──► [PAGE_MODE_SELECT]
     │ (Hold 3s)                   │                     ├── Follow Leader ──► [PAGE_FOLLOW_UTILITIES]
     ▼                             │                     └── Manual Control ──► [PAGE_MANUAL_UTILITIES]
[PAGE_POWER_CONFIRM]               ├── SETTINGS ──► [PAGE_SETTINGS] (Driver, GPIO, IP)
     │ (Hold 1.5s Confirm)         ├── CREDITS  ──► [PAGE_CREDITS] (Department & Team Credits)
     ▼                             ├── POWER    ──► [PAGE_POWER_CONFIRM] ──► [PAGE_SHUTDOWN]
[PAGE_SHUTDOWN]                    └── RETURN   ──► Back to [PAGE_HOME]
```

---

## 4. Web Remote & Real-Time Digital Twin

When powered on, the ESP32 starts an independent SoftAP Wi-Fi network:
- **SSID**: `SwarmBot-01` (open or configurable)
- **URL**: `http://192.168.4.1/`

### Features
1. **60 FPS Live Digital Twin**:
   - The webpage renders an exact 128×64 pixel canvas mirror of the physical OLED screen using live telemetry fetched every 200 ms.
2. **Tactile Button with 60 FPS Hold Timer**:
   - Realistic 3D metallic push-button with `pointerdown`/`pointerup` event handling.
   - Live millisecond counter updates every animation frame: `⏱️ 1450 ms`.
   - Continuous color gradient progress bar ($0 \to 3000\text{ ms}$).
   - Dynamic glowing border feedback:
     - Neutral dark border ($< 1200\text{ ms}$)
     - Glowing **Emerald Green** ring ($\ge 1200\text{ ms}$, Select threshold)
     - Pulsing **Crimson Red** ring ($\ge 2800\text{ ms}$, Power threshold)
3. **Quick Test Chips**:
   - One-click shortcuts for `[Tap: Next]`, `[Double: Back]`, `[Hold 1.5s: Select]`, and `[Hold 3s: Power]` to test states immediately.
4. **REST API Endpoints**:
   - `GET /api/state`: Returns JSON telemetry (`botId`, `battery`, `voltage`, `net`, `peers`, `mode`, `page`, `holdMs`, `uiLocked`).
   - `POST /api/nav?action=X&gesture=Y`: Dispatches navigation events directly into the input queue.
   - `GET /api/i2c-scan`: Triggers an active I2C bus scan and returns detected hex addresses.
   - `POST /api/config`: Validates and saves GPIO and display settings to NVS storage.
   - `POST /api/reboot`: Restarts the ESP32 controller.
   - `POST /api/reset`: Restores factory default settings.

---

## 5. Performance Optimizations

1. **Non-Blocking Cooperative Scheduling**:
   - Zero `delay()` calls in the runtime loop. The display refreshes on an adaptive timer (~25-30 FPS), inputs sample at 20 ms, and the web server services clients asynchronously.
2. **Memory Footprint**:
   - U8g2 Full Buffer mode (`U8G2_SH1106_128X64_NONAME_F_HW_I2C`) utilizes only 1024 bytes of RAM, well within ESP32's 520 KB SRAM.
   - Web assets are compressed into a single `PROGMEM` string (`INDEX_HTML`), consuming 0 bytes of dynamic heap until served.
3. **Hardware Collision Protection**:
   - Whenever any UI interaction occurs (physical button pressed or web remote active), `g_botState.uiInteractionLock` is engaged, automatically pausing autonomous motor actuation until navigation returns to `PAGE_HOME`.
