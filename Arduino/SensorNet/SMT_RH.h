#ifndef SMT_RH_H
#define SMT_RH_H

#include <Arduino.h>

class SMT_RH {
public:
  SMT_RH(int rhPin, int thermistorPin);
  void initialize();
  float readHumidity();     // %RH
  float readTemperature();  // °C (from thermistor)

private:
  const int rhAnalogPin;
  const int thermistorAnalogPin;

  // Shared constants
  static constexpr float VREF = 5.0;

  // Thermistor constants
  static constexpr int fixedResistor = 10000;   // 10k fixed resistor
  static constexpr float BETA = 4150.0;         // Beta Coefficient from data sheet 
  static constexpr float R0 = 100000.0;         // 100kΩ at 25°C
  static constexpr float T0 = 298.15;           // 25°C in Kelvin

  float readThermistorResistance();
};

#endif
