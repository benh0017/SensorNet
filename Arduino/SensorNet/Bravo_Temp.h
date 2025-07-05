// Bravo_Temp.h
#ifndef BRAVO_TEMP_H
#define BRAVO_TEMP_H

#include <Arduino.h>

class Bravo_Temp {
public:
  Bravo_Temp(int pin);
  void initialize();
  float readTemperature();  // Celsius
  float readResistance();   // Ohms

private:
  const int thermistorPin;
  static constexpr float VREF = 5.0;          // Supply voltage
  static constexpr int fixedResistor = 10000; // 10k resistor in divider

  // Beta model parameters
  static constexpr float BETA = 3950.0;       // Beta coefficient
  static constexpr float R0 = 10000.0;        // Resistance at 25°C
  static constexpr float T0 = 298.15;         // 25°C in Kelvin

  float getAverageAnalogValue(int samples = 5, int delayMs = 10);
};

#endif