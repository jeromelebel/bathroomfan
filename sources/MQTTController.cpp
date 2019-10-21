#include "MQTTController.h"

#include <Particle.h>

#include "MQTT.h"

#define MQTT_UPDATE_DELAY    (30 * 1000)

MQTTController::MQTTController() : _mqtt(NULL), _lastUpdate(0), _autoconnect(true), _connectTime(0) {}

MQTTController::~MQTTController() {}

void MQTTController::begin(const char *server, uint16_t port) {
  _mqtt = new MQTT();
  _mqtt->setBroker(server, port);
  Particle.variable("mqtt", _connectTime);
}

void MQTTController::loop() {
  if (_mqtt) {
    if (!_mqtt->isConnected() && _autoconnect) {
      unsigned long long startConnectTime = millis();
      _mqtt->connect("vmc");
      _connectTime = (unsigned long long)(millis() - startConnectTime);
      _autoconnect = _mqtt->isConnected() && (_connectTime < 500);
      return;
    }
    _mqtt->loop();
    if ((unsigned long)(millis() - _lastUpdate) > MQTT_UPDATE_DELAY) {
      _lastUpdate = millis();
      std::vector<CallbackTopicPair*>::iterator it = _callbackTopicPairs.begin();
      while (it != _callbackTopicPairs.end()) {
        String value = (*it)->callback((*it)->topic);
        _mqtt->publish((*it)->topic, value, true);
        ++it;
      }
    }
  }
}

void MQTTController::addTopic(String topic, TopicCallback callback) {
  std::vector<CallbackTopicPair*>::iterator it = getPairForTopic(topic);
  if (it != _callbackTopicPairs.end()) {
    if (callback) {
      (*it)->callback = callback;
    } else {
      CallbackTopicPair* pair = *it;
      _callbackTopicPairs.erase(it);
      delete pair;
    }
    return;
  }
  if (!callback) {
    return;
  }
  CallbackTopicPair *pair = new CallbackTopicPair();
  pair->topic = topic;
  pair->callback = callback;
  _callbackTopicPairs.push_back(pair);
  return;
}

std::vector<MQTTController::CallbackTopicPair*>::iterator MQTTController::getPairForTopic(String topic) {
  std::vector<CallbackTopicPair*>::iterator it = _callbackTopicPairs.begin();
  while (it != _callbackTopicPairs.end()) {
    if ((*it)->topic == topic) {
      break;;
    }
    ++it;
  }
  return it;
}
