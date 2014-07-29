#include <avr/eeprom.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
void EepromSetup() { 
  eeprom_read_block((void*)&(eeprom.settings), (void*)0, sizeof(eeprom.settings));
  if (eeprom.settings.writeCount == -1){
    ResetSettings();
  }
  Serial.println(eeprom.settings.writeCount);
  Serial.println(eeprom.settings.tPoolMaxLimit);
}

void ResetSettings() {
  Serial.println("Reset settings.");
  eeprom.settings.tPoolMaxLimit = 29;
  eeprom.settings.runPumpControlTask = true;
  SaveSettings();
}

void SaveSettings() {
  eeprom.settings.writeCount++;
  eeprom_write_block((const void*)&eeprom.settings, (void*)0, sizeof(eeprom.settings));
}
