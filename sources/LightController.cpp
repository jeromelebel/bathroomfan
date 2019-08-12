#include "LightController.h"

#include <Particle.h>

#define LDR_PIN A0

LightController::LightController() :
        _light(0) {
}

LightController::~LightController() {
}

void LightController::begin() {
    pinMode(LDR_PIN, INPUT);
    Particle.variable("light", _light);
}

void LightController::loop() {
    _light = analogRead(LDR_PIN) / 4095.0;
}
