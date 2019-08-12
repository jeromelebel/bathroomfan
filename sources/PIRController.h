#ifndef PIRController_h
#define PIRController_h

class PIRController {
public:
  PIRController();
  ~PIRController();

  void begin();
  void loop();
  bool isHumanPresent() const;
  unsigned long getLastHumainDuration() const;

private:
  bool _pir;
  unsigned long _lastHumainDate;
};

#endif  // PIRController_h
