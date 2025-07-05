// SensorNet_TestModular.ino
#include <Wire.h>
#include "Bravo_Temp.h"
#include "Bravo_RH.h"
#include "SMT_RH.h"
#include "SMT_EMS.h"
#include "SMT_PMM.h"

#define NUM_BRAVO_TEMP 6
#define SAMPLES_PER_SENSOR 10
#define SAMPLE_DELAY_MS 50

// Bravo Temp sensors
Bravo_Temp bravoTempSensors[NUM_BRAVO_TEMP] = {
  Bravo_Temp(A0), Bravo_Temp(A1), Bravo_Temp(A2), Bravo_Temp(A3), Bravo_Temp(A4), Bravo_Temp(A5)
};

Bravo_RH bravoRH(0x44);
SMT_RH smtRH(A7, A6);
SMT_EMS smtEMS(A10, A11);
SMT_PMM smtPMM(A8, A9);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  for (int i = 0; i < NUM_BRAVO_TEMP; i++) bravoTempSensors[i].initialize();
  bravoRH.initialize();
  smtRH.initialize();
  smtEMS.initialize();
  smtPMM.initialize();

  Serial.println();
  Serial.println("READY. Call individual test functions in loop().");
}

// --- Modular Sensor Test Functions ---
void logBravoTemps() {
  Serial.print("BravoTemps: ");
  for (int i = 0; i < NUM_BRAVO_TEMP; i++) {
    float temp = bravoTempSensors[i].readTemperature();
    Serial.print(isnan(temp) ? "Err" : String(temp, 2));
    if (i < NUM_BRAVO_TEMP - 1) Serial.print(", ");
  }
  Serial.println();
}

void logBravoRH() {
  float temp = bravoRH.readTemperature();
  float rh = bravoRH.readHumidity();
  Serial.print("Bravo RH Temp: "); Serial.print(temp, 2);
  Serial.print(" C, Humidity: "); Serial.print(rh, 2); Serial.println(" %");
}

void logSMT_RH() {
  float temp = smtRH.readTemperature();
  float rh = smtRH.readHumidity();
  Serial.print("SMT RH Temp: "); Serial.print(temp, 2);
  Serial.print(" C, Humidity: "); Serial.print(rh, 2); Serial.println(" %");
}

void logSMT_EMS() {
  float mc = smtEMS.readMoisture();
  float temp = smtEMS.readTemperature();
  Serial.print("SMT EMS Moisture: "); Serial.print(mc, 2);
  Serial.print(" %, Temp: "); Serial.print(temp, 2); Serial.println(" C");
}

void logSMT_PMM() {
  float mc = smtPMM.readMoisture();
  float temp = smtPMM.readTemperature();
  Serial.print("SMT PMM Moisture: "); Serial.print(mc, 2);
  Serial.print(" %, Temp: "); Serial.print(temp, 2); Serial.println(" C");
}

void loop() {
  logBravoTemps();
  logBravoRH();
  logSMT_RH();
  logSMT_EMS();
  logSMT_PMM();

  delay(1000);
}
