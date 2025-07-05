#include "SMT_RH.h"
#include <math.h>

SMT_RH::SMT_RH(int rhPin, int thermistorPin)
  : rhAnalogPin(rhPin), thermistorAnalogPin(thermistorPin) {}

void SMT_RH::initialize() {
  pinMode(rhAnalogPin, INPUT);
  pinMode(thermistorAnalogPin, INPUT);
}

float SMT_RH::readHumidity() {
  int raw = analogRead(rhAnalogPin);
  float voltage = raw * (VREF / 1023.0);

  // Datasheet linear approximation
  float rh = ((voltage / VREF) - 0.16) / 0.0062;

  // Clamp range
  return constrain(rh, 0.0f, 100.0f);
}

float SMT_RH::readThermistorResistance() {
  int raw = analogRead(thermistorAnalogPin);
  float voltage = raw * (VREF / 1023.0);
  if (voltage <= 0.01 || voltage >= VREF) return NAN;
  return fixedResistor * (voltage / (VREF - voltage));
}

float SMT_RH::readTemperature() {
  float R = readThermistorResistance();
  if (isnan(R)) return NAN;
  float tempK = 1.0 / ((1.0 / T0) + (1.0 / BETA) * log(R / R0));
  return tempK - 273.15;
}
