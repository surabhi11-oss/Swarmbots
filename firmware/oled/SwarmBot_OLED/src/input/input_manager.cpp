#include "input_manager.h"
#include "../config/config_manager.h"
#include "../state/bot_state.h"

NavEvent InputManager::_eventQueue[8];
uint8_t InputManager::_queueHead = 0;
uint8_t InputManager::_queueTail = 0;

bool InputManager::_rawSingle = HIGH;
bool InputManager::_stableSingle = HIGH;
uint32_t InputManager::_lastRawChange = 0;
uint32_t InputManager::_pressStart = 0;
bool InputManager::_waitingForSecondTap = false;
uint32_t InputManager::_firstTapRelease = 0;

bool InputManager::_stablePrev = HIGH;
bool InputManager::_stableNext = HIGH;
bool InputManager::_stableSelect = HIGH;
bool InputManager::_stableBack = HIGH;
uint32_t InputManager::_lastMultiChange = 0;

uint32_t InputManager::_lastInteractionTime = 0;

void InputManager::init() {
    const AppConfig& cfg = ConfigManager::get();

    if (cfg.inputMode == INPUT_MODE_SINGLE) {
        if (cfg.pinBtnSingle >= 0) {
            pinMode(cfg.pinBtnSingle, INPUT_PULLUP);
            _rawSingle = digitalRead(cfg.pinBtnSingle);
            _stableSingle = _rawSingle;
        }
        Serial.printf("[INPUT] Single-button mode active on GPIO %d\n", cfg.pinBtnSingle);
    } else {
        if (cfg.pinBtnPrev >= 0) pinMode(cfg.pinBtnPrev, INPUT_PULLUP);
        if (cfg.pinBtnNext >= 0) pinMode(cfg.pinBtnNext, INPUT_PULLUP);
        if (cfg.pinBtnSelect >= 0) pinMode(cfg.pinBtnSelect, INPUT_PULLUP);
        if (cfg.pinBtnBack >= 0) pinMode(cfg.pinBtnBack, INPUT_PULLUP);
        Serial.printf("[INPUT] Multi-button mode: Prev=%d, Next=%d, Sel=%d, Back=%d\n",
                      cfg.pinBtnPrev, cfg.pinBtnNext, cfg.pinBtnSelect, cfg.pinBtnBack);
    }

    _queueHead = 0;
    _queueTail = 0;
    _waitingForSecondTap = false;
    _lastInteractionTime = millis();
}

void InputManager::postEvent(NavEvent ev) {
    if (ev == NAV_NONE) return;

    uint8_t nextHead = (_queueHead + 1) % 8;
    if (nextHead != _queueTail) {
        _eventQueue[_queueHead] = ev;
        _queueHead = nextHead;
    }

    setInteractionActive(true);
}

bool InputManager::hasEvent() {
    return (_queueHead != _queueTail);
}

NavEvent InputManager::popEvent() {
    if (_queueHead == _queueTail) return NAV_NONE;
    NavEvent ev = _eventQueue[_queueTail];
    _queueTail = (_queueTail + 1) % 8;
    return ev;
}

void InputManager::setInteractionActive(bool active) {
    g_botState.uiInteractionLock = active;
    _lastInteractionTime = millis();
}

bool InputManager::isInteractionActive() {
    return g_botState.uiInteractionLock;
}

void InputManager::update() {
    const AppConfig& cfg = ConfigManager::get();
    uint32_t now = millis();

    if (g_botState.uiInteractionLock && g_botState.currentPage == PAGE_HOME) {
        if (now - _lastInteractionTime > UI_INTERACTION_TIMEOUT_MS) {
            g_botState.uiInteractionLock = false;
        }
    }

    if (cfg.inputMode == INPUT_MODE_SINGLE && cfg.pinBtnSingle >= 0) {
        bool reading = digitalRead(cfg.pinBtnSingle);

        if (reading != _rawSingle) {
            _rawSingle = reading;
            _lastRawChange = now;
        }

        if ((now - _lastRawChange) >= BTN_DEBOUNCE_MS && reading != _stableSingle) {
            _stableSingle = reading;

            if (_stableSingle == LOW) {
                _pressStart = now;
            } else {
                uint32_t held = now - _pressStart;
                g_botState.holdProgressMs = 0;
                g_botState.holdTarget = 0;

                if (held >= BTN_HOLD_POWER_MIN_MS) {
                    _waitingForSecondTap = false;
                    postEvent(NAV_POWER);
                }
                else if (held >= BTN_HOLD_MENU_MIN_MS && held <= BTN_HOLD_MENU_MAX_MS) {
                    _waitingForSecondTap = false;
                    postEvent(NAV_SELECT);
                }
                else if (held >= BTN_TAP_MIN_MS && held <= BTN_TAP_MAX_MS) {
                    if (_waitingForSecondTap && (now - _firstTapRelease) <= BTN_DOUBLE_TAP_GAP_MS) {
                        _waitingForSecondTap = false;
                        postEvent(NAV_BACK);
                    } else {
                        _waitingForSecondTap = true;
                        _firstTapRelease = now;
                    }
                }
            }
        }

        if (_stableSingle == LOW) {
            uint32_t currentHeld = now - _pressStart;
            g_botState.holdProgressMs = currentHeld;
            if (g_botState.currentPage == PAGE_HOME) {
                if (currentHeld >= 300 && currentHeld < BTN_HOLD_MENU_MAX_MS) {
                    g_botState.holdTarget = 1;
                } else if (currentHeld >= BTN_HOLD_MENU_MAX_MS) {
                    g_botState.holdTarget = 2;
                } else {
                    g_botState.holdTarget = 0;
                }
            } else {
                g_botState.holdTarget = 0;
            }
        }

        if (_waitingForSecondTap && (now - _firstTapRelease) > BTN_DOUBLE_TAP_GAP_MS) {
            _waitingForSecondTap = false;
            postEvent(NAV_NEXT);
        }
    }
    else if (cfg.inputMode == INPUT_MODE_MULTI) {
        auto checkBtn = [&](int8_t pin, bool& stable, NavEvent ev) {
            if (pin < 0) return;
            bool r = digitalRead(pin);
            if (r != stable) {
                if ((now - _lastMultiChange) > BTN_DEBOUNCE_MS) {
                    stable = r;
                    _lastMultiChange = now;
                    if (stable == LOW) postEvent(ev);
                }
            }
        };

        checkBtn(cfg.pinBtnPrev, _stablePrev, NAV_PREVIOUS);
        checkBtn(cfg.pinBtnNext, _stableNext, NAV_NEXT);
        checkBtn(cfg.pinBtnSelect, _stableSelect, NAV_SELECT);
        checkBtn(cfg.pinBtnBack, _stableBack, NAV_BACK);
    }
}
