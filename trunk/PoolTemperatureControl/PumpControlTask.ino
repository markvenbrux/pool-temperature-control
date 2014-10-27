void PumpControlTaskSetup(timeslicedTask_t *t) {
  // initialize the relay pins as an output.
  digitalWrite(P_PUMP, RELAY_OFF);
  pinMode(P_PUMP, OUTPUT);
  
  t->state = 0;
  t->nextTransitionTime = millis() + 2*second;
}


void PumpControlTask(timeslicedTask_t *t){
  if (!eeprom.settings.runPumpControlTask) return;
  
  if (t->nextTransitionTime < millis()){
    switch(t->state) {
      
      case 0: // Run pump initially, thus it will run at least once a day.
        t->nextTransitionTime = millis() + 30*second;
        digitalWrite(P_PUMP, RELAY_ON);   
        t->state = 1;   
      break;
      
      case 1: // Pump is running 
        t->nextTransitionTime = millis() + 3*minute;
        if (!RunPump()) {
          // Stop pump
          digitalWrite(P_PUMP, RELAY_OFF);
          t->state = 2;
        }
      break;
      
      case 2: // Pump is running 
        t->nextTransitionTime = millis() + 3*minute;
        if (RunPump()) {
          // Start pump
          eeprom.pumpSwitchCount++;
          digitalWrite(P_PUMP, RELAY_ON);
          t->state = 1;
        }
      break;
    }
  }
}


bool RunPump() {
  double deltaTcTp = temperatures.tCollector - temperatures.tPool;          
  double deltaTcoTp = temperatures.tCollectorOut - temperatures.tPool;          
  return
    (deltaTcTp > 0.2 && temperatures.tPool < eeprom.settings.tPoolMaxLimit) ||
    (deltaTcTp < -0.2 && temperatures.tPool > eeprom.settings.tPoolMaxLimit) ||
    (deltaTcoTp > 0.2 && temperatures.tPool < eeprom.settings.tPoolMaxLimit) ||
    (deltaTcoTp < -0.2 && temperatures.tPool > eeprom.settings.tPoolMaxLimit) ||
    temperatures.tCollector < eeprom.settings.tPoolMinLimit;
}
