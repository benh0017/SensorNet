#ifndef SMT_EMS_H
#define SMT_EMS_H

#include <Arduino.h>

class SMT_EMS {
public:
  SMT_EMS(int moisturePin, int thermistorPin);
  void initialize();
  float readMoisture();             // %MC
  float readResistance();           // Ohms (moisture sensor)
  float readTemperature();          // °C
  float readThermistorResistance(); // Ohms

private:
  const int moistureAnalogPin;
  const int thermistorAnalogPin;

  // Shared
  static constexpr float VREF = 5.0;
  static constexpr int fixedResistor = 5100;

  // Moisture circuit
  static constexpr float A = 0.822;
  static constexpr float B = 0.202;

  // Thermistor circuit
  static constexpr float BETA = 4150.0;       // From datasheet
  static constexpr float R0 = 100000.0;       // 100kΩ @ 25°C
  static constexpr float T0 = 298.15;         // 25°C in Kelvin
};

#endif
