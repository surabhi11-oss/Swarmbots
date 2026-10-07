#pragma once
#include <Arduino.h>
#include "../config.h"
#include "../state/bot_state.h"

class UI {
public:
    static void init();
    static void update();
    static void handleNavEvent(NavEvent ev);
    static void setPage(UiPage newPage);

private:
    static void renderBoot();
    static void renderHome();
    static void renderMainMenu();
    static void renderModeSelect();
    static void renderFollowUtilities();
    static void renderManualUtilities();
    static void renderSettings();
    static void renderCredits();
    static void renderPowerConfirm();
    static void renderShutdown();

    static void renderStatusBar(bool showMode = true);

    static uint32_t _bootStart;
    static uint32_t _lastRenderTime;
};
