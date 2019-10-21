#ifndef PIRController_h
#define PIRController_h

class PIRController {
public:
  PIRController();
  ~PIRController();

  void begin();
  void loop();
  bool isHumanPresent() const;
  unsigned long getLastHumanDuration() const;

private:
  bool _pir;
  unsigned long _lastHumanDate;
};

#endif  // PIRController_h
