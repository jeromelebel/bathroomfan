#include "NightModeController.h"

#include <Particle.h>

#include "PIRController.h"

#define HumanDurationBeforeNightMode  (60 * 3)

NightModeController::NightModeController() :
    _pirController(NULL) {
}

NightModeController::~NightModeController() {
}

void NightModeController::begin() {
}

void NightModeController::loop() {
}

bool NightModeController::isNight() const {
  if (Time.hour() > 8 && Time.hour() < 23) {
    return false;
  }
  return _pirController == NULL || _pirController->getLastHumainDuration() > HumanDurationBeforeNightMode;
}
