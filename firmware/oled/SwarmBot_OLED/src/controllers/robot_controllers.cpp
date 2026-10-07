#include "robot_controllers.h"
#include "../config/config_manager.h"
#include "../state/bot_state.h"

uint32_t RobotControllers::_lastBatteryRead = 0;
uint32_t RobotControllers::_lastMotorUpdate = 0;

void RobotControllers::init() {
    const AppConfig& cfg = ConfigManager::get();
    if (cfg.pinBatteryAdc >= 0) {
        analogReadResolution(12);
        Serial.printf("[ROBOT] Battery ADC initialized on GPIO %d.\n", cfg.pinBatteryAdc);
    } else {
        Serial.println("[ROBOT] Battery ADC disabled (-1). Using nominal/debug battery values.");
    }

    if (cfg.pinPowerLatch >= 0) {
        pinMode(cfg.pinPowerLatch, OUTPUT);
        digitalWrite(cfg.pinPowerLatch, HIGH);
        Serial.printf("[ROBOT] Power latch pin active on GPIO %d.\n", cfg.pinPowerLatch);
    }
}

void RobotControllers::updateBattery() {
    const AppConfig& cfg = ConfigManager::get();
    if (cfg.pinBatteryAdc < 0) {
        g_botState.batteryIsReal = false;
        return;
    }

    uint32_t now = millis();
    if (now - _lastBatteryRead >= 1000) {
        _lastBatteryRead = now;

        uint32_t raw = analogRead(cfg.pinBatteryAdc);
        // Assuming 1:1 voltage divider (max 6.6V), 3.3V reference, 12-bit ADC (4095)
        float v = (raw / 4095.0f) * 3.3f * 2.0f;
        g_botState.batteryVoltage = v;

        // LiPo single cell voltage mapping: 3.3V = 0%, 4.2V = 100%
        int pct = (int)((v - 3.30f) / (4.20f - 3.30f) * 100.0f);
        g_botState.batteryPercent = constrain(pct, 0, 100);
        g_botState.batteryIsReal = true;
    }
}

bool RobotControllers::isActionSuspended() {
    return g_botState.uiInteractionLock || g_botState.isShutdown;
}

void RobotControllers::update() {
    updateBattery();

    if (isActionSuspended()) {
        // Safety lock active: neutralize motors
        emergencyStop();
        return;
    }

    uint32_t now = millis();
    if (now - _lastMotorUpdate >= 50) {
        _lastMotorUpdate = now;

        if (g_botState.activeMode == MODE_FOLLOW && g_botState.followIsRunning) {
            // Motion controller follow loop hook
        } else if (g_botState.activeMode == MODE_MANUAL && g_botState.manualIsRunning) {
            // Motion controller manual drive loop hook
        }
    }
}

void RobotControllers::setFollowLeader(uint8_t leaderId, uint8_t distanceCm) {
    g_botState.followLeaderId = leaderId;
    g_botState.followDistanceCm = distanceCm;
    Serial.printf("[ROBOT] Follow Leader target set to Bot %02d, distance %d cm.\n", leaderId, distanceCm);
}

void RobotControllers::startFollow() {
    g_botState.followIsRunning = true;
    Serial.printf("[ROBOT] Follow Leader started towards Bot %02d.\n", g_botState.followLeaderId);
}

void RobotControllers::stopFollow() {
    g_botState.followIsRunning = false;
    emergencyStop();
    Serial.println("[ROBOT] Follow Leader stopped.");
}

void RobotControllers::setManualSpeed(uint8_t speedPercent) {
    g_botState.manualSpeedPercent = constrain(speedPercent, 10, 100);
    Serial.printf("[ROBOT] Manual speed set to %d%%.\n", g_botState.manualSpeedPercent);
}

void RobotControllers::manualDrive(int8_t linear, int8_t angular) {
    if (isActionSuspended()) {
        emergencyStop();
        return;
    }
    g_botState.manualIsRunning = (linear != 0 || angular != 0);
    Serial.printf("[ROBOT] Manual drive linear=%d, angular=%d\n", linear, angular);
}

void RobotControllers::emergencyStop() {
    // Neutralize motor drive outputs (hardware interface hook)
}
