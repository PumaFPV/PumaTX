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



TaskHandle_t pxx;
//TaskHandle_t display_dualcore;
TaskHandle_t mlx_dualcore;
TaskHandle_t button;



void pxx_code( void * pvParameters ){
  byte flag1 = 0x00;
  byte receiver_number = 0x12;
  
  for(;;){
    prepare_pxx(channels, receiver_number, flag1, EU_10_mw);  //receive channels data and prepare then for PXX
    delay(6);
  }
  
}



void button_code( void * pvParameters ){
  
  pin_mode_def(); //Defines every buttons

  for(;;){
    process_buttons();

  }

}



void display_code( void * pvParameters ){
  
  display.begin();
  display.display_default();
  
  for(;;){
    display.set_text(String(channels[0]) + "a" + String(channels[2]), 500); 
    display.update_display();
  }

}



void mlx_code( void * pvParameters ){
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection

  mlx.begin();
  delay(10);
  for(;;){  
    mlx.process();
    get_mlx_data();
    compute_rc();
    rc_data();

    delay(10);
  }
  
}



void dualcore_setup(){
    xTaskCreatePinnedToCore(
                    pxx_code,    //Task function. 
                    "pxx",      //name of task. 
                    1000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    3,            //priority of the task 
                    &pxx,       //Task handle to keep track of created task 
                    1);   //which core to run on 

    /*xTaskCreatePinnedToCore(
                    display_code,    //Task function. 
                    "display",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &display_dualcore,       //Task handle to keep track of created task 
                    1); 
                    */
    xTaskCreatePinnedToCore(
                    mlx_code,    //Task function. 
                    "mlx",      //name of task. 
                    1400,        //Stack size of task 
                    NULL,         //parameter of the task 
                    2,            //priority of the task 
                    &mlx_dualcore,       //Task handle to keep track of created task 
                    1);     
  
    xTaskCreatePinnedToCore(
                    button_code,    //Task function. 
                    "button",      //name of task. 
                    1000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &button,       //Task handle to keep track of created task 
                    1);    
                                        
}
