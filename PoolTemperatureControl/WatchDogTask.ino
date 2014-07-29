#include <avr/wdt.h>
#include <avr/stdbool.h>

unsigned long watchDogTaskNextTransitionTime;

void WatchDogTaskSetup(void) {
  watchDogTaskNextTransitionTime = millis() + day;
}

void WatchDogTask() {
  if (watchDogTaskNextTransitionTime < millis()){
    SaveEeprom();
    resetArduino();
  }
}

void resetArduino() {
  wdt_enable(WDTO_15MS);
  noInterrupts();
  while(1); // wait to die and be reborn....
}


