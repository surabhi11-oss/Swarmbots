#pragma once
#include <Arduino.h>

#define DEFAULT_OLED_SDA          21
#define DEFAULT_OLED_SCL          22
#define DEFAULT_OLED_I2C_ADDR     0x3C
#define DEFAULT_BTN_SINGLE        27
#define DEFAULT_BATTERY_ADC       34
#define DEFAULT_POWER_LATCH_PIN   -1

#define DEFAULT_BTN_PREV          -1
#define DEFAULT_BTN_NEXT          -1
#define DEFAULT_BTN_SELECT        -1
#define DEFAULT_BTN_BACK          -1

#define OLED_SCREEN_WIDTH         128
#define OLED_SCREEN_HEIGHT        64

enum OledDriverType {
    OLED_DRIVER_SH1106 = 0,
    OLED_DRIVER_SSD1306 = 1
};

enum InputModeType {
    INPUT_MODE_SINGLE = 0,
    INPUT_MODE_MULTI = 1
};

enum NavEvent {
    NAV_NONE = 0,
    NAV_NEXT,
    NAV_PREVIOUS,
    NAV_SELECT,
    NAV_BACK,
    NAV_HOME,
    NAV_POWER
};

#define BTN_DEBOUNCE_MS           40
#define BTN_TAP_MIN_MS            50
#define BTN_TAP_MAX_MS            700
#define BTN_DOUBLE_TAP_GAP_MS     350
#define BTN_HOLD_MENU_MIN_MS      1200
#define BTN_HOLD_MENU_MAX_MS      2300
#define BTN_HOLD_POWER_MIN_MS     2800

#define DEFAULT_AP_SSID           "SwarmBot-01"
#define DEFAULT_AP_PASS           "swarmbot123"
#define DEFAULT_AP_IP             "192.168.4.1"
#define WEB_SERVER_PORT           80

#define PREFS_NAMESPACE           "swarm_cfg"
#define UI_INTERACTION_TIMEOUT_MS 15000
