#ifndef BRAVO_RH_H
#define BRAVO_RH_H

#include <Arduino.h>
#include <Wire.h>

class Bravo_RH {
public:
  Bravo_RH(uint8_t i2cAddress);
  void initialize();
  float readHumidity();
  float readTemperature();

private:
  const uint8_t address;
  bool readSensorData(uint16_t &rawTemp, uint16_t &rawHumidity);
};

#endif