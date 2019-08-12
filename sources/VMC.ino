#include <Particle.h>

#include "DHT.h"

#include "DHTController.h"
#include "FanController.h"
#include "LEDController.h"
#include "LightController.h"
#include "PIRController.h"

DHTController dhtController;
FanController fanController;
LEDController ledController;
LightController lightController;
PIRController pirController;

int fanSpeedOverride = -1;
int counter = 0;
int loopDuration = 0;
String startTime;

void setup() {
  Serial.begin(115200);
  WiFi.setHostname("VMC");
  startTime = Time.timeStr();

  dhtController.begin();
  ledController.begin();
  lightController.begin();
  fanController.begin();
  pirController.begin();
  pinMode(D4, INPUT);
  
  Particle.function("fanspeed", setFanSpeedOverride);
  Particle.function("test", myTestMethod);
  Particle.function("test1", myTestMethod1);
  Particle.variable("loop", loopDuration);
  Particle.variable("start", startTime);

  ledController.setOn(true);
  int value = 0x0;
  for (int i = 0; i < PIXEL_COUNT; i++) {
    int currentPixel = 0b100000000;
    for (int j = 0; j < PIXEL_COUNT - i; j++) {
      currentPixel = currentPixel >> 1;
      ledController.setPixels(LEDController::PixelColors::values(value | currentPixel, 0x010101));
      delay(50);
    }
    value = value | currentPixel;
  }
  delay(500);
  dhtController.loop();
  ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(dhtController.getTemperature(), 17, 28, 0x000100), 1500));
}

void loop() {
  unsigned long start = micros();
  dhtController.loop();
  fanController.loop();
  lightController.loop();
  ledController.loop();
  pirController.loop();
  if (dhtController.getHumidity() < 50) {
    setFanSpeed(20);
  } else if (dhtController.getHumidity() < 51) {
  } else if (dhtController.getHumidity() < 65) {
    setFanSpeed(40);
  } else if (dhtController.getHumidity() < 66) {
  } else {
    setFanSpeed(75);
  }
  if (pirController.isHumanPresent()) {
    ledController.setPixels(LEDController::PixelColors::percentageValue(dhtController.getHumidity(), 9, 99, 0x010000));
    if (!ledController.isOn()) {
        ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(dhtController.getTemperature(), 17, 28, 0x000100), 1500));
        ledController.setOn(true);
    }
  } else {
    ledController.setOn(false);
  }
  loopDuration = micros() - start;
}

void setFanSpeed(int value) {
  if (fanSpeedOverride != -1) {
    return;
  }
  if (fanController.getFanSpeed() != value && !ledController.hasNotification()) {
    ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(value * 100, 9, 99, 0x010101), 500));
  }
  fanController.setFanSpeed(value);
}

int setFanSpeedOverride(String value) {
  fanSpeedOverride = value.toInt();
  if (fanSpeedOverride < 0) {
    fanSpeedOverride = -1;
    return fanSpeedOverride;
  }
  ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentage(fanSpeedOverride, 0x010101), 500));
  fanController.setFanSpeed(fanSpeedOverride);
  return fanSpeedOverride;
}

int myTestMethod(String value) {
  int valueToInt = value.toInt();
  ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(valueToInt, 9, 99, 0x000001), 1000));
  return pirController.isHumanPresent();
}

int myTestMethod1(String value) {
  int valueToInt = value.toInt();
  ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(valueToInt, 9, 99, 0x000101), 1000));
  return counter;
}

