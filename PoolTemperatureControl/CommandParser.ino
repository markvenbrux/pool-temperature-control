
#define MAX_CMD_LENGTH 33
static char rcvCommand[MAX_CMD_LENGTH];                            // command receive buffer
static char parseCommand[MAX_CMD_LENGTH];                          // command parse buffer
static int rcvIndex = 0;                                           // index in receive buffer

// The tokens
const char* T_SET = "s";
const char* T_GET = "g";
const char* T_TPOOLMAXLIMIT = "tpm";
const char* T_RUNPUMPCONTROLTASK = "rpct";
const char* T_PINSET = "ps";
const char* T_CURRENT = "current";
const char* T_ALL = "all";
const char* T_MIN = "min";
const char* T_MAX = "max";
const char* T_RESET_SETTINGS = "rse";
const char* T_RESET_STATISTICS = "rst";



///////////////////////////////////////////////////////////////////////////////
void CommandParserTaskSetup() { 
}

///////////////////////////////////////////////////////////////////////////////
void CommandParserTask() {
  if ( bluetoothSerial.available() > 0  ) {        
    char c = bluetoothSerial.read();
    bluetoothSerial.write(c);
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
    rcvCommand[rcvIndex] = '\0'; // Terminate the received string
    bool parseResult = ParseSetCommand(rcvCommand);
    if (!parseResult) parseResult = ParseGetCommand(rcvCommand);
    if (!parseResult) parseResult = ParsePinSetCommand(rcvCommand);
    if (!parseResult) parseResult = ParseCommand(rcvCommand);
    if (!parseResult) {
      bluetoothSerial.print("Failed to parse: ");
      bluetoothSerial.println(rcvCommand);
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
      bluetoothSerial.print("tPoolMax = ");  bluetoothSerial.println(eeprom.settings.tPoolMaxLimit);
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
  } else if(cmd && strcmp(cmd, "a")==0) {
      PrintTemperatures(T_ALL, temperatures);
      PrintTemperatures(T_MIN, eeprom.minTemperatures);
      PrintTemperatures(T_MAX, eeprom.maxTemperatures);
    result = true;
  }
  return result;
}

