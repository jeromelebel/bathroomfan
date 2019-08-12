#ifndef FanController_h
#define FanController_h

class FanController {
public:
  FanController();
  ~FanController();
  
  void begin();
  void loop();
  void setFanSpeed(int value);
  int getFanSpeed() { return _fanSpeed; };
  int getRPM() const { return _rpm; };

private:
  void readRPM();
  
  int _fanSpeed;
  int _rpm;
};

#endif  // Fan_h