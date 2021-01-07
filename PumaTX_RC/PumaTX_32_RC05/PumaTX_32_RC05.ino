#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"
#include "R9M.h"
#include "menu.h"
#include "dualcore.h"

void setup() 
{
  Serial.begin(115200); //Starts Serial connection
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection

    xTaskCreatePinnedToCore(
                    pxx_code,    //Task function. 
                    "pxx",      //name of task. 
                    1000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    3,            //priority of the task 
                    &pxx_dualcore,       //Task handle to keep track of created task 
                    1);   //which core to run on 
            
    xTaskCreatePinnedToCore(
                    mlx_code,    //Task function. 
                    "mlx",      //name of task. 
                    1800,        //Stack size of task 
                    NULL,         //parameter of the task 
                    2,            //priority of the task 
                    &mlx_dualcore,       //Task handle to keep track of created task 
                    1);     
  
    xTaskCreatePinnedToCore(
                    button_code,    //Task function. 
                    "button",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &button_dualcore,       //Task handle to keep track of created task 
                    1); 
                    
    xTaskCreatePinnedToCore(
                    menu_code,    //Task function. 
                    "menu",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &menu_dualcore,       //Task handle to keep track of created task 
                    1);
}

void loop(){}
