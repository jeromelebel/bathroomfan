#include "MQTTController.h"

#include <Particle.h>

#include "MQTT.h"

#define MQTT_UPDATE_DELAY         (30 * 1000)
#define MQTT_AUTO_CONNECT_DELAY   (15 * 60 * 1000)

MQTTController::MQTTController() : _mqtt(NULL), _lastUpdate(0), _lastConnectionFailureDate(0) {}

MQTTController::~MQTTController() {}

void MQTTController::begin(const char *server, uint16_t port) {
  _lastConnectionFailureDate = millis() - MQTT_AUTO_CONNECT_DELAY;
  _mqtt = new MQTT();
  _mqtt->setBroker(server, port);
}

void MQTTController::loop() {
  if (!_mqtt) {
    return;
  }
  if (!_mqtt->isConnected() && ((unsigned long)(millis() - _lastConnectionFailureDate) > MQTT_AUTO_CONNECT_DELAY)) {
    unsigned long startConnectTime = millis();
    if (!_mqtt->connect("vmc")) {
      _lastConnectionFailureDate = millis();
    }
    return;
  }
  _mqtt->loop();
  if ((unsigned long)(millis() - _lastUpdate) <= MQTT_UPDATE_DELAY) {
    return;
  }
  _lastUpdate = millis();
  std::vector<CallbackTopicPair*>::iterator it = _callbackTopicPairs.begin();
  while (it != _callbackTopicPairs.end()) {
    String value = (*it)->callback((*it)->topic);
    _mqtt->publish((*it)->topic, value, true);
    ++it;
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
