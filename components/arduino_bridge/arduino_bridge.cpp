#include <Arduino.h>
#include "arduino_bridge.h"

namespace {
constexpr int led_pin = 2;
bool led_is_on = false;
}

extern "C" void arduino_bridge_init(void) {
    initArduino();
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);
    delay(500);
}

extern "C" void arduino_bridge_step(void) {
    led_is_on = !led_is_on;
    digitalWrite(led_pin, led_is_on ? HIGH : LOW);
    delay(500);
}
