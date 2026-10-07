#pragma once
#include <Arduino.h>

// Atharva's icons (XBM format)
extern const uint8_t icon_Power_bits[] PROGMEM;
extern const uint8_t icon_Settings_bits[] PROGMEM;
extern const uint8_t icon_Mode_bits[] PROGMEM;
extern const uint8_t icon_Credits_bits[] PROGMEM;
extern const uint8_t icon_Return_bits[] PROGMEM;
extern const uint8_t icon_ArrowLeft_bits[] PROGMEM;
extern const uint8_t icon_ArrowRight_bits[] PROGMEM;
extern const uint8_t icon_Wifi_bits[] PROGMEM;
extern const uint8_t icon_Battery_bits[] PROGMEM;
extern const uint8_t icon_Bat25_bits[] PROGMEM;

// Animations (64x64 MSB bitmaps: 512 bytes per frame)
#define ROCKET_FRAME_COUNT 28
#define NETWORK_FRAME_COUNT 28
#define TICK_FRAME_COUNT 28
#define LOADER_FRAME_COUNT 28

extern const uint8_t rocketFrames[ROCKET_FRAME_COUNT][512] PROGMEM;
extern const uint8_t networkFrames[NETWORK_FRAME_COUNT][512] PROGMEM;
extern const uint8_t tickFrames[TICK_FRAME_COUNT][512] PROGMEM;
extern const uint8_t loaderFrames[LOADER_FRAME_COUNT][512] PROGMEM;
