#pragma once
#include <Arduino.h>
#include "../config.h"

struct AppConfig {
    OledDriverType oledDriver = OLED_DRIVER_SH1106;
    uint8_t oledI2cAddr = DEFAULT_OLED_I2C_ADDR;
    int8_t pinOledSda = DEFAULT_OLED_SDA;
    int8_t pinOledScl = DEFAULT_OLED_SCL;

    InputModeType inputMode = INPUT_MODE_SINGLE;
    int8_t pinBtnSingle = DEFAULT_BTN_SINGLE;
    int8_t pinBtnPrev = DEFAULT_BTN_PREV;
    int8_t pinBtnNext = DEFAULT_BTN_NEXT;
    int8_t pinBtnSelect = DEFAULT_BTN_SELECT;
    int8_t pinBtnBack = DEFAULT_BTN_BACK;

    int8_t pinBatteryAdc = DEFAULT_BATTERY_ADC;
    int8_t pinPowerLatch = DEFAULT_POWER_LATCH_PIN;
    uint8_t botId = 1;

    char apSsid[32] = DEFAULT_AP_SSID;
    char apPassword[32] = DEFAULT_AP_PASS;
};

class ConfigManager {
public:
    static void init();
    static const AppConfig& get();
    static bool save(const AppConfig& newCfg);
    static void resetDefaults();
    static bool validate(const AppConfig& cfg, String& errorMsg);
    static bool checkBootRecovery();

private:
    static AppConfig _activeConfig;
    static void load();
};
