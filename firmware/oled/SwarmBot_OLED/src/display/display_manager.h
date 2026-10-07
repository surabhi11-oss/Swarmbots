#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "../config.h"

class DisplayManager {
public:
    static bool begin(int sda, int scl, uint8_t i2cAddr, OledDriverType driver);
    static void clear();
    static void display();

    static void drawStr(int x, int y, const char* str);
    static void drawStrCenter(int y, const char* str);
    static void drawStrRight(int x, int y, const char* str);

    static void drawBitmap(int x, int y, int byteWidth, int height, const uint8_t* bitmap);
    static void drawXBM(int x, int y, int width, int height, const uint8_t* bits);

    static void drawBox(int x, int y, int w, int h);
    static void drawFrame(int x, int y, int w, int h);
    static void drawLine(int x1, int y1, int x2, int y2);
    static void drawCircle(int x0, int y0, int rad);
    static void fillCircle(int x0, int y0, int rad);

    static void setFont(const uint8_t* font);
    static int getStrWidth(const char* str);

    static U8G2* getU8g2() { return _u8g2; }
    static bool isReady() { return _initialized; }

    static uint8_t scanI2C(uint8_t* foundAddrs, uint8_t maxAddrs);

private:
    static U8G2* _u8g2;
    static bool _initialized;
    static OledDriverType _driver;
    static uint8_t _i2cAddr;
};
