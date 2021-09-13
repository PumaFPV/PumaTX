/*
ESP32 Dual core example code

Serial.println(xPortGetCoreID());
vTaskDelay(100);

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
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

  } 
}

*/

TaskHandle_t mlx_dualcore;
TaskHandle_t button_dualcore;
TaskHandle_t menu_dualcore;
TaskHandle_t rc_dualcore;


void button_code( void * pvParameters )
{
  pin_mode_def(); //Defines every buttons

  unsigned int previous_button_loop_time = 0;
  for(;;){
    if(millis() - previous_button_loop_time > 10)
    {
      process_buttons();
      previous_button_loop_time = millis();
    }
  }
}

void mlx_code( void * pvParameters )
{
  mlx.begin();
  unsigned int previous_mlx_loop_millis = 0;

  for(;;){  
    if(millis() - previous_mlx_loop_millis > 10)
    {
      mlx.process();
      get_mlx_data();
    
      previous_mlx_loop_millis = millis();
    }
  }
  
}

void menu_code(void * pvParameters)
{
  display.begin();
  display.display_default();
  unsigned int previous_menu_loop_millis = 0;

  for(;;)
  {
    if(millis()- previous_menu_loop_millis > 40)
    {
      navigation();
      menu_loop();
      Serial.println(page);
      previous_menu_loop_millis = millis();
    }
  }
}

void rc_code(void * pvParameters)
{

  unsigned int previous_rc_loop_millis = 0;

  for(;;)
  {
    if(millis()- previous_rc_loop_millis > 40)
    {
      compute_rc();
      rc_data();
      previous_rc_loop_millis = millis();
    }
  }
}