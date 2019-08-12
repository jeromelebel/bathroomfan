/* DHT library 
 *
 * MIT license
 * written by Adafruit Industries
 * modified for Spark Core by RussGrue
 * */

#include "DHT.h"

#include <math.h>

DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
  _pin = pin;
  _type = type;
  _count = count;
}

void DHT::begin(void) {
// set up the pins!
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);
  _lastreadtime = 0;
}

double DHT::getTemperature() const {
  double f = NAN;

  if (isDataValid()) {
    switch (_type) {
      case DHT11:
        f = data[2];
      case DHT22:
      case DHT21:
        f = data[2] & 0x7F;
        f *= 256;
        f += data[3];
        f /= 10;
        if (data[2] & 0x80)
          f *= -1;
    }
  }
  return f;
}

double DHT::getHumidity() const {
  double f;

  if (isDataValid()) {
    switch (_type) {
      case DHT11:
        f = data[0];
        return f;
      case DHT22:
      case DHT21:
        f = data[0];
        f *= 256;
        f += data[1];
        f /= 10;
        return f;
    }
  }
  return NAN;
}

double DHT::getDewPoint() const {
    double tempCelcius = getTemperature();
    double percentHumidity = getHumidity();
  double a = 17.271;
  double b = 237.7;
  double tC = (a * (double) tempCelcius) / (b + (double) tempCelcius) + log( (double) percentHumidity / 100);
  double Td = (b * tC) / (a - tC);
  return Td;
}


boolean DHT::read() {
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  unsigned long currenttime;

// Check if sensor was read less than two seconds ago and return early
// to use last reading.
  currenttime = millis();
  if (isDataValid() && ((currenttime - _lastreadtime) < 1000)) {
    return true; // return last correct measurement
  }
  _lastreadtime = currenttime;

  data[0] = data[1] = data[2] = data [3] = data[4] = data[5] = 0xFF;
  
// pull the pin high and wait 250 milliseconds
  digitalWrite(_pin, HIGH);
  delay(250);

// now pull it low for ~20 milliseconds
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(20);
  noInterrupts();
  digitalWrite(_pin, HIGH);
  delayMicroseconds(40);
  pinMode(_pin, INPUT);

// read in timings
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (digitalRead(_pin) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = digitalRead(_pin);

    if (counter == 255) break;

// ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
// shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > _count)
        data[j/8] |= 1;
      j++;
    }
  }

  interrupts();

// check we read 40 bits and that the checksum matches
  return (j >= 40) && isDataValid();
}
