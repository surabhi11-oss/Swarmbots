#include "ui.h"
#include "assets.h"
#include "../display/display_manager.h"
#include "../input/input_manager.h"
#include "../config/config_manager.h"
#include "../controllers/robot_controllers.h"

uint32_t UI::_bootStart = 0;
uint32_t UI::_lastRenderTime = 0;

// ============================================================
// TIMING CONSTANTS (EXACT USER SPECIFICATION)
// ============================================================

const uint16_t NODE_STEP_MS = 130;   // each node
const uint16_t EDGE_STEP_MS = 120;   // each hex edge
const uint16_t S_STEP_MS    = 110;   // each S segment
const uint16_t TEXT_STEP_MS = 90;    // each char
const uint16_t HOLD_MS      = 1400;  // final hold

const uint8_t NODE_COUNT  = 6;
const uint8_t EDGE_COUNT  = 6;
const uint8_t S_SEG_COUNT = 8;
const uint8_t TEXT_COUNT  = 9; // "SWARM BOT"

const uint32_t T_NODES_END = NODE_COUNT * NODE_STEP_MS;                          // 780 ms
const uint32_t T_EDGES_END = T_NODES_END + EDGE_COUNT * EDGE_STEP_MS;            // 1500 ms
const uint32_t T_S_END     = T_EDGES_END + S_SEG_COUNT * S_STEP_MS;              // 2380 ms
const uint32_t T_TEXT_END  = T_S_END + TEXT_COUNT * TEXT_STEP_MS;                // 3190 ms
const uint32_t T_END       = T_TEXT_END + HOLD_MS;                               // 4590 ms

// ============================================================
// LOGO GEOMETRY
// ============================================================

const int hexPts[6][2] = {
  {64, 4},   // top
  {84, 15},  // top-right
  {84, 36},  // bottom-right
  {64, 47},  // bottom
  {44, 36},  // bottom-left
  {44, 15}   // top-left
};

// Angular S / thunder path (9 vertices, 8 segments)
const int sPath[9][2] = {
  {74, 15},
  {63, 11},
  {53, 17},
  {53, 23},
  {67, 28},
  {75, 32},
  {75, 37},
  {66, 42},
  {54, 38}
};

// ============================================================
// DRAWING HELPERS
// ============================================================

static inline float clamp01(float v) {
  if (v < 0.0f) return 0.0f;
  if (v > 1.0f) return 1.0f;
  return v;
}

static void drawLogoThickLine(int x1, int y1, int x2, int y2) {
  U8G2* u = DisplayManager::getU8g2();
  if (!u) return;
  u->drawLine(x1, y1, x2, y2);
  u->drawLine(x1 + 1, y1, x2 + 1, y2);
}

static void drawPartialThickLine(int x1, int y1, int x2, int y2, float p) {
  p = clamp01(p);
  int xe = x1 + (int)((x2 - x1) * p);
  int ye = y1 + (int)((y2 - y1) * p);
  drawLogoThickLine(x1, y1, xe, ye);
}

// Stage 1: Nodes animated
static void drawNodesAnimated(uint32_t t) {
  U8G2* u = DisplayManager::getU8g2();
  if (!u) return;
  for (int i = 0; i < NODE_COUNT; i++) {
    uint32_t nodeStart = i * NODE_STEP_MS;
    uint32_t nodeEnd   = nodeStart + NODE_STEP_MS;

    if (t >= nodeEnd) {
      u->drawDisc(hexPts[i][0], hexPts[i][1], 2);
    }
    else if (t > nodeStart) {
      float p = (float)(t - nodeStart) / NODE_STEP_MS;
      int r = 1 + (int)(p * 1.5f);   // grows 1 -> 2
      u->drawDisc(hexPts[i][0], hexPts[i][1], r);
    }
  }
}

// Stage 2: Outer hex edges animated
static void drawEdgesAnimated(uint32_t t) {
  if (t <= T_NODES_END) return;
  uint32_t local = t - T_NODES_END;

  for (int i = 0; i < EDGE_COUNT; i++) {
    int next = (i + 1) % EDGE_COUNT;
    uint32_t edgeStart = i * EDGE_STEP_MS;
    uint32_t edgeEnd   = edgeStart + EDGE_STEP_MS;

    if (local >= edgeEnd) {
      drawLogoThickLine(
        hexPts[i][0], hexPts[i][1],
        hexPts[next][0], hexPts[next][1]
      );
    }
    else if (local > edgeStart) {
      float p = (float)(local - edgeStart) / EDGE_STEP_MS;
      drawPartialThickLine(
        hexPts[i][0], hexPts[i][1],
        hexPts[next][0], hexPts[next][1],
        p
      );
    }
  }
}

// Stage 3: Central S / thunder animated (3-pixel thickness)
static void drawSAnimated(uint32_t t) {
  if (t <= T_EDGES_END) return;
  uint32_t local = t - T_EDGES_END;
  U8G2* u = DisplayManager::getU8g2();
  if (!u) return;

  for (int i = 0; i < S_SEG_COUNT; i++) {
    uint32_t segStart = i * S_STEP_MS;
    uint32_t segEnd   = segStart + S_STEP_MS;

    if (local >= segEnd) {
      u->drawLine(sPath[i][0], sPath[i][1], sPath[i + 1][0], sPath[i + 1][1]);
      u->drawLine(sPath[i][0], sPath[i][1] + 1, sPath[i + 1][0], sPath[i + 1][1] + 1);
      u->drawLine(sPath[i][0] + 1, sPath[i][1], sPath[i + 1][0] + 1, sPath[i + 1][1]);
    }
    else if (local > segStart) {
      float p = (float)(local - segStart) / S_STEP_MS;
      int x1 = sPath[i][0];
      int y1 = sPath[i][1];
      int x2 = sPath[i + 1][0];
      int y2 = sPath[i + 1][1];

      int xe = x1 + (int)((x2 - x1) * p);
      int ye = y1 + (int)((y2 - y1) * p);

      u->drawLine(x1, y1, xe, ye);
      u->drawLine(x1, y1 + 1, xe, ye + 1);
      u->drawLine(x1 + 1, y1, xe + 1, ye);
    }
  }
}

// Stage 4: Typewriter wordmark reveal
static void drawWordmarkAnimated(uint32_t t) {
  if (t <= T_S_END) return;
  U8G2* u = DisplayManager::getU8g2();
  if (!u) return;

  const char *full = "SWARM BOT";
  uint32_t local = t - T_S_END;

  int visibleChars = local / TEXT_STEP_MS;
  if (visibleChars > TEXT_COUNT) visibleChars = TEXT_COUNT;

  char buf[10];
  for (int i = 0; i < visibleChars; i++) {
    buf[i] = full[i];
  }
  buf[visibleChars] = '\0';

  u->setFont(u8g2_font_5x8_tf);
  int fullWidth = u->getStrWidth(full);
  int x = (128 - fullWidth) / 2;
  u->drawStr(x, 63, buf);
}

// Credits content
static const char* s_creditsLines[] = {
    "SWARM BOT",
    "",
    "TEAM",
    "AARYA  ATHARVA",
    "PARTH  TANUSH",
    "SURABHI  UDIT",
    "SHREYA  ANUJ",
    "YASH  VEDANTI",
    "",
    "MENTORS",
    "RAJNARAYAN  RIYA",
    "",
    "SPONSORED BY",
    "ECS DEPARTMENT",
    "VESIT"
};
static const uint8_t s_creditsCount = sizeof(s_creditsLines) / sizeof(s_creditsLines[0]);

void UI::init() {
    _bootStart = millis();
    g_botState.currentPage = PAGE_BOOT;
    _lastRenderTime = millis();
}

void UI::setPage(UiPage newPage) {
    g_botState.currentPage = newPage;
    g_botState.holdProgressMs = 0;
    g_botState.holdTarget = 0;
    InputManager::setInteractionActive(newPage != PAGE_HOME && newPage != PAGE_BOOT);
}

void UI::handleNavEvent(NavEvent ev) {
    if (ev == NAV_NONE) return;

    if (ev == NAV_POWER) {
        setPage(PAGE_POWER_CONFIRM);
        return;
    }

    if (ev == NAV_HOME) {
        setPage(PAGE_HOME);
        return;
    }

    switch (g_botState.currentPage) {
        case PAGE_HOME:
            if (ev == NAV_SELECT) {
                setPage(PAGE_MAIN_MENU);
            }
            break;

        case PAGE_MAIN_MENU:
            if (ev == NAV_NEXT) {
                g_botState.selectedMenuItem = (MainMenuItem)((g_botState.selectedMenuItem + 1) % MENU_ITEM_COUNT);
            } else if (ev == NAV_PREVIOUS) {
                g_botState.selectedMenuItem = (MainMenuItem)((g_botState.selectedMenuItem + MENU_ITEM_COUNT - 1) % MENU_ITEM_COUNT);
            } else if (ev == NAV_BACK) {
                setPage(PAGE_HOME);
            } else if (ev == NAV_SELECT) {
                switch (g_botState.selectedMenuItem) {
                    case MENU_MODES:    setPage(PAGE_MODE_SELECT); break;
                    case MENU_SETTINGS: setPage(PAGE_SETTINGS); break;
                    case MENU_CREDITS:  setPage(PAGE_CREDITS); break;
                    case MENU_POWER:    setPage(PAGE_POWER_CONFIRM); break;
                    case MENU_RETURN:   setPage(PAGE_HOME); break;
                    default: break;
                }
            }
            break;

        case PAGE_MODE_SELECT:
            if (ev == NAV_NEXT) {
                g_botState.modeSelectCursor = (g_botState.modeSelectCursor + 1) % 3;
            } else if (ev == NAV_PREVIOUS) {
                g_botState.modeSelectCursor = (g_botState.modeSelectCursor + 2) % 3;
            } else if (ev == NAV_BACK) {
                setPage(PAGE_MAIN_MENU);
            } else if (ev == NAV_SELECT) {
                if (g_botState.modeSelectCursor == 0) {
                    g_botState.activeMode = MODE_FOLLOW;
                    setPage(PAGE_FOLLOW_UTILITIES);
                } else if (g_botState.modeSelectCursor == 1) {
                    g_botState.activeMode = MODE_MANUAL;
                    setPage(PAGE_MANUAL_UTILITIES);
                } else {
                    setPage(PAGE_MAIN_MENU);
                }
            }
            break;

        case PAGE_FOLLOW_UTILITIES:
            if (ev == NAV_NEXT) {
                g_botState.followUtilityCursor = (g_botState.followUtilityCursor + 1) % 4;
            } else if (ev == NAV_PREVIOUS) {
                g_botState.followUtilityCursor = (g_botState.followUtilityCursor + 3) % 4;
            } else if (ev == NAV_BACK) {
                setPage(PAGE_MODE_SELECT);
            } else if (ev == NAV_SELECT) {
                if (g_botState.followUtilityCursor == 0) {
                    g_botState.followLeaderId = (g_botState.followLeaderId % 9) + 1;
                } else if (g_botState.followUtilityCursor == 1) {
                    g_botState.followDistanceCm = (g_botState.followDistanceCm >= 30) ? 15 : g_botState.followDistanceCm + 5;
                } else if (g_botState.followUtilityCursor == 2) {
                    if (g_botState.followIsRunning) RobotControllers::stopFollow();
                    else RobotControllers::startFollow();
                } else {
                    setPage(PAGE_MODE_SELECT);
                }
            }
            break;

        case PAGE_MANUAL_UTILITIES:
            if (ev == NAV_NEXT) {
                g_botState.manualUtilityCursor = (g_botState.manualUtilityCursor + 1) % 4;
            } else if (ev == NAV_PREVIOUS) {
                g_botState.manualUtilityCursor = (g_botState.manualUtilityCursor + 3) % 4;
            } else if (ev == NAV_BACK) {
                setPage(PAGE_MODE_SELECT);
            } else if (ev == NAV_SELECT) {
                if (g_botState.manualUtilityCursor == 0) {
                    if (strcmp(g_botState.manualControlSource, "WEB") == 0) {
                        strcpy(g_botState.manualControlSource, "JOY");
                    } else {
                        strcpy(g_botState.manualControlSource, "WEB");
                    }
                } else if (g_botState.manualUtilityCursor == 1) {
                    g_botState.manualSpeedPercent = (g_botState.manualSpeedPercent >= 100) ? 25 : g_botState.manualSpeedPercent + 25;
                } else if (g_botState.manualUtilityCursor == 2) {
                    g_botState.manualIsRunning = !g_botState.manualIsRunning;
                } else {
                    setPage(PAGE_MODE_SELECT);
                }
            }
            break;

        case PAGE_SETTINGS:
            if (ev == NAV_NEXT) {
                g_botState.settingsCursor = (g_botState.settingsCursor + 1) % 5;
            } else if (ev == NAV_PREVIOUS) {
                g_botState.settingsCursor = (g_botState.settingsCursor + 4) % 5;
            } else if (ev == NAV_BACK) {
                setPage(PAGE_MAIN_MENU);
            } else if (ev == NAV_SELECT) {
                if (g_botState.settingsCursor == 3) {
                    ConfigManager::resetDefaults();
                    Serial.println("[UI] Defaults restored from OLED Settings.");
                } else if (g_botState.settingsCursor == 4) {
                    setPage(PAGE_MAIN_MENU);
                }
            }
            break;

        case PAGE_CREDITS:
            if (ev == NAV_BACK || ev == NAV_SELECT || ev == NAV_NEXT) {
                setPage(PAGE_MAIN_MENU);
            }
            break;

        case PAGE_POWER_CONFIRM:
            if (ev == NAV_BACK) {
                setPage(PAGE_HOME);
            } else if (ev == NAV_SELECT) {
                setPage(PAGE_SHUTDOWN);
            }
            break;

        case PAGE_SHUTDOWN:
            break;

        default:
            break;
    }
}

// ============================================================
// STATUS BAR
// ============================================================

void UI::renderStatusBar(bool showMode) {
    DisplayManager::drawXBM(2, 2, 11, 8, icon_Wifi_bits);

    char botBuf[12];
    snprintf(botBuf, sizeof(botBuf), "%dBOT", g_botState.connectedBots);
    DisplayManager::setFont(u8g2_font_5x8_tr);
    DisplayManager::drawStr(15, 9, botBuf);

    char batStr[8];
    snprintf(batStr, sizeof(batStr), "%u%%", g_botState.batteryPercent);
    int batStrW = DisplayManager::getStrWidth(batStr);
    DisplayManager::drawStr(106 - batStrW, 9, batStr);

    DisplayManager::drawXBM(109, 1, 16, 8, icon_Battery_bits);

    uint8_t pct = g_botState.batteryPercent;
    if (pct >= 20) DisplayManager::drawXBM(111, 3, 2, 4, icon_Bat25_bits);
    if (pct >= 45) DisplayManager::drawXBM(114, 3, 2, 4, icon_Bat25_bits);
    if (pct >= 70) DisplayManager::drawXBM(117, 3, 2, 4, icon_Bat25_bits);
    if (pct >= 90) DisplayManager::drawXBM(120, 3, 2, 4, icon_Bat25_bits);

    DisplayManager::drawLine(0, 11, 127, 11);
}

// ============================================================
// BOOT ANIMATION (EXACT SEQUENCE + TIMINGS)
// ============================================================

void UI::renderBoot() {
    uint32_t elapsed = millis() - _bootStart;
    U8G2* u = DisplayManager::getU8g2();
    if (!u) return;

    // Render sequential layers according to exact millisecond checkpoints
    drawNodesAnimated(elapsed);
    drawEdgesAnimated(elapsed);
    drawSAnimated(elapsed);
    drawWordmarkAnimated(elapsed);

    // Innovative touch: subtle high-tech node radar ping when logo locks
    if (elapsed > T_TEXT_END && elapsed < T_TEXT_END + 350) {
        float pingP = (float)(elapsed - T_TEXT_END) / 350.0f;
        int ringR = 3 + (int)(pingP * 2.0f);
        for (int i = 0; i < NODE_COUNT; i++) {
            u->drawCircle(hexPts[i][0], hexPts[i][1], ringR);
        }
    }

    // Hold final frame clean and transition to Home at T_END (4590 ms)
    if (elapsed >= T_END) {
        setPage(PAGE_HOME);
    }
}

// ============================================================
// POLISHED HOME SCREEN
// ============================================================

void UI::renderHome() {
    renderStatusBar(true);

    // Bot ID title: centered, crisp, bold font (baseline Y=24)
    char botTitle[16];
    snprintf(botTitle, sizeof(botTitle), "BOT %02d", g_botState.botId);
    DisplayManager::setFont(u8g2_font_7x14B_tr);
    DisplayManager::drawStrCenter(24, botTitle);

    // Active mode: centered, technical font (baseline Y=35)
    DisplayManager::setFont(u8g2_font_5x8_tr);
    if (g_botState.activeMode == MODE_FOLLOW) {
        DisplayManager::drawStrCenter(35, "[FOLLOW LEADER]");
    } else if (g_botState.activeMode == MODE_MANUAL) {
        DisplayManager::drawStrCenter(35, "[MANUAL MODE]");
    } else {
        DisplayManager::drawStrCenter(35, "[IDLE MODE]");
    }

    // Explicit network state: Node visual + Text (baseline Y=46)
    DisplayManager::setFont(u8g2_font_5x8_tr);
    if (g_botState.networkState == NET_CONNECTED) {
        char netText[24];
        snprintf(netText, sizeof(netText), "%d/%d CONNECTED", g_botState.connectedBots, g_botState.connectedBots);
        int txtW = DisplayManager::getStrWidth(netText);
        int totalW = 12 + 4 + txtW;
        int startX = (128 - totalW) / 2;

        DisplayManager::fillCircle(startX + 2, 43, 2);
        DisplayManager::drawLine(startX + 2, 43, startX + 10, 43);
        DisplayManager::fillCircle(startX + 10, 43, 2);
        DisplayManager::drawStr(startX + 16, 46, netText);
    } else if (g_botState.networkState == NET_CONNECTING) {
        int startX = (128 - (12 + 4 + DisplayManager::getStrWidth("CONNECTING..."))) / 2;
        if ((millis() / 250) % 2 == 0) {
            DisplayManager::fillCircle(startX + 2, 43, 2);
            DisplayManager::fillCircle(startX + 10, 43, 2);
        } else {
            DisplayManager::drawCircle(startX + 2, 43, 2);
            DisplayManager::drawCircle(startX + 10, 43, 2);
        }
        DisplayManager::drawStr(startX + 16, 46, "CONNECTING...");
    } else {
        int startX = (128 - (8 + 4 + DisplayManager::getStrWidth("DISCONNECTED"))) / 2;
        DisplayManager::drawCircle(startX + 3, 43, 2);
        DisplayManager::drawStr(startX + 12, 46, "DISCONNECTED");
    }

    // Bottom hold progress feedback (Y=54 to 61)
    if (g_botState.holdProgressMs >= 200) {
        DisplayManager::drawFrame(22, 54, 84, 7);
        int w = map(constrain((int)g_botState.holdProgressMs, 200, 3000), 200, 3000, 0, 80);
        DisplayManager::drawBox(24, 56, w, 3);
    } else {
        DisplayManager::setFont(u8g2_font_4x6_tr);
        DisplayManager::drawStrCenter(62, "HOLD:MENU   LONG:PWR");
    }
}

// ============================================================
// POLISHED MAIN MENU CAROUSEL
// ============================================================

void UI::renderMainMenu() {
    // 5 carousel position indicator dots at top (Y=4)
    int dotStartX = 64 - (5 * 7) / 2;
    U8G2* u = DisplayManager::getU8g2();
    for (uint8_t i = 0; i < 5; i++) {
        if (i == (uint8_t)g_botState.selectedMenuItem) {
            DisplayManager::fillCircle(dotStartX + i * 7 + 2, 4, 2);
        } else if (u) {
            u->drawPixel(dotStartX + i * 7 + 2, 4);
        }
    }

    // Left and right navigation arrows
    DisplayManager::drawXBM(3, 27, 6, 10, icon_ArrowLeft_bits);
    DisplayManager::drawXBM(119, 27, 6, 10, icon_ArrowRight_bits);

    // Selected carousel icon and label
    switch (g_botState.selectedMenuItem) {
        case MENU_MODES:
            DisplayManager::drawXBM(47, 14, 33, 24, icon_Mode_bits);
            DisplayManager::setFont(u8g2_font_6x12_tr);
            DisplayManager::drawStrCenter(57, "MODES");
            break;
        case MENU_SETTINGS:
            DisplayManager::drawXBM(48, 10, 32, 32, icon_Settings_bits);
            DisplayManager::setFont(u8g2_font_6x12_tr);
            DisplayManager::drawStrCenter(57, "SETTINGS");
            break;
        case MENU_CREDITS:
            DisplayManager::drawXBM(49, 19, 29, 14, icon_Credits_bits);
            DisplayManager::setFont(u8g2_font_6x12_tr);
            DisplayManager::drawStrCenter(57, "CREDITS");
            break;
        case MENU_POWER:
            DisplayManager::drawXBM(49, 10, 30, 32, icon_Power_bits);
            DisplayManager::setFont(u8g2_font_6x12_tr);
            DisplayManager::drawStrCenter(57, "POWER");
            break;
        case MENU_RETURN:
            DisplayManager::drawXBM(49, 14, 30, 24, icon_Return_bits);
            DisplayManager::setFont(u8g2_font_6x12_tr);
            DisplayManager::drawStrCenter(57, "RETURN");
            break;
        default: break;
    }
}

// ============================================================
// POLISHED SUBPAGES
// ============================================================

void UI::renderModeSelect() {
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(11, "SELECT MODE");
    DisplayManager::drawLine(0, 13, 127, 13);

    const char* options[3] = {"1. Follow Leader", "2. Manual Control", "< Back to Menu"};
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t y = 26 + (i * 12);
        if (i == g_botState.modeSelectCursor) {
            DisplayManager::drawBox(6, y - 9, 116, 11);
            U8G2* u = DisplayManager::getU8g2();
            if (u) u->setDrawColor(0);
            DisplayManager::drawStr(12, y, options[i]);
            if (u) u->setDrawColor(1);
        } else {
            DisplayManager::drawStr(12, y, options[i]);
        }
    }
}

void UI::renderFollowUtilities() {
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(11, "FOLLOW LEADER");
    DisplayManager::drawLine(0, 13, 127, 13);

    DisplayManager::setFont(u8g2_font_5x8_tr);
    char idLine[24];
    snprintf(idLine, sizeof(idLine), "Leader: Bot %02d", g_botState.followLeaderId);
    char distLine[24];
    snprintf(distLine, sizeof(distLine), "Distance: %d cm", g_botState.followDistanceCm);
    char stateLine[24];
    snprintf(stateLine, sizeof(stateLine), "Drive: %s", g_botState.followIsRunning ? "RUNNING" : "IDLE");

    const char* items[4] = {idLine, distLine, stateLine, "< Back to Modes"};
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t y = 24 + (i * 10);
        if (i == g_botState.followUtilityCursor) {
            DisplayManager::drawBox(6, y - 8, 116, 10);
            U8G2* u = DisplayManager::getU8g2();
            if (u) u->setDrawColor(0);
            DisplayManager::drawStr(10, y, items[i]);
            if (u) u->setDrawColor(1);
        } else {
            DisplayManager::drawStr(10, y, items[i]);
        }
    }
}

void UI::renderManualUtilities() {
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(11, "MANUAL CONTROL");
    DisplayManager::drawLine(0, 13, 127, 13);

    DisplayManager::setFont(u8g2_font_5x8_tr);
    char srcLine[24];
    snprintf(srcLine, sizeof(srcLine), "Source: %s", g_botState.manualControlSource);
    char spdLine[24];
    snprintf(spdLine, sizeof(spdLine), "Speed: %d%%", g_botState.manualSpeedPercent);
    char stateLine[24];
    snprintf(stateLine, sizeof(stateLine), "Drive: %s", g_botState.manualIsRunning ? "ACTIVE" : "STOPPED");

    const char* items[4] = {srcLine, spdLine, stateLine, "< Back to Modes"};
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t y = 24 + (i * 10);
        if (i == g_botState.manualUtilityCursor) {
            DisplayManager::drawBox(6, y - 8, 116, 10);
            U8G2* u = DisplayManager::getU8g2();
            if (u) u->setDrawColor(0);
            DisplayManager::drawStr(10, y, items[i]);
            if (u) u->setDrawColor(1);
        } else {
            DisplayManager::drawStr(10, y, items[i]);
        }
    }
}

void UI::renderSettings() {
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(11, "SETTINGS");
    DisplayManager::drawLine(0, 13, 127, 13);

    DisplayManager::setFont(u8g2_font_5x8_tr);
    const AppConfig& cfg = ConfigManager::get();
    char drvLine[24];
    snprintf(drvLine, sizeof(drvLine), "Driver: %s 0x%02X",
             (cfg.oledDriver == OLED_DRIVER_SH1106 ? "SH1106" : "SSD1306"), cfg.oledI2cAddr);
    char inLine[24];
    snprintf(inLine, sizeof(inLine), "Input: %s (P%d)",
             (cfg.inputMode == INPUT_MODE_SINGLE ? "1-BTN" : "MULTI"), cfg.pinBtnSingle);

    const char* items[5] = {drvLine, inLine, "Web: 192.168.4.1", "Reset Defaults", "< Back to Menu"};
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t y = 23 + (i * 9);
        if (i == g_botState.settingsCursor) {
            DisplayManager::drawBox(4, y - 7, 120, 9);
            U8G2* u = DisplayManager::getU8g2();
            if (u) u->setDrawColor(0);
            DisplayManager::drawStr(8, y, items[i]);
            if (u) u->setDrawColor(1);
        } else {
            DisplayManager::drawStr(8, y, items[i]);
        }
    }
}

void UI::renderCredits() {
    DisplayManager::setFont(u8g2_font_6x10_tr);
    DisplayManager::drawStrCenter(10, "CREDITS");
    DisplayManager::drawLine(0, 12, 127, 12);

    DisplayManager::setFont(u8g2_font_5x8_tr);
    constexpr int lineH = 9;
    const int totalHeight = s_creditsCount * lineH;
    const int viewportH = 50;
    const int travel = totalHeight + viewportH;
    int offset = (millis() / 90) % travel;
    int y = 14 + viewportH - offset;

    for (uint8_t i = 0; i < s_creditsCount; i++) {
        if (y >= 20 && y <= 63) {
            DisplayManager::drawStrCenter(y, s_creditsLines[i]);
        }
        y += lineH;
    }
}

void UI::renderPowerConfirm() {
    DisplayManager::drawXBM(49, 2, 30, 32, icon_Power_bits);
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(42, "POWER OFF?");
    DisplayManager::setFont(u8g2_font_5x8_tr);
    DisplayManager::drawStrCenter(52, "HOLD 1.5s: CONFIRM");
    DisplayManager::drawStrCenter(61, "TAP: CANCEL");
}

void UI::renderShutdown() {
    DisplayManager::drawXBM(49, 6, 30, 32, icon_Power_bits);
    DisplayManager::setFont(u8g2_font_6x12_tr);
    DisplayManager::drawStrCenter(46, "SAFE TO POWER OFF");
    DisplayManager::setFont(u8g2_font_5x8_tr);
    DisplayManager::drawStrCenter(58, "SYSTEM HALTED");
}

void UI::update() {
    while (InputManager::hasEvent()) {
        NavEvent ev = InputManager::popEvent();
        handleNavEvent(ev);
    }

    uint32_t now = millis();
    if (now - _lastRenderTime < 33) return; // ~30 FPS max
    _lastRenderTime = now;

    DisplayManager::clear();

    switch (g_botState.currentPage) {
        case PAGE_BOOT:             renderBoot(); break;
        case PAGE_HOME:             renderHome(); break;
        case PAGE_MAIN_MENU:        renderMainMenu(); break;
        case PAGE_MODE_SELECT:      renderModeSelect(); break;
        case PAGE_FOLLOW_UTILITIES: renderFollowUtilities(); break;
        case PAGE_MANUAL_UTILITIES: renderManualUtilities(); break;
        case PAGE_SETTINGS:         renderSettings(); break;
        case PAGE_CREDITS:          renderCredits(); break;
        case PAGE_POWER_CONFIRM:    renderPowerConfirm(); break;
        case PAGE_SHUTDOWN:         renderShutdown(); break;
        default: break;
    }

    DisplayManager::display();
}
