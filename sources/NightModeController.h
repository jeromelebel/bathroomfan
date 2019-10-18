#ifndef NightModeController_h
#define NightModeController_h

class PIRController;

class NightModeController {
public:
  NightModeController();
  ~NightModeController();

  void setPIRController(PIRController *pirController) { _pirController = pirController; };
  void begin();
  void loop();
  bool isNight() const;

private:
  PIRController *_pirController;
};

#endif  // NightModeController_h
