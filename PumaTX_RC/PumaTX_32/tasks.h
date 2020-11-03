/*
ESP32 Dual core example code


TaskHandle_t Task1;

void setup_task(){
    xTaskCreatePinnedToCore(
                    Task1code,    //Task function. 
                    "Task1",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &Task1,       //Task handle to keep track of created task 
                    0); 
}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led1, HIGH);
    delay(1000);
    digitalWrite(led1, LOW);
    delay(1000);
  } 
}

*/

TaskHandle_t PXX;

void PXX_code( void * pvParameters ){


  for(;;){
    prepare_PXX();
  }
  
}

void setup_task(){
    xTaskCreatePinnedToCore(
                    PXX_code,    //Task function. 
                    "PXX",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    0,            //priority of the task 
                    &PXX,       //Task handle to keep track of created task 
                    0); 
}
