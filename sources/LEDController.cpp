#include "LEDController.h"

#include <neopixel.h>
#include <Particle.h>
#include <vector>

#define PIXEL_PIN D1
#define PIXEL_TYPE WS2812B

// static
LEDController::PixelColors LEDController::PixelColors::percentage(float percentage, uint32_t color) {
  std::vector<uint32_t> colors;
  bool allOff = false;
  for (int i = 0; i < PIXEL_COUNT; i++) {
    uint32_t pixelColor = 0x0;
    if (percentage > (float)(i + 1) / (float)(PIXEL_COUNT + 1)) {
      pixelColor = color;
    } else if (i == 0) {
      allOff = true;
    } else if (allOff && i == PIXEL_COUNT - 1) {
      pixelColor = color;
    }
    colors.push_back(pixelColor);
  }
  return LEDController::PixelColors(colors);
}

// static
LEDController::PixelColors LEDController::PixelColors::percentageValue(float value, float minimum, float maximum, uint32_t color) {
  return LEDController::PixelColors::percentage((value - minimum) / (maximum - minimum), color);
}

// static
LEDController::PixelColors LEDController::PixelColors::values(int pixels, uint32_t color) {
  std::vector<uint32_t> colors;
  for (int i = 0; i < PIXEL_COUNT; i++) {
    colors.push_back((pixels & 1) ? color : 0x0);
    pixels = pixels >> 1;
  }
  return LEDController::PixelColors(colors);
}

// static
LEDController::PixelColors LEDController::PixelColors::black() {
  std::vector<uint32_t> colors;
  for (int i = 0; i < PIXEL_COUNT; i++) {
    colors.push_back(0x0);
  }
  return LEDController::PixelColors(colors);
}

LEDController::PixelColors::PixelColors(std::vector<uint32_t> colors) :
    _pixelColors(colors) {
}

LEDController::PixelColors::~PixelColors() {
}

LEDController::Notification::Notification(LEDController::PixelColors colors, long duration) :
    _duration(duration),
    _startDate(0),
    _started(false),
    _pixelColors(colors) {
}

LEDController::Notification::~Notification() {
}

LEDController::LEDController() :
    _strip(new Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE)),
    _defaultPixelColors(LEDController::PixelColors::black()),
    _on(false)  {
}

LEDController::~LEDController() {
  delete _strip;
}

void LEDController::begin() {
  _strip->begin();
  _strip->show();
}

void LEDController::loop() {
  bool needUpdate = false;
  while (!_notifications.empty()) {
    Notification &currentNotification = _notifications[0];
    if (!currentNotification.isStarted()) {
      currentNotification.start();
      needUpdate = true;
      break;
    } else if (currentNotification.hasExpired()) {
      removeCurrentNotification();
      needUpdate = true;
    }
  }
  if (needUpdate) {
    updatePixelColors();
  }
}

void LEDController::setOn(bool value) {
  if (value == _on) {
    return;
  }
  _on = value;
  updatePixelColors();
}

void LEDController::setPixels(const PixelColors &pixelColors) {
  if (_on && !hasNotification()) {
    applyPixelColors(pixelColors);
  }
  _defaultPixelColors = pixelColors;
}

void LEDController::updatePixelColors() {
  if (hasNotification()) {
    applyPixelColors(_notifications[0].pixelColors());
  } else if (_on) {
    applyPixelColors(_defaultPixelColors);
  } else {
    applyPixelColors(LEDController::PixelColors::black());
  }
}

void LEDController::applyPixelColors(const PixelColors &pixelColors) {
  for (int i = 0; i < PIXEL_COUNT; ++i) {
    _strip->setPixelColor(PIXEL_COUNT - 1 - i, pixelColors[i]);
  }
  _strip->show();
}

void LEDController::addNotification(LEDController::Notification notification) {
  _notifications.push_back(notification);
}

void LEDController::startNextNotification() {
}

void LEDController::removeCurrentNotification() {
  if (_notifications.empty()) {
    return;
  }
  _notifications.erase(_notifications.begin());
}
