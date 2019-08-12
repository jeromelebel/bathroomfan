/* DHT library 
 *
 * MIT license
 * written by Adafruit Industries
 * modified for Spark Core by RussGrue
 * */

#ifndef DHT_H
#define DHT_H

#include <Particle.h>

// how many timing transitions we need to keep track of. 2 * number bits + extra
#define MAXTIMINGS 85

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

class DHT {
private:
  uint8_t data[6];
  uint8_t _pin, _type, _count;
  unsigned long _lastreadtime;
  bool isDataValid() const { return data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF); };

public:
  DHT(uint8_t pin, uint8_t type, uint8_t count=6);

  void begin();
  bool read();
  double getHumidity() const;
  double getTemperature() const;
  double getDewPoint() const;
};

#endif
