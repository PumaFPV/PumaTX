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

TaskHandle_t pxx_dualcore;
TaskHandle_t mlx_dualcore;
TaskHandle_t button_dualcore;
TaskHandle_t menu_dualcore;

void pxx_code( void * pvParameters )
{
  byte flag1 = 0x00;
  byte receiver_number = 0x12;
  
  for(;;){
    prepare_pxx(channels, receiver_number, flag1, EU_10_mw);  //receive channels data and prepare then for PXX
    delay(6);
  }
}



void button_code( void * pvParameters )
{
  
  pin_mode_def(); //Defines every buttons

  for(;;){
    process_buttons();

  }
}



void mlx_code( void * pvParameters )
{
  display.begin();
  display.display_default();
  mlx.begin();

  for(;;){  
    
    mlx.process();
    get_mlx_data();
    compute_rc();
    rc_data();
    menu_loop();
    Serial.println(page);
    //Serial.println(uxTaskGetStackHighWaterMark(NULL));

    delay(5);
  }
  
}

void menu_code(void * pvParameters)
{
  for(;;)
  {
    navigation();
    delay(100);
  }
}
