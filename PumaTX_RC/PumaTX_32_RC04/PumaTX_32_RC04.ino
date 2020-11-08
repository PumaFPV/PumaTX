#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"
#include "mlx.h"
#include "R9M.h"


void setup() 
{
  Serial.begin(115200); //Starts Serial connection
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
  setup_left_mlx();
  setup_right_mlx();

  pin_mode_def(); //Defines every buttons
}

void loop() 
{

  process_buttons();
  compute_rc();
  rc_data();

  receiver_number = 0x12;
  

  unsigned long current_millis_pxx = millis();
  
  if (current_millis_pxx - previous_millis_pxx >= interval_pxx) 
  {

    previous_millis_pxx = current_millis_pxx;
    prepare_pxx(channels, receiver_number, flag1, EU_10_mw);  //receive channels data and prepare then for PXX
    
    if(left_mlx)
    {
      loop_left_mlx();
    }
    else
    {
      loop_right_mlx();
    }
    left_mlx = !left_mlx;
    
  }

}
