#ifndef SMT_PMM_H
#define SMT_PMM_H

#include <Arduino.h>

class SMT_PMM {
public:
  SMT_PMM(int moisturePin, int thermistorPin);
  void initialize();
  float readMoisture();     // %MC
  float readResistance();   // Ohms (moisture sensor)
  float readTemperature();  // °C
  float readThermistorResistance();

private:
  const int moistureAnalogPin;
  const int thermistorAnalogPin;

  // Shared
  static constexpr float VREF = 5.0;
  static constexpr int fixedResistor = 5100;

  // Constants for moisture circuit
  static constexpr float A = 25.0;
  static constexpr float B = -5.0;

  // Constants for PMM thermistor
  static constexpr float BETA = 4150.0;
  static constexpr float R0 = 100000.0; // 100kΩ at 25°C
  static constexpr float T0 = 298.15;   // 25°C in Kelvin

};

#endif
