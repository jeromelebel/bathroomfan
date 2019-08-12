#ifndef FanController_h
#define FanController_h

class FanController {
public:
  FanController();
  ~FanController();
  
  void begin();
  void loop();
  void setFanSpeed(double value);
  double getFanSpeed() { return _fanSpeed; };
  int getRPM() const { return _rpm; };

private:
  void readRPM();
  
  double _fanSpeed;
  int _rpm;
};

#endif  // Fan_h