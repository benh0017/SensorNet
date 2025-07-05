#include <Wire.h>
#include <SD.h>
#include "Bravo_Temp.h"
#include "Bravo_RH.h"
#include "SMT_RH.h"
#include "SMT_EMS.h"
#include "SMT_PMM.h"

#define NUM_BRAVO_TEMP 6
#define CHIP_SELECT_PIN 53

// Sensor objects
Bravo_Temp bravoTempSensors[NUM_BRAVO_TEMP] = {
  Bravo_Temp(A0), Bravo_Temp(A1), Bravo_Temp(A2), Bravo_Temp(A3), Bravo_Temp(A4), Bravo_Temp(A5)
};
Bravo_RH bravoRH(0x44);
SMT_RH smtRH(A7, A6);
SMT_PMM smtPMM(A8, A9);
SMT_EMS smtEMS(A10, A11);

// Logging state
unsigned long previousMillis = 0;
unsigned long LOG_INTERVAL_MS = 1000;
bool sdCardInitialized = false;
bool continuousMode = false;
bool testMode = false;
String filename = "TestRuns.csv";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  for (int i = 0; i < NUM_BRAVO_TEMP; i++) bravoTempSensors[i].initialize();
  bravoRH.initialize();
  smtRH.initialize();
  smtEMS.initialize();
  smtPMM.initialize();

  if (SD.begin(CHIP_SELECT_PIN)) {
    Serial.println("SD card initialized.");
    sdCardInitialized = true;
  } else {
    Serial.println("SD card initialization failed!");
  }
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    handleCommand(command);
  }

  if (continuousMode && millis() - previousMillis >= LOG_INTERVAL_MS) {
    previousMillis = millis();
    logAllSensors();
  }
}

void handleCommand(const String& command) {
  if (command == "SINGLE") {
    testMode = true;
    logAllSensors();
  } else if (command == "STOP") {
    continuousMode = false;
    Serial.println("Stopped logging.");
  } else if (command.startsWith("CONTINUOUS,")) {
    parseContinuousCommand(command);
  } else if (command.startsWith("READ,")) {
    flushSerialInput();
    parseReadCommand(command);
  } else if (command == "LIST") {
    flushSerialInput();
    listFiles();
  }
}

void parseContinuousCommand(const String& input) {
  int firstComma = input.indexOf(',');
  int secondComma = input.indexOf(',', firstComma + 1);
  if (firstComma == -1 || secondComma == -1) {
    Serial.println("Invalid CONTINUOUS format. Use: CONTINUOUS,YYMMDD,INTERVAL_MS");
    return;
  }

  String dateCode = input.substring(firstComma + 1, secondComma);
  String intervalStr = input.substring(secondComma + 1);
  unsigned long interval = intervalStr.toInt();

  if (interval < 1000 || interval > 86400000) {
    Serial.println("Interval must be between 1000 and 86400000 ms."); 
    return;
  }

  LOG_INTERVAL_MS = interval;
  filename = dateCode + ".csv";

  bool fileExists = SD.exists(filename.c_str());
  File logFile = SD.open(filename.c_str(), FILE_WRITE);
  if (!logFile) {
    Serial.println("Failed to open file.");
    return;
  }

  if (!fileExists) {
    logFile.println("Timestamp,BravoT0,BravoT1,BravoT2,BravoT3,BravoT4,BravoT5,BravoRH_T,BravoRH_H,SMT_RH_T,SMT_RH_H,EMS_MC,EMS_T,PMM_MC,PMM_T");
  }

  logFile.close();

  Serial.print("Logging to: ");
  Serial.print(filename);
  Serial.print(" every ");
  Serial.print(LOG_INTERVAL_MS);
  Serial.println(" ms");

  previousMillis = millis();
  continuousMode = true;
}

void parseReadCommand(String input) {
  int comma1 = input.indexOf(',');
  int comma2 = input.indexOf(',', comma1 + 1);
  if (comma1 == -1 || comma2 == -1) {
    Serial.println("Invalid READ format. Use: READ,filename.csv,line_count");
    return;
  }

  String fileToRead = input.substring(comma1 + 1, comma2);
  int count = input.substring(comma2 + 1).toInt();
  if (count <= 0) {
    Serial.println("Line count must be greater than zero.");
    return;
  }

  File file = SD.open(fileToRead.c_str());
  if (!file) {
    Serial.println("Failed to open file.");
    return;
  }

  int totalLines = 0;
  while (file.available()) {
    if (file.readStringUntil('\n').length() > 0)
      totalLines++;
  }

  file.close();
  file = SD.open(fileToRead.c_str());
  int currentLine = 0;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    currentLine++;
    if (currentLine == 1 || currentLine > totalLines - count) {
      Serial.println(line);
      delay(10);
    }
  }

  file.close();
  Serial.println("END");
}

void listFiles() {
  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open SD root.");
    return;
  }

  bool foundCsv = false;
  File entry = root.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      String name = entry.name();
      name.toUpperCase();
      if (name.endsWith(".CSV")) {
        delay(50);
        Serial.println(name);
        foundCsv = true;
      }
    }
    entry.close();
    entry = root.openNextFile();
  }

  if (!foundCsv) {
    Serial.println("NO_FILES");
  } else {
    Serial.println("END_LIST");
  }
}

void flushSerialInput() {
  while (Serial.available()) {
    Serial.read();
  }
}

void logAllSensors() {
  if (!sdCardInitialized) return;

  bool fileExists = SD.exists(filename.c_str());
  File logFile = SD.open(filename.c_str(), FILE_WRITE);
  if (!logFile) return;

  if (!fileExists) {
    logFile.println("Timestamp,BravoT0,BravoT1,BravoT2,BravoT3,BravoT4,BravoT5,BravoRH_T,BravoRH_H,SMT_RH_T,SMT_RH_H,EMS_MC,EMS_T,PMM_MC,PMM_T");
  }

  String logLine = String(millis() / 1000.0, 2);

  for (int i = 0; i < NUM_BRAVO_TEMP; i++) {
    float temp = bravoTempSensors[i].readTemperature();
    logLine += "," + String(isnan(temp) ? 0.0 : temp, 2);
  }

  float bravoT = bravoRH.readTemperature();
  float bravoH = bravoRH.readHumidity();
  logLine += "," + String(isnan(bravoT) ? 0.0 : bravoT, 2);
  logLine += "," + String(isnan(bravoH) ? 0.0 : bravoH, 2);

  logLine += "," + String(smtRH.readTemperature(), 2);
  logLine += "," + String(smtRH.readHumidity(), 2);
  logLine += "," + String(smtEMS.readMoisture(), 2);
  logLine += "," + String(smtEMS.readTemperature(), 2);
  logLine += "," + String(smtPMM.readMoisture(), 2);
  logLine += "," + String(smtPMM.readTemperature(), 2);


  if (testMode) {
    Serial.println("Timestamp,BravoT0,BravoT1,BravoT2,BravoT3,BravoT4,BravoT5,BravoRH_T,BravoRH_H,SMT_RH_T,SMT_RH_H,EMS_MC,EMS_T,PMM_MC,PMM_T");
    Serial.println(logLine);
    testMode = false;
  }

  logFile.println(logLine);
  logFile.close();
}
