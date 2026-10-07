#include "config_manager.h"
#include <Preferences.h>

AppConfig ConfigManager::_activeConfig;
static Preferences s_prefs;

void ConfigManager::init() {
    load();
}

const AppConfig& ConfigManager::get() {
    return _activeConfig;
}

void ConfigManager::load() {
    if (!s_prefs.begin(PREFS_NAMESPACE, true)) {
        Serial.println("[CFG] NVS read failed; using factory defaults.");
        _activeConfig = AppConfig();
        return;
    }

    if (!s_prefs.isKey("cfg_init")) {
        s_prefs.end();
        Serial.println("[CFG] No saved profile found; storing factory defaults.");
        resetDefaults();
        return;
    }

    _activeConfig.oledDriver = (OledDriverType)s_prefs.getUChar("driver", (uint8_t)OLED_DRIVER_SH1106);
    _activeConfig.oledI2cAddr = s_prefs.getUChar("i2c_addr", DEFAULT_OLED_I2C_ADDR);
    _activeConfig.pinOledSda = s_prefs.getChar("sda", DEFAULT_OLED_SDA);
    _activeConfig.pinOledScl = s_prefs.getChar("scl", DEFAULT_OLED_SCL);

    _activeConfig.inputMode = (InputModeType)s_prefs.getUChar("in_mode", (uint8_t)INPUT_MODE_SINGLE);
    _activeConfig.pinBtnSingle = s_prefs.getChar("btn_s", DEFAULT_BTN_SINGLE);
    _activeConfig.pinBtnPrev = s_prefs.getChar("btn_p", DEFAULT_BTN_PREV);
    _activeConfig.pinBtnNext = s_prefs.getChar("btn_n", DEFAULT_BTN_NEXT);
    _activeConfig.pinBtnSelect = s_prefs.getChar("btn_sel", DEFAULT_BTN_SELECT);
    _activeConfig.pinBtnBack = s_prefs.getChar("btn_b", DEFAULT_BTN_BACK);

    _activeConfig.pinBatteryAdc = s_prefs.getChar("bat_adc", DEFAULT_BATTERY_ADC);
    _activeConfig.pinPowerLatch = s_prefs.getChar("pwr_lat", DEFAULT_POWER_LATCH_PIN);
    _activeConfig.botId = s_prefs.getUChar("bot_id", 1);

    String ssid = s_prefs.getString("ssid", DEFAULT_AP_SSID);
    String pass = s_prefs.getString("pass", DEFAULT_AP_PASS);
    strncpy(_activeConfig.apSsid, ssid.c_str(), sizeof(_activeConfig.apSsid) - 1);
    strncpy(_activeConfig.apPassword, pass.c_str(), sizeof(_activeConfig.apPassword) - 1);

    s_prefs.end();

    String err;
    if (!validate(_activeConfig, err)) {
        Serial.printf("[CFG] Profile validation error: %s. Resetting defaults.\n", err.c_str());
        resetDefaults();
    } else {
        Serial.println("[CFG] Persistent configuration loaded.");
    }
}

bool ConfigManager::save(const AppConfig& newCfg) {
    String err;
    if (!validate(newCfg, err)) {
        Serial.printf("[CFG] Save rejected: %s\n", err.c_str());
        return false;
    }

    if (!s_prefs.begin(PREFS_NAMESPACE, false)) {
        Serial.println("[CFG] Error opening Preferences for writing.");
        return false;
    }

    s_prefs.putUChar("cfg_init", 1);
    s_prefs.putUChar("driver", (uint8_t)newCfg.oledDriver);
    s_prefs.putUChar("i2c_addr", newCfg.oledI2cAddr);
    s_prefs.putChar("sda", newCfg.pinOledSda);
    s_prefs.putChar("scl", newCfg.pinOledScl);

    s_prefs.putUChar("in_mode", (uint8_t)newCfg.inputMode);
    s_prefs.putChar("btn_s", newCfg.pinBtnSingle);
    s_prefs.putChar("btn_p", newCfg.pinBtnPrev);
    s_prefs.putChar("btn_n", newCfg.pinBtnNext);
    s_prefs.putChar("btn_sel", newCfg.pinBtnSelect);
    s_prefs.putChar("btn_b", newCfg.pinBtnBack);

    s_prefs.putChar("bat_adc", newCfg.pinBatteryAdc);
    s_prefs.putChar("pwr_lat", newCfg.pinPowerLatch);
    s_prefs.putUChar("bot_id", newCfg.botId);

    s_prefs.putString("ssid", newCfg.apSsid);
    s_prefs.putString("pass", newCfg.apPassword);

    s_prefs.end();

    _activeConfig = newCfg;
    Serial.println("[CFG] Configuration saved to NVS.");
    return true;
}

void ConfigManager::resetDefaults() {
    if (s_prefs.begin(PREFS_NAMESPACE, false)) {
        s_prefs.clear();
        s_prefs.end();
    }
    _activeConfig = AppConfig();
    save(_activeConfig);
    Serial.println("[CFG] Factory defaults restored.");
}

bool ConfigManager::validate(const AppConfig& cfg, String& errorMsg) {
    auto isInputOnly = [](int8_t p) {
        return (p == 34 || p == 35 || p == 36 || p == 39);
    };

    auto isValidGpio = [](int8_t p) {
        if (p < 0) return true;
        return (p <= 39);
    };

    if (cfg.pinOledSda < 0 || cfg.pinOledScl < 0) {
        errorMsg = "OLED SDA and SCL pins must be assigned (>= 0).";
        return false;
    }
    if (cfg.pinOledSda == cfg.pinOledScl) {
        errorMsg = "OLED SDA and SCL cannot be the same GPIO.";
        return false;
    }
    if (isInputOnly(cfg.pinOledSda)) {
        errorMsg = "GPIO " + String(cfg.pinOledSda) + " is input-only and cannot be used for OLED SDA.";
        return false;
    }
    if (isInputOnly(cfg.pinOledScl)) {
        errorMsg = "GPIO " + String(cfg.pinOledScl) + " is input-only and cannot be used for OLED SCL.";
        return false;
    }
    if (cfg.pinPowerLatch >= 0 && isInputOnly(cfg.pinPowerLatch)) {
        errorMsg = "GPIO " + String(cfg.pinPowerLatch) + " is input-only and cannot be used for Power Latch.";
        return false;
    }

    int8_t pins[8];
    int count = 0;
    pins[count++] = cfg.pinOledSda;
    pins[count++] = cfg.pinOledScl;
    if (cfg.inputMode == INPUT_MODE_SINGLE) {
        pins[count++] = cfg.pinBtnSingle;
    } else {
        if (cfg.pinBtnPrev >= 0) pins[count++] = cfg.pinBtnPrev;
        if (cfg.pinBtnNext >= 0) pins[count++] = cfg.pinBtnNext;
        if (cfg.pinBtnSelect >= 0) pins[count++] = cfg.pinBtnSelect;
        if (cfg.pinBtnBack >= 0) pins[count++] = cfg.pinBtnBack;
    }
    if (cfg.pinBatteryAdc >= 0) pins[count++] = cfg.pinBatteryAdc;
    if (cfg.pinPowerLatch >= 0) pins[count++] = cfg.pinPowerLatch;

    for (int i = 0; i < count; i++) {
        if (pins[i] >= 0) {
            if (!isValidGpio(pins[i])) {
                errorMsg = "GPIO " + String(pins[i]) + " is out of range for ESP32 (0-39).";
                return false;
            }
            for (int j = i + 1; j < count; j++) {
                if (pins[i] == pins[j]) {
                    errorMsg = "GPIO " + String(pins[i]) + " is assigned to multiple functions.";
                    return false;
                }
            }
        }
    }

    if (cfg.oledI2cAddr != 0x3C && cfg.oledI2cAddr != 0x3D) {
        errorMsg = "OLED I2C address must be 0x3C or 0x3D.";
        return false;
    }

    return true;
}

bool ConfigManager::checkBootRecovery() {
    pinMode(DEFAULT_BTN_SINGLE, INPUT_PULLUP);
    if (digitalRead(DEFAULT_BTN_SINGLE) == LOW) {
        Serial.println("[CFG] Recovery button held at boot. Hold for 5s to reset defaults...");
        uint32_t start = millis();
        while (digitalRead(DEFAULT_BTN_SINGLE) == LOW) {
            if (millis() - start > 5000) {
                resetDefaults();
                Serial.println("[CFG] *** RECOVERY COMPLETE: FACTORY DEFAULTS RESTORED ***");
                return true;
            }
            delay(50);
        }
        Serial.println("[CFG] Recovery cancelled (button released early).");
    }
    return false;
}
