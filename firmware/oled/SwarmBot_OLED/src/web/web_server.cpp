#include "web_server.h"
#include <WiFi.h>
#include <WebServer.h>
#include "web_page.h"
#include "../state/bot_state.h"
#include "../config/config_manager.h"
#include "../input/input_manager.h"
#include "../display/display_manager.h"

static WebServer s_server(80);

static void handleRoot() {
    s_server.send_P(200, "text/html", INDEX_HTML);
}

static void handleGetState() {
    char json[512];
    const char* pName = "HOME";
    switch (g_botState.currentPage) {
        case PAGE_BOOT: pName = "BOOT"; break;
        case PAGE_HOME: pName = "HOME"; break;
        case PAGE_MAIN_MENU: pName = "MAIN_MENU"; break;
        case PAGE_MODE_SELECT: pName = "MODE_SELECT"; break;
        case PAGE_FOLLOW_UTILITIES: pName = "FOLLOW_UTILITIES"; break;
        case PAGE_MANUAL_UTILITIES: pName = "MANUAL_UTILITIES"; break;
        case PAGE_SETTINGS: pName = "SETTINGS"; break;
        case PAGE_CREDITS: pName = "CREDITS"; break;
        case PAGE_POWER_CONFIRM: pName = "POWER_CONFIRM"; break;
        case PAGE_SHUTDOWN: pName = "SHUTDOWN"; break;
    }

    const char* mName = (g_botState.activeMode == MODE_FOLLOW) ? "FOLLOW" : "MANUAL";
    const char* netName = (g_botState.networkState == NET_CONNECTED) ? "connected" :
                          (g_botState.networkState == NET_CONNECTING) ? "connecting" : "disconnected";

    int activeCursor = 0;
    if (g_botState.currentPage == PAGE_MODE_SELECT) activeCursor = g_botState.modeSelectCursor;
    else if (g_botState.currentPage == PAGE_FOLLOW_UTILITIES) activeCursor = g_botState.followUtilityCursor;
    else if (g_botState.currentPage == PAGE_MANUAL_UTILITIES) activeCursor = g_botState.manualUtilityCursor;
    else if (g_botState.currentPage == PAGE_SETTINGS) activeCursor = g_botState.settingsCursor;

    snprintf(json, sizeof(json),
        "{\"botId\":%d,\"battery\":%d,\"voltage\":%.2f,\"batteryReal\":%s,"
        "\"net\":\"%s\",\"peers\":%d,\"mode\":\"%s\",\"page\":\"%s\","
        "\"menuIdx\":%d,\"cursor\":%d,\"holdMs\":%d,\"holdTarget\":%d,\"uiLocked\":%s}",
        g_botState.botId, g_botState.batteryPercent, g_botState.batteryVoltage,
        g_botState.batteryIsReal ? "true" : "false",
        netName, g_botState.connectedBots, mName, pName,
        (int)g_botState.selectedMenuItem, activeCursor,
        g_botState.holdProgressMs, g_botState.holdTarget,
        g_botState.uiInteractionLock ? "true" : "false"
    );

    s_server.send(200, "application/json", json);
}

static void handleNav() {
    if (s_server.hasArg("action")) {
        String act = s_server.arg("action");
        if (act == "NEXT") InputManager::postEvent(NAV_NEXT);
        else if (act == "PREV") InputManager::postEvent(NAV_PREVIOUS);
        else if (act == "SELECT") InputManager::postEvent(NAV_SELECT);
        else if (act == "BACK") InputManager::postEvent(NAV_BACK);
        else if (act == "HOME") InputManager::postEvent(NAV_HOME);
        else if (act == "POWER") InputManager::postEvent(NAV_POWER);
    } else if (s_server.hasArg("gesture")) {
        String g = s_server.arg("gesture");
        if (g == "TAP") InputManager::postEvent(NAV_NEXT);
        else if (g == "DOUBLE") InputManager::postEvent(NAV_BACK);
        else if (g == "HOLD_SELECT") InputManager::postEvent(NAV_SELECT);
        else if (g == "HOLD_POWER") InputManager::postEvent(NAV_POWER);
    }
    s_server.send(200, "text/plain", "OK");
}

static void handleI2CScan() {
    uint8_t addrs[16];
    uint8_t count = DisplayManager::scanI2C(addrs, 16);
    String json = "{\"count\":" + String(count) + ",\"addresses\":[";
    for (uint8_t i = 0; i < count; i++) {
        if (i > 0) json += ",";
        char buf[8];
        snprintf(buf, sizeof(buf), "\"0x%02X\"", addrs[i]);
        json += buf;
    }
    json += "]}";
    s_server.send(200, "application/json", json);
}

static void handleSaveConfig() {
    AppConfig cfg = ConfigManager::get();

    if (s_server.hasArg("botId")) cfg.botId = s_server.arg("botId").toInt();
    if (s_server.hasArg("oledDriver")) cfg.oledDriver = (OledDriverType)s_server.arg("oledDriver").toInt();
    if (s_server.hasArg("oledI2cAddr")) cfg.oledI2cAddr = (uint8_t)strtol(s_server.arg("oledI2cAddr").c_str(), NULL, 16);
    if (s_server.hasArg("pinOledSda")) cfg.pinOledSda = s_server.arg("pinOledSda").toInt();
    if (s_server.hasArg("pinOledScl")) cfg.pinOledScl = s_server.arg("pinOledScl").toInt();
    if (s_server.hasArg("inputMode")) cfg.inputMode = (InputModeType)s_server.arg("inputMode").toInt();
    if (s_server.hasArg("pinBtnSingle")) cfg.pinBtnSingle = s_server.arg("pinBtnSingle").toInt();
    if (s_server.hasArg("pinBtnPrev")) cfg.pinBtnPrev = s_server.arg("pinBtnPrev").toInt();
    if (s_server.hasArg("pinBtnNext")) cfg.pinBtnNext = s_server.arg("pinBtnNext").toInt();
    if (s_server.hasArg("pinBtnSelect")) cfg.pinBtnSelect = s_server.arg("pinBtnSelect").toInt();
    if (s_server.hasArg("pinBtnBack")) cfg.pinBtnBack = s_server.arg("pinBtnBack").toInt();
    if (s_server.hasArg("pinBatteryAdc")) cfg.pinBatteryAdc = s_server.arg("pinBatteryAdc").toInt();
    if (s_server.hasArg("pinPowerLatch")) cfg.pinPowerLatch = s_server.arg("pinPowerLatch").toInt();

    String errStr;
    if (!ConfigManager::validate(cfg, errStr)) {
        String resp = "{\"success\":false,\"error\":\"" + errStr + "\"}";
        s_server.send(400, "application/json", resp);
        return;
    }

    ConfigManager::save(cfg);
    s_server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuration saved. Reboot required.\"}");
}

static void handleReset() {
    ConfigManager::resetDefaults();
    s_server.send(200, "application/json", "{\"success\":true,\"message\":\"Defaults restored.\"}");
}

static void handleReboot() {
    s_server.send(200, "application/json", "{\"success\":true,\"message\":\"Rebooting ESP32...\"}");
    delay(500);
    ESP.restart();
}

void WebServerManager::begin() {
    const AppConfig& cfg = ConfigManager::get();
    Serial.println("[WEB] Initializing SoftAP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(cfg.apSsid, cfg.apPassword);
    Serial.printf("[WEB] AP SSID: %s (IP: %s)\n", cfg.apSsid, WiFi.softAPIP().toString().c_str());

    s_server.on("/", HTTP_GET, handleRoot);
    s_server.on("/api/state", HTTP_GET, handleGetState);
    s_server.on("/api/nav", HTTP_POST, handleNav);
    s_server.on("/api/i2c-scan", HTTP_GET, handleI2CScan);
    s_server.on("/api/config", HTTP_POST, handleSaveConfig);
    s_server.on("/api/reset", HTTP_POST, handleReset);
    s_server.on("/api/reboot", HTTP_POST, handleReboot);

    s_server.begin();
    Serial.println("[WEB] HTTP server listening on port 80.");
}

void WebServerManager::update() {
    s_server.handleClient();
}
