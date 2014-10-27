void MonitorTemperaturesTaskSetup(timeslicedTask_t *t) {
  t->state = 0;
  t->nextTransitionTime = millis();
}

void MonitorTemperaturesTask(timeslicedTask_t *t){
  
  if (t->nextTransitionTime < millis()){
    switch(t->state) {
      
      case 0: // Start temperature conversions
        StartTemperatureConversion(TAmbientAddr);
        StartTemperatureConversion(TPoolAddr);
        StartTemperatureConversion(TCollectorAddr);
        StartTemperatureConversion(TCollectorOutAddr);   
        t->state = 1;   
        t->nextTransitionTime = millis() + 1*second;
      break;
  
      case 1: {// Read temperatures 
        bool readResult = true;
        readResult = ReadTemperature(TAmbientAddr, &temperatures.tAmbient) && readResult;
        readResult = ReadTemperature(TPoolAddr, &temperatures.tPool) && readResult;
        readResult = ReadTemperature(TCollectorAddr, &temperatures.tCollector) && readResult;
        readResult = ReadTemperature(TCollectorOutAddr, &temperatures.tCollectorOut) && readResult;

        if (readResult) {
          UpdateStatistics(temperatures.tAmbient, &eeprom.minTemperatures.tAmbient, &eeprom.maxTemperatures.tAmbient);
          UpdateStatistics(temperatures.tPool, &eeprom.minTemperatures.tPool, &eeprom.maxTemperatures.tPool);
          UpdateStatistics(temperatures.tCollector, &eeprom.minTemperatures.tCollector, &eeprom.maxTemperatures.tCollector);
          UpdateStatistics(temperatures.tCollectorOut, &eeprom.minTemperatures.tCollectorOut, &eeprom.maxTemperatures.tCollectorOut); 
        }
        t->nextTransitionTime = millis() + 9*second; // one measurement per 10 seconds
        t->state = 0;    
      }
      break;
    }
  }
}

void UpdateStatistics(double t, double * min, double * max) {
  if (t < *min) *min = t;
  if (t > *max) *max = t;
}

