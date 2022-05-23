#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"
#include "R9M.h"
#include "menu.h"

void setup() 
{
  Serial.begin(115200); //Starts Serial connection
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
  mlx.begin();  //Initialise MLX
  display.begin();

  pin_mode_def(); //Defines every buttons
  display.display_default();

}

void loop() 
{

  process_buttons();
  compute_rc();
  rc_data();

  receiver_number = 0x12;
  
  display.set_text(String(channels[0]) + " " + String(channels[2]), 500);

  unsigned long current_millis_pxx = millis();

  if (current_millis_pxx - previous_millis_pxx >= interval_pxx) 
  {
    mlx.process();

    get_mlx_data();
    
    previous_millis_pxx = current_millis_pxx;
    prepare_pxx(channels, receiver_number, flag1, EU_10_mw);  //receive channels data and prepare then for PXX
    display.update_display();
  }
  

}
