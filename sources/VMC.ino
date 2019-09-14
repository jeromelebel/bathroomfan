#include <Particle.h>

#include "DHT.h"

#include "DHTController.h"
#include "FanController.h"
#include "LEDController.h"
#include "LightController.h"
#include "MQTTController.h"
#include "PIRController.h"
#include "PreferenceController.h"

#define ShowTemperatureDelay   5000
#define MQTTServerPref         "mqtts"
#define MQTTPortPref           "mqttp"
#define TempMQTTTopic          "vmc/temp"
#define HumiMQTTTopic          "vmc/humi"
#define FanRPMMQTTTopic        "vmc/fan/rpm"
#define FanSpeedMQTTTopic      "vmc/fan/speed"

DHTController dhtController;
FanController fanController;
LEDController ledController;
LightController lightController;
PIRController pirController;
PreferenceController preferenceController;
MQTTController mqttController;

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

String valueForTopic(String topic) {
  if (topic == TempMQTTTopic) {
    return String(dhtController.getTemperature(), 1);
  } else if (topic == HumiMQTTTopic) {
    return String(dhtController.getHumidity(), 1);
  } else if (topic == FanRPMMQTTTopic) {
    return String(fanController.getRPM());
  } else if (topic == FanSpeedMQTTTopic) {
    return String(fanController.getFanSpeed());
  }
  return String();
}

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

  preferenceController.load();

  dhtController.begin();
  ledController.begin();
  lightController.begin();
  fanController.begin();
  pirController.begin();
  preferenceController.load();

  Particle.function("MQTTServer", setMQTTServer);
  Particle.function("MQTTPort", setMQTTPort);
  Particle.function("fanspeed", setFanSpeedOverride);
  Particle.function("test", myTestMethod);
  Particle.variable("loop", loopDuration);
  Particle.variable("start", startTime);

  String mqttServer = preferenceController.valueForKey(MQTTServerPref);
  String mqttPort = preferenceController.valueForKey(MQTTPortPref);
  Particle.publish("mqtt server", mqttServer, PRIVATE);
  Particle.publish("mqtt port", mqttPort, PRIVATE);

  welcomeAnimation();
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
  mqttController.loop();
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

void welcomeAnimation() {
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

int setMQTTServer(String server) {
  preferenceController.setValueForKey(server, MQTTServerPref);
  preferenceController.save();
  return 1;
}

int setMQTTPort(String port) {
  preferenceController.setValueForKey(port, MQTTPortPref);
  preferenceController.save();
  return 1;
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
  String mqttServer = preferenceController.valueForKey(MQTTServerPref);
  String mqttPort = preferenceController.valueForKey(MQTTPortPref);
  if (mqttServer != "" && mqttPort != "") {
    mqttController.begin(mqttServer, mqttPort.toInt());
    mqttController.addTopic(HumiMQTTTopic, valueForTopic);
    mqttController.addTopic(TempMQTTTopic, valueForTopic);
    mqttController.addTopic(FanRPMMQTTTopic, valueForTopic);
    mqttController.addTopic(FanSpeedMQTTTopic, valueForTopic);
  }
  return 1;
}


