
#define MAX_CMD_LENGTH 33
static char rcvCommand[MAX_CMD_LENGTH];      // command receive buffer
static char parseCommand[MAX_CMD_LENGTH];    // command parse buffer
static int rcvIndex = 0;                     // index in receive buffer

// The tokens
const char* T_PRINT_ALL = "a";
const char* T_HELP = "h";
const char* T_SET = "s";
const char* T_GET = "g";
const char* T_TPOOLMAXLIMIT = "tpmax";
const char* T_TPOOLMINLIMIT = "tpmin";
const char* T_RUNPUMPCONTROLTASK = "rpct";
const char* T_PINSET = "ps";
const char* T_CURRENT = "current";
const char* T_ALL = "all";
const char* T_MIN = "min";
const char* T_MAX = "max";
const char* T_RESET_SETTINGS = "rse";
const char* T_RESET_STATISTICS = "rst";

const char* T_HELP_TEXT = 
"Available commands:\n"
"a: print all tempreatures\n"
"s [tpmax|tpmin]=xy.z : set tPoolLimit\n"
"s rpct=[0|1] : set runPumpControlTask [off|on]\n"
"g [tpmax|tpmin] : get tPoolLimit\n"
"ps x=[0|1] : set pin x [low|high]\n"
"rse : reset settings\n"
"rst : reset statistics"
;

///////////////////////////////////////////////////////////////////////////////
void CommandParserTaskSetup(timeslicedTask_t *t) { 
}

///////////////////////////////////////////////////////////////////////////////
void CommandParserTask(timeslicedTask_t *t) {
  if ( Serial.available() > 0  ) {        
    char c = Serial.read();
    Serial.write(c);
    
    CommandParserParse(c);
  }  
}

void CommandParserParse(char c) {
  if ( c != '\r' && c != '\n' && rcvIndex < MAX_CMD_LENGTH-1) {
    if (c != '\b') {
      rcvCommand[rcvIndex++] = c;
    } else if (rcvIndex > 0) {
      rcvIndex--;      
    }
  } else {
    Serial.println();
    rcvCommand[rcvIndex] = '\0'; // Terminate the received string
    bool parseResult = ParseSetCommand(rcvCommand);
    if (!parseResult) parseResult = ParseGetCommand(rcvCommand);
    if (!parseResult) parseResult = ParsePinSetCommand(rcvCommand);
    if (!parseResult) parseResult = ParseCommand(rcvCommand);
    if (!parseResult) {
      Serial.print("Failed to parse: ");
      Serial.println(rcvCommand);
    }
    rcvIndex=0;
    rcvCommand[0] = '\0';
  }
}

bool ParseSetCommand(const char * command) {
  bool result = false;
  strcpy(parseCommand, rcvCommand);
  char * p = strtok(parseCommand, " ");
  if(p && strcmp(p, T_SET)==0) {
    char * field = strtok(NULL, "=" );
    if(field) {
      char * value = strtok(NULL, "'\0'");
      if(value) {
        if (strcmp(field, T_TPOOLMAXLIMIT)==0) {
          double d = strtod(value, NULL);
          eeprom.settings.tPoolMaxLimit = d;
          result = true;
        } else if (strcmp(field, T_TPOOLMINLIMIT)==0) {
          double d = strtod(value, NULL);
          eeprom.settings.tPoolMinLimit = d;
          result = true;
        } else if (strcmp(field, T_RUNPUMPCONTROLTASK)==0) {
          bool b = atoi(value);
          eeprom.settings.runPumpControlTask = b;
          result = true;
        }
      }
    }
    if (result) SaveEeprom();
  }    
  return result;
}

bool ParsePinSetCommand(const char * command) {
  bool result = false;
  char parseCommand[MAX_CMD_LENGTH];
  strcpy(parseCommand, rcvCommand);
  char * p = strtok(parseCommand, " ");
  if(p && strcmp(p, T_PINSET)==0) {
    char * pinStr = strtok(NULL, "=" );
    if(pinStr) {
      char * valueStr = strtok(NULL, "'\0'");
      if(valueStr) {
        int pin = atoi(pinStr);
        int value = atoi(valueStr);
        digitalWrite(pin, value);
        pinMode(pin, OUTPUT);
        result = true;
      }
    }
  }    
  return result;
}

bool ParseGetCommand(const char * command) {
  bool result = false;
  strcpy(parseCommand, rcvCommand);
  char * cmd = strtok(parseCommand, " ");
  if(cmd && strcmp(cmd, T_GET)==0) {
    char * field = strtok(NULL, "'\0'" );
    if(field && strcmp(field, T_TPOOLMAXLIMIT)==0){
      Serial.print("tPoolMax = ");  Serial.println(eeprom.settings.tPoolMaxLimit);
      result = true;
    }
    if(field && strcmp(field, T_TPOOLMINLIMIT)==0){
      Serial.print("tPoolMin = ");  Serial.println(eeprom.settings.tPoolMinLimit);
      result = true;
    }
    if(field && strcmp(field, T_ALL)==0){
      PrintTemperatures(T_CURRENT, temperatures);
      PrintTemperatures(T_MIN, eeprom.minTemperatures);
      PrintTemperatures(T_MAX, eeprom.maxTemperatures);
      result = true;
    }
  }
  return result;
}

bool ParseCommand(const char * command) {
  bool result = false;
  strcpy(parseCommand, rcvCommand);
  char * cmd = strtok(parseCommand, "'\0'");
          if(cmd && strcmp(cmd, T_RESET_SETTINGS)==0) {
    ResetSettings();
    result = true;
  } else if(cmd && strcmp(cmd, T_RESET_STATISTICS)==0) {
    ResetStatistics();
    result = true;
  } else if(cmd && strcmp(cmd, T_PRINT_ALL)==0) {
      Serial.print("writeCount: ");Serial.println(eeprom.writeCount);
      Serial.print("pumpSwitchCount: ");Serial.println(eeprom.pumpSwitchCount);      
      Serial.print("tPoolMaxLimit: ");Serial.println(eeprom.settings.tPoolMaxLimit);
      Serial.print("tPoolMinLimit: ");Serial.println(eeprom.settings.tPoolMinLimit);
      PrintTemperatures(T_CURRENT, temperatures);
      PrintTemperatures(T_MIN, eeprom.minTemperatures);
      PrintTemperatures(T_MAX, eeprom.maxTemperatures);
    result = true;
  } else if(cmd && strcmp(cmd, T_HELP)==0) {
     Serial.println(T_HELP_TEXT);
     result = true;
  }
  return result;
}

