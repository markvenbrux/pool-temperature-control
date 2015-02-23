
#include <avr/wdt.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "MyTypes.h"
// Use with a Arduino pro mini ATMega328 (3.3V, 8MHz) and HC-06 Bluetooth 
// module for both program uploading and interaction.
// Why 57600 baud?
// Use with Bluetooth module on arduino physical serial port: configure the BT
// module to operate at 57600 baud to match with the baudrate expected 
// by the bootloader for the Arduino pro mini ATMega328 (3.3V, 8MHz).
// To upload new software from the arduino IDE:
// - press '->' (upload) button
// - hold arduino reset button until 3 sec. after 
//   message "Sketch uses ... Maximum is 2,048 bytes." message appeared.
// TODO: find a way to trigger an automatic reset of the Arduino while 
// uploading via Bluetooth.
// http://letsmakerobots.com/content/programming-arduino-bluetooth
// http://wiki.pinguino.cc/index.php/SPP_Bluetooth_Modules

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
// http://www.pjrc.com/teensy/td_libs_OneWire.html
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library
// Temperature sensor DS18B20 waterproof stainless steel
// tube (6*50mm) encapsulation.
// Power supply range: 3.0V to 5.5V 
// Operating temperature range: -55°C to +125°C 
// Accuracy over the range of -10°C to +85°C: ±0.5°C. 
// Output lead: red (VCC), yellow(DATA) , black(GND) 
// Wire at pool: blue(VCC), white(DATA) , purple(GND) 

// Pin definitions
// Relay pins
#define P_PUMP 4
#define P_LIGHT 5
#define P_8 8
#define P_9 9
// Temperature sensors
#define P_ONEWIRE 10
// Motion sensor
#define P_MOTION 11
// Status led
#define P_STATUS_LED 13

#define RELAY_ON 0
#define RELAY_OFF 1

eeprom_t eeprom;
temperatures_t temperatures;

OneWire  ds(P_ONEWIRE);  // on pin 10 (a 4.7K resistor is necessary)
const byte TPoolAddr[8] =         {0x28, 0xDB, 0x96, 0xF2, 0x5, 0x0, 0x0, 0x4A};
const byte TCollectorOutAddr[8] = {0x28, 0xE0, 0xD8, 0xF1, 0x5, 0x0, 0x0, 0x9C};
const byte TCollectorAddr[8] =    {0x28, 0x7A, 0x30, 0xF2, 0x5, 0x0, 0x0, 0x8E};
const byte TAmbientAddr[8] =      {0x28, 0x56, 0xD3, 0xF3, 0x5, 0x0, 0x0, 0x5E};

timeslicedTask_t blinkTask = {"Blink", BlinkTaskSetup, BlinkTask};
timeslicedTask_t watchDogTask = {"WatchDog", WatchDogTaskSetup, WatchDogTask};
timeslicedTask_t pumpControlTask = {"PumpControl", PumpControlTaskSetup, PumpControlTask};
timeslicedTask_t commandParserTask = {"PumpControl", CommandParserTaskSetup, CommandParserTask};
timeslicedTask_t monitorTemperaturesTask = {"MonitorTemperatures", MonitorTemperaturesTaskSetup, MonitorTemperaturesTask};

timeslicedTask_t tasks[] = {blinkTask, watchDogTask, pumpControlTask, commandParserTask, monitorTemperaturesTask};
const int numberOfTasks = (sizeof(tasks)/sizeof(timeslicedTask_t));

void setup(void) {
  // We always need to make sure the WDT is disabled immediately after a 
  // reset, otherwise it will continue to operate with default values.
  wdt_disable();  
  Serial.begin(57600);
  EepromSetup();  
  for (int i = 0; i < numberOfTasks; i++) {
    tasks[i].Setup(&tasks[i]);
  }
  digitalWrite(P_8, RELAY_OFF);
  digitalWrite(P_9, RELAY_OFF);
  pinMode(P_8, OUTPUT);
  pinMode(P_9, OUTPUT);
}

void loop(void) {
  for (int i = 0; i < numberOfTasks; i++) {
    tasks[i].Task(&tasks[i]);
  }
}


