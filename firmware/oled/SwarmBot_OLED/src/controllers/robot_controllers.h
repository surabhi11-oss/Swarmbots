#pragma once
#include <Arduino.h>

class RobotControllers {
public:
    static void init();
    static void update();

    static void updateBattery();
    static bool isActionSuspended();

    static void setFollowLeader(uint8_t leaderId, uint8_t distanceCm);
    static void startFollow();
    static void stopFollow();

    static void setManualSpeed(uint8_t speedPercent);
    static void manualDrive(int8_t linear, int8_t angular);
    static void emergencyStop();

private:
    static uint32_t _lastBatteryRead;
    static uint32_t _lastMotorUpdate;
};
