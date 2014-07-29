const unsigned long second = 1000;
const unsigned long minute = second*60;
const unsigned long hour = minute*60;
const unsigned long day = hour*24;

typedef struct settings_t {
  bool runPumpControlTask;
  double tPoolMaxLimit;
};
typedef struct temperatures_t {
  double tAmbient;
  double tPool;
  double tCollector;
  double tCollectorOut;
};
typedef struct eeprom_t {
  int writeCount;
  settings_t settings;
  temperatures_t minTemperatures;
  temperatures_t maxTemperatures;
};

