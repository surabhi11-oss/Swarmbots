// =============================================================================
// SWARM BOT UNIFIED ESP32 FIRMWARE
// Combining flexible OLED drivers, gesture/multi-button engine, live web twin,
// and interaction-safe autonomous locking.
// =============================================================================

#include <Arduino.h>
#include "src/config.h"
#include "src/state/bot_state.h"
#include "src/config/config_manager.h"
#include "src/display/display_manager.h"
#include "src/input/input_manager.h"
#include "src/controllers/robot_controllers.h"
#include "src/ui/ui.h"
#include "src/web/web_server.h"

static uint32_t s_lastUptimeCheck = 0;

void setup() {
    Serial.begin(115200);
    delay(250);
    Serial.println();
    Serial.println("=================================================");
    Serial.println("           SWARM BOT ESP32 FIRMWARE              ");
    Serial.println("=================================================");

    // 1. Check recovery hardware button (GPIO 27 hold 5s on boot)
    ConfigManager::checkBootRecovery();

    // 2. Load persistent hardware configuration from NVS
    ConfigManager::init();
    const AppConfig& cfg = ConfigManager::get();
    g_botState.botId = cfg.botId;

    // 3. Initialize OLED display
    DisplayManager::begin(cfg.pinOledSda, cfg.pinOledScl, cfg.oledI2cAddr, cfg.oledDriver);

    // 4. Initialize Input system
    InputManager::init();

    // 5. Initialize Robot Controllers & Hardware stubs
    RobotControllers::init();

    // 6. Initialize UI state machine
    UI::init();

    // 7. Start SoftAP & Web Server
    WebServerManager::begin();

    s_lastUptimeCheck = millis();
    Serial.println("[SYSTEM] Setup completed successfully.");
}

void loop() {
    // 1. Process physical button & virtual inputs
    InputManager::update();

    // 2. Update robot motion & battery ADC monitoring
    RobotControllers::update();

    // 3. Render OLED display frame
    UI::update();

    // 4. Handle incoming HTTP dashboard requests
    WebServerManager::update();

    // 5. Update uptime counter
    uint32_t now = millis();
    if (now - s_lastUptimeCheck >= 1000) {
        s_lastUptimeCheck = now;
        g_botState.uptimeSeconds++;
    }

    // Cooperative yield for ESP32 FreeRTOS tasks
    vTaskDelay(1);
}
