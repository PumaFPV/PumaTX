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
  unsigned long previous_button_millis = 0;
  const long button_interval = 10;

  pin_mode_def(); //Defines every buttons

  for(;;){
    unsigned long current_button_millis = millis();

    if(current_button_millis - previous_button_millis >= button_interval)
    {
      previous_button_millis = current_button_millis;

      process_buttons();

    }
  }
}



void mlx_code( void * pvParameters )
{
  unsigned long previous_mlx_millis = 0;
  const long mlx_interval = 10;

  mlx.begin();

  for(;;){  
    unsigned long current_mlx_millis = millis();

    if(current_mlx_millis - previous_mlx_millis >= mlx_interval)
    {
      previous_mlx_millis = current_mlx_millis;
     
      mlx.process();
      get_mlx_data();
    
    }
  }
  
}

void menu_code(void * pvParameters)
{
  unsigned long previous_menu_millis = 0;
  const long menu_interval = 50;

  display.begin();
  display.display_default();

  for(;;)
  {
    unsigned long current_menu_millis = millis();

    if(current_menu_millis - previous_menu_millis >= menu_interval)
    {
      previous_menu_millis = current_menu_millis;

      navigation();
      menu_loop();
    }
  }
}

void rc_code(void * pvParameters)
{

  unsigned long previous_rc_millis = 0;
  const long rc_interval = 40;

  for(;;)
  {
    unsigned long current_rc_millis = millis();

    if(current_rc_millis - previous_rc_millis >= rc_interval)
    {
      previous_rc_millis = current_rc_millis;

      compute_rc();
      rc_data(); 
    }
  }
}