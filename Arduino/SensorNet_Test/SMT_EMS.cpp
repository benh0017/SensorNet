#include "SMT_EMS.h"
#include <math.h>

SMT_EMS::SMT_EMS(int moisturePin, int thermistorPin)
  : moistureAnalogPin(moisturePin), thermistorAnalogPin(thermistorPin) {}

void SMT_EMS::initialize() {
  pinMode(moistureAnalogPin, INPUT);
  pinMode(thermistorAnalogPin, INPUT);
}

float SMT_EMS::readResistance() {
  int raw = analogRead(moistureAnalogPin);
  float voltage = raw * (VREF / 1023.0);
  float resistance = fixedResistor * ((VREF - voltage) / voltage);
  return resistance;
}

float SMT_EMS::readMoisture() {
  float resistance = readResistance();          // Ohms
  float temperature = readTemperature();        // °C
  if (isinf(resistance)) return 0.0f;
  float R = resistance / 1e6;                   // Convert to megaohms
  float x = temperature;                        // rename to X to match datasheet equation
  // Moisture content equation
  float numerator = R + (0.567 - 0.0260 * x + 0.000052 * x * x);
  float denominator = 0.881 * pow(1.0056, x);
  float mc = (numerator / denominator - B) / A;
  return constrain(mc, 0.0f, 100.0f);
}

float SMT_EMS::readThermistorResistance() {
  int raw = analogRead(thermistorAnalogPin);
  float voltage = raw * (VREF / 1023.0);
  float resistance = fixedResistor * (voltage / (VREF - voltage));
  return resistance;
}

float SMT_EMS::readTemperature() {
  float R = readThermistorResistance();
  float tempK = 1.0 / ((1.0 / T0) + (1.0 / BETA) * log(R / R0));
  float tempC = tempK - 273.15;
  return tempC;
}
