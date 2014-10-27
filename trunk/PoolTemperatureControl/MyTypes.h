const unsigned long second = 1000;
const unsigned long minute = second*60;
const unsigned long hour = minute*60;
const unsigned long day = hour*24;

int taskState;
unsigned long nextTransitionTime;

struct timeslicedTask_t {
  char * name;
  void (*Setup)(timeslicedTask_t*);
  void (*Task)(timeslicedTask_t*);
  int state;
  unsigned long nextTransitionTime;
  void * taskData;
};

void PrintTask(timeslicedTask_t t){
  Serial.print("Task: "); Serial.println(t.name);
  Serial.print("  state      = "); Serial.println(t.state);
  Serial.print("  nextTrans. = "); Serial.println(t.nextTransitionTime);
 }
 
struct settings_t {
  bool runPumpControlTask;
  double tPoolMaxLimit;
  double tPoolMinLimit;
};
struct temperatures_t {
  double tAmbient;
  double tPool;
  double tCollector;
  double tCollectorOut;
};
struct eeprom_t {
  int writeCount;
  settings_t settings;
  int pumpSwitchCount;
  temperatures_t minTemperatures;
  temperatures_t maxTemperatures;
};

void PrintTemperatures(const char * label, temperatures_t t){
  Serial.println(label);
  Serial.print("  tAmbient      = "); Serial.println(t.tAmbient);
  Serial.print("  tPool         = "); Serial.println(t.tPool);
  Serial.print("  tCollector    = "); Serial.println(t.tCollector);
  Serial.print("  tCollectorOut = "); Serial.println(t.tCollectorOut);
 }


