#ifndef DHTController_h
#define DHTController_h

class DHT;

class DHTController {
public:
  DHTController();
  ~DHTController();

  void begin();
  void loop();
  double getHumidity() const { return _humidity; };
  double getTemperature() const { return _temperature; };
  bool isSuccessRead() const { return _successRead; };

private:
  DHT *_dht;
  bool _successRead;
  unsigned long int _lastQuery;
  double _temperature;
  double _humidity;
};

#endif  // DHTController_h
