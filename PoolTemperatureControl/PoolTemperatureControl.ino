#include <OneWire.h>
#include <SoftwareSerial.h>
#include "MyTypes.h"
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
// http://www.pjrc.com/teensy/td_libs_OneWire.html
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

// Pin definitions
//Relay pins
#define P_BT_RX 4
#define P_BT_TX 5
#define P_PUMP 6
#define P_LIGHT 7
#define P_8 8
#define P_9 9
// Temperature sensors
#define P_ONEWIRE 10
// Motion sensor
#define P_MOTION 11

#define RELAY_ON 0
#define RELAY_OFF 1

// Bluetooth serial
// Find this library with reference at http://www.extrapixel.ch/processing/bluetoothDesktop/
SoftwareSerial bluetoothSerial(P_BT_RX, P_BT_TX);

eeprom_t eeprom;
temperatures_t temperatures;

OneWire  ds(P_ONEWIRE);  // on pin 10 (a 4.7K resistor is necessary)
const byte TPoolAddr[8] =         {0x28, 0xDB, 0x96, 0xF2, 0x5, 0x0, 0x0, 0x4A};
const byte TCollectorOutAddr[8] = {0x28, 0xE0, 0xD8, 0xF1, 0x5, 0x0, 0x0, 0x9C};
const byte TCollectorAddr[8] =    {0x28, 0x7A, 0x30, 0xF2, 0x5, 0x0, 0x0, 0x8E};
const byte TAmbientAddr[8] =      {0x28, 0x56, 0xD3, 0xF3, 0x5, 0x0, 0x0, 0x5E};

void setup(void) {
  SerialTaskSetup();
  CommandParserTaskSetup();
  EepromSetup();
  PumpControlTaskSetup();
  // LightControlTaskSetup();
  WatchDogTaskSetup();
  // initialize the relay pins as an output.
  digitalWrite(P_8, RELAY_OFF);
  digitalWrite(P_9, RELAY_OFF);
  pinMode(P_8, OUTPUT);
  pinMode(P_9, OUTPUT);
}

void loop(void) {
  if (eeprom.settings.runPumpControlTask) PumpControlTask();
  SerialTask();
  CommandParserTask();
  // LightControlTask();
  WatchDogTask();
}


