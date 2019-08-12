#include "FanController.h"

#include <Particle.h>

#define PWM_PIN D0
#define RPM_PIN A3
#define PWM_FREQ 25000

static volatile unsigned long lastRPMChange = 0;
static volatile unsigned long rpmDuration = 0;

void rpmInterrupt() {
  unsigned long currentMicros = micros();
  rpmDuration = currentMicros - lastRPMChange;
  lastRPMChange = currentMicros;
}

FanController::FanController() {
}

FanController::~FanController() {
}

void FanController::begin() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(RPM_PIN, INPUT);
  attachInterrupt(RPM_PIN, rpmInterrupt, CHANGE);
  setFanSpeed(0);
  Particle.variable("rpm", _rpm);
  Particle.variable("fanspeed", _fanSpeed);
}

void FanController::loop() {
  readRPM();
}

void FanController::setFanSpeed(int value) {
  if (value < 0) {
    value = 0;
  } else if (value > 100) {
    value = 100;
  }
  _fanSpeed = value;
  analogWrite(PWM_PIN, _fanSpeed / 100. * 255., PWM_FREQ);
}

void FanController::readRPM() {
  if ((unsigned long)(micros() - lastRPMChange) > 200000) {
    _rpm = 0;
  } else {
    _rpm = 1000000. / rpmDuration * 15.;
  }
}
