#include "SMT_PMM.h"
#include <math.h>

SMT_PMM::SMT_PMM(int moisturePin, int thermistorPin)
  : moistureAnalogPin(moisturePin), thermistorAnalogPin(thermistorPin) {}

void SMT_PMM::initialize() {
  pinMode(moistureAnalogPin, INPUT);
  pinMode(thermistorAnalogPin, INPUT);
}

float SMT_PMM::readResistance() {
  int raw = analogRead(moistureAnalogPin);
  float voltage = raw * (VREF / 1023.0);
  if (voltage <= 0.0) return NAN;
  return fixedResistor * (voltage / (VREF - voltage));
}

float SMT_PMM::readMoisture() {
  float R = readResistance();
  if (isnan(R)) return NAN;
  float rMega = R / 1e6;
  float mc = A + B * log10(rMega);
  return constrain(mc, 0.0f, 100.0f);
}

float SMT_PMM::readThermistorResistance() {
  int raw = analogRead(thermistorAnalogPin);
  float voltage = raw * (VREF / 1023.0);
  if (voltage <= 0.0 || voltage >= VREF) return NAN;

  return fixedResistor * (voltage / (VREF - voltage));
}

float SMT_PMM::readTemperature() {
  float R = readThermistorResistance();
  if (isnan(R)) return NAN;
  float tempK = 1.0 / ((1.0 / T0) + (1.0 / BETA) * log(R / R0));
  return tempK - 273.15;
}
