#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <esp_now.h>

#include "font_6x9.h"
#include "image__25__bits.h"
#include "image_Battery_bits.h"
#include "image_FaceCharging_bits.h"
#include "image_wifi_bits.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

bool espNowConnected = true;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    u8g2.begin();
    WiFi.mode(WIFI_STA);

    if (esp_now_init() == ESP_OK) {
        Serial.println("ESP-NOW initialized");
    } else {
        Serial.println("ESP-NOW initialization failed");
    }
}

void loop() {
    u8g2.clearBuffer();
    u8g2.setBitmapMode(1);
    u8g2.setFontMode(1);

    u8g2.drawXBM(108, 2, 16, 8, image_Battery_bits);

    u8g2.drawXBM(110, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(113, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(116, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(119, 4, 2, 4, image__25__bits);

    u8g2.drawXBM(5, 2, 11, 8, image_wifi_bits);

    u8g2.drawXBM(49, 17, 29, 14, image_FaceCharging_bits);

    u8g2.setFont(u8g2_font_profont22_tr);
    u8g2.drawStr(28, 47, "Bot 01");

    u8g2.drawLine(3, 12, 125, 12);

    u8g2.setFont(font_6x9);
    u8g2.drawUTF8(32, 59, espNowConnected ? "•Connected" : "•Disconnected");

    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(87, 10, "69%");

    u8g2.sendBuffer();

    delay(100);
}