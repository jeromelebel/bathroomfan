#include <Particle.h>

#include "DHT.h"

#include "DHTController.h"
#include "FanController.h"
#include "LEDController.h"
#include "LightController.h"
#include "PIRController.h"

#define ShowTemperatureDelay   5000

DHTController dhtController;
FanController fanController;
LEDController ledController;
LightController lightController;
PIRController pirController;

int fanSpeedOverride = -1;
int counter = 0;
int loopDuration = 0;
String startTime;

typedef struct {
  int speed;
  int humidity;
} HumidityFanSpeed;

HumidityFanSpeed humidityFanSpeed[] = {
  {  20,  40 }, //  20% fan speed, at  34% humidity.
  {  40,  50 }, //  40% fan speed, at  50% humidity.
  { 100,  80 }, // 100% fan speed, at 80% humidity.
  { 100, 100 }, // 100% fan speed, at 100% humidity.
};
const size_t humidityFanSpeedCount = sizeof(humidityFanSpeed) / sizeof(*humidityFanSpeed);

void showTemperatureNotification(double temperature) {
  LEDController::PixelColors pixelColors = LEDController::PixelColors::black();
  uint32_t color = 0x000100;
  if (temperature >= 20.5) {
    pixelColors = LEDController::PixelColors::percentageValue(temperature, 19.5, 28.5, color);
  } else if (temperature < 19.5) {
    pixelColors = LEDController::PixelColors::percentageValue(temperature, 20.5, 11.5, color);
  } else {
    pixelColors = LEDController::PixelColors::values(0b10000001, color);
  }
  LEDController::Notification notification(pixelColors, ShowTemperatureDelay);
  ledController.addNotification(notification);
}

void setFanSpeedForHumidity(double humidity) {
  size_t index = 0;
  for (; index < humidityFanSpeedCount; index++) {
    if (humidity <= humidityFanSpeed[index].humidity) {
      break;
    }
  }
  if (index == 0) {
    setFanSpeed(humidityFanSpeed[index].speed);
    return;
  }
  double previousLevelHumidity = humidityFanSpeed[index - 1].humidity;
  double humidityDelta = humidityFanSpeed[index].humidity - previousLevelHumidity;
  int previousLevelFanSpeed = humidityFanSpeed[index - 1].speed;
  int fanSpeedDelta = humidityFanSpeed[index].speed - previousLevelFanSpeed;
  int fanSpeed = (humidity - previousLevelHumidity) / humidityDelta * fanSpeedDelta + previousLevelFanSpeed;
  if (fanController.getFanSpeed() != fanSpeed) {
    setFanSpeed(fanSpeed);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.setHostname("VMC");
  startTime = Time.timeStr();

  dhtController.begin();
  ledController.begin();
  lightController.begin();
  fanController.begin();
  pirController.begin();
  
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
  showTemperatureNotification(dhtController.getTemperature());
}

void loop() {
  unsigned long start = micros();
  dhtController.loop();
  fanController.loop();
  lightController.loop();
  ledController.loop();
  pirController.loop();
  int humidity = dhtController.getHumidity();
  setFanSpeedForHumidity(humidity);
  if (pirController.isHumanPresent()) {
    ledController.setPixels(LEDController::PixelColors::percentageValue(dhtController.getHumidity(), 10, 100, 0x010000));
    if (!ledController.isOn()) {
        showTemperatureNotification(dhtController.getTemperature());
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
  if (fanController.getFanSpeed() != value && !ledController.hasNotification() && ledController.isOn()) {
    LEDController::PixelColors color = LEDController::PixelColors::percentageValue(value, 10, 100, 0x010101);
    ledController.addNotification(LEDController::Notification(color, 500));
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

int myTestMethod(String stringValue) {
  double value = stringValue.toFloat();
  showTemperatureNotification(value);
  return pirController.isHumanPresent();
}

int myTestMethod1(String stringValue) {
  double value = stringValue.toFloat();
  ledController.addNotification(LEDController::Notification(LEDController::PixelColors::percentageValue(value, 10, 100, 0x000101), 1000));
  return counter;
}

