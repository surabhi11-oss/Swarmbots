#include "display_manager.h"

U8G2* DisplayManager::_u8g2 = nullptr;
bool DisplayManager::_initialized = false;
OledDriverType DisplayManager::_driver = OLED_DRIVER_SH1106;
uint8_t DisplayManager::_i2cAddr = DEFAULT_OLED_I2C_ADDR;

bool DisplayManager::begin(int sda, int scl, uint8_t i2cAddr, OledDriverType driver) {
    _driver = driver;
    _i2cAddr = i2cAddr;

    Wire.begin(sda, scl);
    Wire.setClock(400000);

    Wire.beginTransmission(i2cAddr);
    byte err = Wire.endTransmission();
    if (err != 0) {
        Serial.printf("[OLED] Warning: No I2C ACK at 0x%02X (error %d). Probing anyway...\n", i2cAddr, err);
    } else {
        Serial.printf("[OLED] Device acknowledged at I2C address 0x%02X.\n", i2cAddr);
    }

    if (_u8g2 != nullptr) {
        delete _u8g2;
        _u8g2 = nullptr;
    }

    if (driver == OLED_DRIVER_SH1106) {
        Serial.println("[OLED] Initializing SH1106 128x64 driver.");
        _u8g2 = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    } else {
        Serial.println("[OLED] Initializing SSD1306 128x64 driver.");
        _u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    }

    _u8g2->setI2CAddress(i2cAddr * 2);
    _u8g2->begin();
    _u8g2->setBitmapMode(1);
    _u8g2->setFontMode(1);

    _initialized = true;
    return true;
}

void DisplayManager::clear() {
    if (_u8g2) _u8g2->clearBuffer();
}

void DisplayManager::display() {
    if (_u8g2) _u8g2->sendBuffer();
}

void DisplayManager::drawStr(int x, int y, const char* str) {
    if (_u8g2) _u8g2->drawStr(x, y, str);
}

void DisplayManager::drawStrCenter(int y, const char* str) {
    if (!_u8g2) return;
    int w = _u8g2->getStrWidth(str);
    int x = (OLED_SCREEN_WIDTH - w) / 2;
    _u8g2->drawStr(x, y, str);
}

void DisplayManager::drawStrRight(int x, int y, const char* str) {
    if (!_u8g2) return;
    int w = _u8g2->getStrWidth(str);
    _u8g2->drawStr(x - w, y, str);
}

void DisplayManager::drawBitmap(int x, int y, int byteWidth, int height, const uint8_t* bitmap) {
    if (_u8g2) _u8g2->drawBitmap(x, y, byteWidth, height, bitmap);
}

void DisplayManager::drawXBM(int x, int y, int width, int height, const uint8_t* bits) {
    if (_u8g2) _u8g2->drawXBM(x, y, width, height, bits);
}

void DisplayManager::drawBox(int x, int y, int w, int h) {
    if (_u8g2) _u8g2->drawBox(x, y, w, h);
}

void DisplayManager::drawFrame(int x, int y, int w, int h) {
    if (_u8g2) _u8g2->drawFrame(x, y, w, h);
}

void DisplayManager::drawLine(int x1, int y1, int x2, int y2) {
    if (_u8g2) _u8g2->drawLine(x1, y1, x2, y2);
}

void DisplayManager::drawCircle(int x0, int y0, int rad) {
    if (_u8g2) _u8g2->drawCircle(x0, y0, rad);
}

void DisplayManager::fillCircle(int x0, int y0, int rad) {
    if (_u8g2) _u8g2->drawDisc(x0, y0, rad);
}

void DisplayManager::setFont(const uint8_t* font) {
    if (_u8g2) _u8g2->setFont(font);
}

int DisplayManager::getStrWidth(const char* str) {
    if (!_u8g2) return 0;
    return _u8g2->getStrWidth(str);
}

uint8_t DisplayManager::scanI2C(uint8_t* foundAddrs, uint8_t maxAddrs) {
    uint8_t count = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            if (count < maxAddrs) {
                foundAddrs[count++] = addr;
            }
        }
    }
    return count;
}
