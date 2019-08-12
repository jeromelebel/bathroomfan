#ifndef LightController_h
#define LightController_h

class LightController {
public:
  LightController();
  ~LightController();

  void begin();
  void loop();
  double getLight() const { return _light; };

private:
  double _light;
};

#endif  // LightController_h
