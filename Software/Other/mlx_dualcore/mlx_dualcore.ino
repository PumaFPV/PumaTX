#include <Wire.h>
#include "MLX.h"


#define I2C_SDA 4
#define I2C_SCL 13

MLX mlx(0x0C, 0x0D);  //Left, Right
TaskHandle_t mlx_dualcore;

void setup()
{
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection

  xTaskCreatePinnedToCore(
                          mlx_code,    //Task function. 
                          "mlx",      //name of task. 
                          1400,        //Stack size of task 
                          NULL,         //parameter of the task 
                          2,            //priority of the task 
                          &mlx_dualcore,       //Task handle to keep track of created task 
                          1);   
}

void loop(){}


void mlx_code( void * pvParameters ){

  mlx.begin();
  for(;;){  
    mlx.process();
    delay(5);
  }
  
}
