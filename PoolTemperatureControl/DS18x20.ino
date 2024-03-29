
int crcErrorCount = 0;

void StartTemperatureConversion(const byte addr[8]) {
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end
}

bool ReadTemperature(const byte addr[8], double *pTemperature) {
  // we might do a ds.depower() here, but the reset will take care of it.
  bool result = true;
  byte i;
  byte data[12];
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for ( i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  if ( OneWire::crc8( data, 8) != data[8]) {
    crcErrorCount++;
    Serial.print("ReadTemperature: CrcErrorCount: "); Serial.println(crcErrorCount);
    result = false;
  } else {
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
    *pTemperature = (float)raw / 16.0;
  }
  return result;
}

