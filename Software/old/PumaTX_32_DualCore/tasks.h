/*
ESP32 Dual core example code



Serial.println(xPortGetCoreID());
vTaskDelay

TaskHandle_t Task1;

void setup_task(){
    xTaskCreatePinnedToCore(
                    Task1code,    //Task function. 
                    "Task1",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 3 is highest 0 is lowest
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
TaskHandle_t main;
TaskHandle_t mlx;
TaskHandle_t button;



void PXX_code( void * pvParameters ){
  
  setup_PXX(0x12, EU_10_mw); //Rx number, power and zone

  for(;;){
    prepare_PXX();
  }
  
}



void button_code( void * pvParameters ){
  
  PinModeDef(); //Defines every buttons

  for(;;){
    ProcessButtons();
  }

}



void main_code( void * pvParameters ){
  
  for(;;){ 
    ComputeRC4();
    RCdata();
  }

}



void mlx_code( void * pvParameters ){
  
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
  SetupLeftMLX();
  SetupRightMLX();
  
  for(;;){  
    LoopLeftMLX();
    vTaskDelay(5);
    LoopRightMLX();
  }
  
}



void setup_task(){
    xTaskCreatePinnedToCore(
                    PXX_code,    //Task function. 
                    "PXX",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    3,            //priority of the task 
                    &PXX,       //Task handle to keep track of created task 
                    0); 

    xTaskCreatePinnedToCore(
                    main_code,    //Task function. 
                    "main",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &main,       //Task handle to keep track of created task 
                    1); 
                    
    xTaskCreatePinnedToCore(
                    mlx_code,    //Task function. 
                    "mlx",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    2,            //priority of the task 
                    &mlx,       //Task handle to keep track of created task 
                    1);     

    xTaskCreatePinnedToCore(
                    button_code,    //Task function. 
                    "button",      //name of task. 
                    10000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    0,            //priority of the task 
                    &button,       //Task handle to keep track of created task 
                    1);                                 
}
