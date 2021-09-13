#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "rc.h"
#include "menu.h"
#include "dualcore.h"

void setup() 
{
  Serial.begin(115200); //Starts Serial connection
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
            
    xTaskCreatePinnedToCore(
                    mlx_code,    //Task function. 
                    "mlx reading",      //name of task. 
                    1800,        //Stack size of task 
                    NULL,         //parameter of the task 
                    2,            //priority of the task 
                    &mlx_dualcore,       //Task handle to keep track of created task 
                    1);     //Core
  
    xTaskCreatePinnedToCore(
                    button_code,    //Task function. 
                    "button reading",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &button_dualcore,       //Task handle to keep track of created task 
                    1);     //Core
                    
    xTaskCreatePinnedToCore(
                    menu_code,    //Task function. 
                    "menu",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &menu_dualcore,       //Task handle to keep track of created task 
                    1);     //Core
    
    xTaskCreatePinnedToCore(
                    rc_code,    //Task function. 
                    "rc",      //name of task. 
                    2000,        //Stack size of task 
                    NULL,         //parameter of the task 
                    1,            //priority of the task 
                    &rc_dualcore,       //Task handle to keep track of created task 
                    1);     //Core

}

void loop(){}
