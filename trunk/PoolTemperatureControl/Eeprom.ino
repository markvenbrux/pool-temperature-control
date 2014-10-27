#include <avr/eeprom.h>

void EepromSetup() { 
  eeprom_read_block((void*)&(eeprom), (void*)0, sizeof(eeprom));
  if (eeprom.writeCount == -1){
    ResetEeprom();
  }
  Serial.print("writeCount: ");Serial.println(eeprom.writeCount);
  Serial.print("tPoolMaxLimit: ");Serial.println(eeprom.settings.tPoolMaxLimit);
  Serial.print("tPoolMinLimit: ");Serial.println(eeprom.settings.tPoolMinLimit);
}

void ResetSettings() {
  Serial.println("Reset settings.");
  eeprom.settings.tPoolMaxLimit = 29;
  eeprom.settings.tPoolMinLimit = 1;
  eeprom.settings.runPumpControlTask = true;
  SaveEeprom();
}

void ResetStatistics() {
  Serial.println("Reset statistics.");
  SetTemperatures(&eeprom.minTemperatures, 1000);
  SetTemperatures(&eeprom.maxTemperatures, -1000);
  eeprom.pumpSwitchCount = 0;
}

void SetTemperatures(temperatures_t * t, double value){
  (*t).tAmbient = value;
  (*t).tPool = value;
  (*t).tCollector = value;
  (*t).tCollectorOut = value;
}

void ResetEeprom() {
  Serial.println("Reset eeprom.");
  ResetSettings();
  ResetStatistics();
}

void SaveEeprom() {
  eeprom.writeCount++;
  eeprom_write_block((const void*)&eeprom, (void*)0, sizeof(eeprom));
}

