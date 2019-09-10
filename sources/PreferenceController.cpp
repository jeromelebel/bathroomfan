#include "PreferenceController.h"

#include <Particle.h>

#define StartTag    "Start"

namespace {

void readBuffer(int address, int size, char *buffer) {
  for (int ii = 0; ii < size; ii++) {
    buffer[ii] = EEPROM.read(address + ii);
  }
}

bool testTag(const char *tag, int *address) {
  int tagLength = strlen(tag);
  char *buffer = (char *)malloc(tagLength + 1);
  readBuffer(*address, tagLength, buffer);
  buffer[tagLength] = 0;
  bool result = strncmp(tag, buffer, tagLength) == 0;
  if (result) {
    *address = *address + tagLength;
  }
  free(buffer);
  return result;
}

uint8_t readUInt8(int *address) {
  uint8_t result = EEPROM.read(*address);
  *address = *address + 1;
  return result;
}

void readBuffer(int *address, char *buffer, int length) {
  for (int ii = 0; ii < length; ++ii) {
    buffer[ii] = EEPROM.read(*address);
    *address = *address + 1;
  }
}

String readString(int *address) {
  String result;
  int length = readUInt8(address);
  char *buffer = (char *)malloc(length + 1);
  readBuffer(address, buffer, length);
  buffer[length] = 0;
  result = buffer;
  free((void *)buffer);
  return result;
}

void writeUInt8(int *address, uint8_t character) {
  EEPROM.write(*address, character);
  *address = *address + 1;
}

void writeBuffer(int *address, const char *buffer, int length) {
  for (int ii = 0; ii < length; ++ii) {
    writeUInt8(address, (uint8_t)buffer[ii]);
  }
}

void writeString(int *address, String string) {
  int length = string.length();
  if (length > 255) {
    length = 255;
  }
  writeUInt8(address, (uint8_t)length);
  writeBuffer(address, string.c_str(), length);
}

}  // namespace

PreferenceController::PreferenceController() {
}

PreferenceController::~PreferenceController() {
}

bool PreferenceController::load() {
  int address = 0;
  if (!testTag(StartTag, &address)) {
    return false;
  }
  int keyLength = readUInt8(&address);
  for (int ii = 0; ii < keyLength; ++ii) {
    String key = readString(&address);
    String value = readString(&address);
    setValueForKey(value, key);
  }
  return true;
}

void PreferenceController::save() {
  int address = 0;
  writeBuffer(&address, StartTag, strlen(StartTag));
  writeUInt8(&address, _keyValues.size());
  for (auto it = _keyValues.begin(); it != _keyValues.end(); ++it) {
    writeString(&address, (*it)->key);
    writeString(&address, (*it)->value);
  }
}

bool PreferenceController::hasValueForKey(String key) {
  std::vector<KeyValue*>::iterator it = getKeyValueForKey(key);
  return it != _keyValues.end();
}

String PreferenceController::valueForKey(String key) {
  std::vector<KeyValue*>::iterator it = getKeyValueForKey(key);
  if (it != _keyValues.end()) {
    return (*it)->value;
  }
  return String();
}

bool PreferenceController::setValueForKey(String value, String key) {
  if (key.length() > 255 || value.length() > 255) {
    return false;
  }
  std::vector<KeyValue*>::iterator it = getKeyValueForKey(key);
  if (it != _keyValues.end()) {
    (*it)->value = value;
    return true;
  }
  if (_keyValues.size() == 255) {
    return false;
  }
  KeyValue *keyValue = new KeyValue();
  keyValue->key = key;
  keyValue->value = value;
  _keyValues.push_back(keyValue);
  return true;
}

bool PreferenceController::removeValueForKey(String key) {
  std::vector<KeyValue*>::iterator it = getKeyValueForKey(key);
  if (it == _keyValues.end()) {
    return false;
  }
  KeyValue *keyValue = *it;
  _keyValues.erase(it);
  delete keyValue;
  return true;
}

String PreferenceController::keyAtIndex(int index) const {
  if (index >= (int)_keyValues.size()) {
      return String();
  }
  const KeyValue *keyValue = _keyValues.at(index);
  return keyValue->key;
}

String PreferenceController::valueAtIndex(int index) const {
  if (index >= (int)_keyValues.size()) {
      return String();
  }
  const KeyValue *keyValue = _keyValues.at(index);
  return keyValue->value;
}


std::vector<PreferenceController::KeyValue*>::iterator PreferenceController::getKeyValueForKey(String key) {
  std::vector<KeyValue*>::iterator it = _keyValues.begin();
  while (it != _keyValues.end()) {
    if ((*it)->key == key) {
      break;;
    }
    ++it;
  }
  return it;
}
