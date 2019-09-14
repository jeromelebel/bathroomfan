#include <Particle.h>
#include <vector>

class MQTT;

class MQTTController {
public:
  typedef String (*TopicCallback)(String topic);

  MQTTController();
  ~MQTTController();

  void begin(const char *server, uint16_t port);
  void loop();

  void addTopic(String topic, TopicCallback callback);

private:
  class CallbackTopicPair {
  public:
    String topic;
    TopicCallback callback;
  };

  std::vector<CallbackTopicPair*>::iterator getPairForTopic(String topic);

  std::vector<CallbackTopicPair *> _callbackTopicPairs;
  MQTT *_mqtt;
  unsigned long _lastUpdate;
};
