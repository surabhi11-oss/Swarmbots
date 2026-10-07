#pragma once
#include <Arduino.h>
#include "../config.h"

class InputManager {
public:
    static void init();
    static void update();
    static void postEvent(NavEvent ev);
    static bool hasEvent();
    static NavEvent popEvent();

    static void setInteractionActive(bool active);
    static bool isInteractionActive();

private:
    static NavEvent _eventQueue[8];
    static uint8_t _queueHead;
    static uint8_t _queueTail;

    static bool _rawSingle;
    static bool _stableSingle;
    static uint32_t _lastRawChange;
    static uint32_t _pressStart;
    static bool _waitingForSecondTap;
    static uint32_t _firstTapRelease;

    static bool _stablePrev;
    static bool _stableNext;
    static bool _stableSelect;
    static bool _stableBack;
    static uint32_t _lastMultiChange;

    static uint32_t _lastInteractionTime;
};
