int pumpControlTaskState;
unsigned long pumpControlTaskNextTransitionTime;

void PumpControlTaskSetup(void) {
  // initialize the relay pins as an output.
  digitalWrite(P_PUMP, RELAY_OFF);
  pinMode(P_PUMP, OUTPUT);
  
  pumpControlTaskState = -1;
  pumpControlTaskNextTransitionTime = millis();
}


void PumpControlTask(){
  
  if (pumpControlTaskNextTransitionTime < millis()){
    switch(pumpControlTaskState) {
      
      case -1: // Run pump initially, thus it will run at least once a day.
        digitalWrite(P_PUMP, RELAY_ON);   
        pumpControlTaskState = 0;   
        pumpControlTaskNextTransitionTime = millis() + 30*second;
      break;
      
      case 0: // Start temperature conversions
        StartTemperatureConversion(TAmbientAddr);
        StartTemperatureConversion(TPoolAddr);
        StartTemperatureConversion(TCollectorAddr);
        StartTemperatureConversion(TCollectorOutAddr);   
        pumpControlTaskState = 1;   
        pumpControlTaskNextTransitionTime = millis() + 1*second;
      break;
  
      case 1: // Read temperature and control pump
        temperatures.tAmbient = ReadTemperature(TAmbientAddr);
        UpdateStatistics(temperatures.tAmbient, &eeprom.minTemperatures.tAmbient, &eeprom.maxTemperatures.tAmbient);
        temperatures.tPool = ReadTemperature(TPoolAddr);
        UpdateStatistics(temperatures.tPool, &eeprom.minTemperatures.tPool, &eeprom.maxTemperatures.tPool);
        temperatures.tCollector = ReadTemperature(TCollectorAddr);
        UpdateStatistics(temperatures.tCollector, &eeprom.minTemperatures.tCollector, &eeprom.maxTemperatures.tCollector);
        temperatures.tCollectorOut = ReadTemperature(TCollectorOutAddr);
        UpdateStatistics(temperatures.tCollectorOut, &eeprom.minTemperatures.tCollectorOut, &eeprom.maxTemperatures.tCollectorOut);
               
        if (
            ((temperatures.tCollector - temperatures.tPool) > 0.2 && temperatures.tPool < eeprom.settings.tPoolMaxLimit) ||
            ((temperatures.tCollector - temperatures.tPool) < -0.2 && temperatures.tPool > eeprom.settings.tPoolMaxLimit) 
         ) {
          // Start pump
          digitalWrite(P_PUMP, RELAY_ON);
        } else {
          // Stop pump
          digitalWrite(P_PUMP, RELAY_OFF);
        }
        pumpControlTaskState = 2;
      break;
      
      case 2: // Sleep
      default:
        pumpControlTaskNextTransitionTime = millis() + 5*second;//+ 5*minute;
        pumpControlTaskState = 0;    
      break;
      
    }
  }
}
void UpdateStatistics(double t, double * min, double * max) {
  if (t < *min) *min = t;
  if (t > *max) *max = t;
}

