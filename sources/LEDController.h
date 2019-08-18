#ifndef LEDController_h
#define LEDController_h

#include <Particle.h>
#include <vector>

class Adafruit_NeoPixel;

#define PIXEL_COUNT 8

class LEDController {
public:
  class PixelColors {
  public:
    static PixelColors percentage(float percentage, uint32_t color);
    static PixelColors percentageValue(float value, float minimum, float maximum, uint32_t color);
    static PixelColors values(int pixels, uint32_t color);
    static PixelColors black();
    ~PixelColors();
    uint32_t operator[](int i) const { return _pixelColors[i]; };

  private:
    PixelColors(std::vector<uint32_t> colors);
    void setColor(int i, uint32_t color) { _pixelColors[i] = color; };
    std::vector<uint32_t> _pixelColors;
  };

  class Notification {
  public:
    Notification(PixelColors colors, long duration);
    ~Notification();
    bool isStarted() const { return _started; };
    void start() { _startDate = millis(); _started = true; };
    bool hasExpired() const { return _started && ((unsigned long)(millis() - _startDate) > _duration); };
    PixelColors pixelColors() const { return _pixelColors; };

  private:
    unsigned long _duration;
    unsigned long _startDate;
    bool _started;
    PixelColors _pixelColors;
  };

  LEDController();
  ~LEDController();

  void begin();
  void loop();
  void setOn(bool value);
  bool isOn() { return _on; };
  void setPixels(const PixelColors &pixelColors);
  void addNotification(Notification notification);
  bool hasNotification() { return !_notifications.empty(); };

private:
  void updatePixelColors();
  void applyPixelColors(const PixelColors &pixelColors);
  void startNextNotification();
  void removeCurrentNotification();
  
  Adafruit_NeoPixel *_strip;
  std::vector<Notification> _notifications;
  PixelColors _defaultPixelColors;
  bool _on;
};

#endif  // LEDS_h
