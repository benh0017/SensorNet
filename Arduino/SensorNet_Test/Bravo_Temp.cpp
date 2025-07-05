#include "Bravo_Temp.h"
#include <math.h>

Bravo_Temp::Bravo_Temp(int pin) : thermistorPin(pin) {}

void Bravo_Temp::initialize() {
  pinMode(thermistorPin, INPUT);
}

float Bravo_Temp::getAverageAnalogValue(int samples, int delayMs) {
  long total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(thermistorPin);
    delay(delayMs);
  }
  return total / (float)samples;
}

float Bravo_Temp::readResistance() {
  float analogValue = getAverageAnalogValue();
  float voltage = analogValue * (VREF / 1023.0);
  if (voltage <= 0.0) return NAN;
  return fixedResistor * (voltage / (VREF - voltage));
}

float Bravo_Temp::readTemperature() {
  float R = readResistance();
  if (isnan(R)) return NAN;
  float tempK = 1.0 / ((1.0 / T0) + (1.0 / BETA) * log(R / R0));
  return tempK - 273.15;
}