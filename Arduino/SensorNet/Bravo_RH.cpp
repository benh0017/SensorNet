#include "Bravo_RH.h"

#define CMD_MEASURE_HIGH_PRECISION 0xFD

Bravo_RH::Bravo_RH(uint8_t i2cAddress) : address(i2cAddress) {}

void Bravo_RH::initialize() {
  Wire.begin();
}

bool Bravo_RH::readSensorData(uint16_t &rawTemp, uint16_t &rawHumidity) {
  Wire.beginTransmission(address);
  Wire.write(CMD_MEASURE_HIGH_PRECISION);
  if (Wire.endTransmission() != 0) return false;
  delay(10);

  Wire.requestFrom(address, (uint8_t)6);
  if (Wire.available() < 6) return false;

  rawTemp = (Wire.read() << 8) | Wire.read();
  Wire.read(); // skip CRC
  rawHumidity = (Wire.read() << 8) | Wire.read();
  Wire.read(); // skip CRC

  return true;
}

float Bravo_RH::readTemperature() {
  uint16_t rawTemp, rawHumidity;
  if (!readSensorData(rawTemp, rawHumidity)) return NAN;
  return -45.0f + 175.0f * ((float)rawTemp / 65535.0f);
}

float Bravo_RH::readHumidity() {
  uint16_t rawTemp, rawHumidity;
  if (!readSensorData(rawTemp, rawHumidity)) return NAN;
  float rh = -6.0f + 125.0f * ((float)rawHumidity / 65535.0f);
  if (rh < 0.0f) rh = 0.0f;
  if (rh > 100.0f) rh = 100.0f;
  return rh;
}
