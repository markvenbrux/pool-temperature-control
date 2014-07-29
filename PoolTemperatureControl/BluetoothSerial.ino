int serialTaskState;
unsigned long serialTaskNextTransitionTime;
int incomingByte;   // for incoming serial data

///////////////////////////////////////////////////////////////////////////////
void SerialTaskSetup() { 
  Serial.begin(9600);
  bluetoothSerial.begin(9600);
}

void SerialTask() {
    if (serialTaskNextTransitionTime < millis()){
    switch(serialTaskState) {     
      case 0: // See if new input is available      
          if (Serial.available()){
            incomingByte = Serial.read();   // for incoming serial data
            serialTaskNextTransitionTime = millis() + 10;
            serialTaskState = 1;
          }
      break;  
      case 1: // write incoming byte after a small delay
          bluetoothSerial.write(incomingByte);
          serialTaskState = 0;
      break;
    }
  }
}

