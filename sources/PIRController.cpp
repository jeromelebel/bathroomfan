#include "PIRController.h"

#include <Particle.h>

#define PIR_PIN D4
#define LED_PIN D7
#define HUMAN_DURATION ((unsigned long)(1000 * 60 * 5))

PIRController::PIRController() :
    _pir(false),
    _lastHumanDate(-HUMAN_DURATION) {
}

PIRController::~PIRController() {
}

void PIRController::begin() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void PIRController::loop() {
  _pir = digitalRead(PIR_PIN);
  if (_pir) {
    _lastHumanDate = millis();
    digitalWrite(D7, HIGH);
  } else {
    digitalWrite(D7, LOW);
  }
}

bool PIRController::isHumanPresent() const {
  return getLastHumanDuration() < HUMAN_DURATION;
}

unsigned long PIRController::getLastHumanDuration() const {
  return millis() - _lastHumanDate;
};
