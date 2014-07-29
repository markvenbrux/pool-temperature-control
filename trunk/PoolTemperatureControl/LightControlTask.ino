
int lightControlTaskState;
unsigned long lightControlTaskNextTransitionTime;

void LightControlTaskSetup(void) {
  digitalWrite(P_LIGHT, RELAY_OFF);
  pinMode(P_LIGHT, OUTPUT);
  pinMode(P_MOTION, INPUT);
 
  lightControlTaskState = 0;
  lightControlTaskNextTransitionTime = millis() + second;
}


void LightControlTask() {  
  if (lightControlTaskNextTransitionTime < millis()){
    switch(lightControlTaskState) {     
      case 0: // Check PIR sensor: turn light on when motion is detected     
        if (digitalRead(P_MOTION) == HIGH) {
          // Switch light on
          digitalWrite(P_LIGHT, RELAY_ON);
          lightControlTaskNextTransitionTime = millis() + 60*second;
          lightControlTaskState = 1; 
        } else {
          // Remain off, test again soon
          lightControlTaskNextTransitionTime = millis() + 1*second;
        }   
      break;  
      case 1: // Check PIR sensor: turn light off when no motion is detected        
        if (digitalRead(P_MOTION) == LOW) {
          // Switch light off
          digitalWrite(P_LIGHT, RELAY_OFF);
          lightControlTaskNextTransitionTime = millis() + 1*second;
          lightControlTaskState = 0; 
        } else {
          // Remain on, test again after a while
          lightControlTaskNextTransitionTime = millis() + 60*second;
        }
      break;  
    }
  }
}


