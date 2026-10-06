#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <esp_now.h>

#include "image__25__bits.h"
#include "image_Battery_bits.h"
#include "image_ButtonLeftSmall_bits.h"
#include "image_ButtonRightSmall_bits.h"
#include "image_Credits_bits.h"
#include "image_Mode_bits.h"
#include "image_Power_bits.h"
#include "image_Return_bits.h"
#include "image_Settings_bits.h"
#include "image_wifi_bits.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const int BUTTON_PIN = 27;

int selected = 0;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void drawMenu() {
    u8g2.clearBuffer();
    u8g2.setBitmapMode(1);
    u8g2.setFontMode(1);

    u8g2.drawXBM(108, 2, 16, 8, image_Battery_bits);

    u8g2.drawXBM(110, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(113, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(116, 4, 2, 4, image__25__bits);
    u8g2.drawXBM(119, 4, 2, 4, image__25__bits);

    u8g2.drawXBM(5, 2, 11, 8, image_wifi_bits);

    u8g2.drawLine(2, 12, 125, 12);

    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(87, 10, "69%");

    u8g2.drawXBM(117, 31, 6, 10, image_ButtonRightSmall_bits);
    u8g2.drawXBM(5, 31, 6, 10, image_ButtonLeftSmall_bits);

    switch (selected) {
        case 0:
            u8g2.drawXBM(49, 17, 30, 32, image_Power_bits);
            u8g2.drawStr(51, 59, "Power");
            break;

        case 1:
            u8g2.drawXBM(48, 17, 32, 32, image_Settings_bits);
            u8g2.drawStr(41, 59, "Settings");
            break;

        case 2:
            u8g2.drawXBM(47, 20, 33, 24, image_Mode_bits);
            u8g2.drawStr(52, 59, "Mode");
            break;

        case 3:
            u8g2.drawXBM(50, 28, 29, 14, image_Credits_bits);
            u8g2.drawStr(44, 59, "Credits");
            break;

        case 4:
            u8g2.drawXBM(49, 20, 30, 24, image_Return_bits);
            u8g2.drawStr(47, 59, "Return");
            break;
    }

    u8g2.sendBuffer();
}

void setup() {
    Serial.begin(115200);

    Wire.begin(21, 22);
    u8g2.begin();

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    esp_now_init();

    drawMenu();
}

void loop() {
    bool buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW && lastButtonState == HIGH) {
        if (millis() - lastDebounceTime > 150) {
            selected++;

            if (selected > 4) {
                selected = 0;
            }

            Serial.print("Selected: ");
            Serial.println(selected);

            drawMenu();

            lastDebounceTime = millis();
        }
    }

    lastButtonState = buttonState;
}