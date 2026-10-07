#pragma once
#include <Arduino.h>
#include "../config.h"

enum UiPage {
    PAGE_BOOT = 0,
    PAGE_HOME,
    PAGE_MAIN_MENU,
    PAGE_MODE_SELECT,
    PAGE_FOLLOW_UTILITIES,
    PAGE_MANUAL_UTILITIES,
    PAGE_SETTINGS,
    PAGE_CREDITS,
    PAGE_POWER_CONFIRM,
    PAGE_SHUTDOWN
};

enum BotMode {
    MODE_IDLE = 0,
    MODE_FOLLOW,
    MODE_MANUAL
};

enum NetworkState {
    NET_DISCONNECTED = 0,
    NET_CONNECTING,
    NET_CONNECTED
};

enum MainMenuItem {
    MENU_MODES = 0,
    MENU_SETTINGS,
    MENU_CREDITS,
    MENU_POWER,
    MENU_RETURN,
    MENU_ITEM_COUNT
};

struct BotState {
    uint8_t botId = 1;
    uint32_t uptimeSeconds = 0;

    uint8_t batteryPercent = 82;
    float batteryVoltage = 3.95f;
    bool batteryCharging = false;
    bool batteryIsReal = false;

    NetworkState networkState = NET_CONNECTED;
    uint8_t connectedBots = 4;
    bool networkIsReal = false;
    char apIP[16] = "192.168.4.1";
    char apSSID[32] = "SwarmBot-01";

    BotMode activeMode = MODE_FOLLOW;

    UiPage currentPage = PAGE_BOOT;
    MainMenuItem selectedMenuItem = MENU_MODES;
    uint8_t modeSelectCursor = 0;
    uint8_t followUtilityCursor = 0;
    uint8_t manualUtilityCursor = 0;
    uint8_t settingsCursor = 0;

    uint8_t followLeaderId = 1;
    uint8_t followDistanceCm = 25;
    bool followIsRunning = false;

    uint8_t manualSpeedPercent = 75;
    bool manualIsRunning = false;
    char manualControlSource[16] = "WEB";

    bool uiInteractionLock = false;
    uint16_t holdProgressMs = 0;
    uint8_t holdTarget = 0;

    bool isShutdown = false;
};

extern BotState g_botState;
