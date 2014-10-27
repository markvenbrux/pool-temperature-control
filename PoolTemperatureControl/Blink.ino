struct blinkTaskData {
  int blinkSpeed;
};

void BlinkTaskSetup(timeslicedTask_t *t) {
  digitalWrite(P_STATUS_LED, LOW);
  pinMode(P_STATUS_LED, OUTPUT);  
  t->state = 0;
  t->nextTransitionTime = millis() + 1*second;
  t->taskData = malloc(sizeof(struct blinkTaskData));
  blinkTaskData* d = (blinkTaskData*)t->taskData;
  d->blinkSpeed = 1000;
}

void BlinkTask(timeslicedTask_t *t){
  blinkTaskData* d = (blinkTaskData*)t->taskData;
  
  if (t->nextTransitionTime < millis()){
    switch(blinkTask.state) {
      
      case 0:
        t->nextTransitionTime = millis() + d->blinkSpeed;
        //Serial.print("Millies: ");Serial.println(millis());
        digitalWrite(P_STATUS_LED, HIGH);   
        t->state = 1;   
      break;
      
      case 1:
        t->nextTransitionTime = millis() + d->blinkSpeed;
        digitalWrite(P_STATUS_LED, LOW);   
        t->state = 0;   
      break;
     
    }
  }
}

