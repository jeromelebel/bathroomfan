#include <Particle.h>
#include <vector>

class PreferenceController {
public:
  PreferenceController();
  ~PreferenceController();

  bool load();
  void save();

  bool hasValueForKey(String key);
  String valueForKey(String key);
  bool setValueForKey(String value, String key);
  bool removeValueForKey(String key);
  int count() const { return _keyValues.size(); };
  String keyAtIndex(int index) const;
  String valueAtIndex(int index) const;


protected:
  class KeyValue {
  public:
    String key;
    String value;
  };

  std::vector<KeyValue*>::iterator getKeyValueForKey(String key);

  std::vector<KeyValue *> _keyValues;
};
