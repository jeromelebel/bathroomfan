#include "DHTController.h"

#include "DHT.h"

#define DHTPIN D5
#define QUERY_PERIOD 1000

DHTController::DHTController() :
    _dht(new DHT(DHTPIN, DHT22)),
    _successRead(false),
    _lastQuery(0) {
}

DHTController::~DHTController() {
  delete _dht;
}

void DHTController::begin() {
  _lastQuery = millis() - QUERY_PERIOD - 1;
  _dht->begin();
  Particle.variable("humi", _humidity);
  Particle.variable("temp", _temperature);
}

void DHTController::loop() {
  unsigned long current = millis();
  if ((unsigned long)(current - _lastQuery) > QUERY_PERIOD) {
    _successRead = _dht->read();
    if (!_successRead) {
      return;
    }
    _humidity = _dht->getHumidity();
    _temperature = _dht->getTemperature();
  }
}
