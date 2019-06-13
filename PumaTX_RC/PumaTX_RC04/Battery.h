void ReadVoltage(){
  
  Voltage.State = analogRead(Voltage.Pin) * ( 5.00 / 1023.00) / 0.4443;
  
}

void ShowBatteryVoltage(void){
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 50);
  u8g2.print("Voltage:");
  u8g2.setCursor(0, 57);
  u8g2.print(Voltage.State);

  int BatteryPercentage = 0;
  float lipo100 = 4.2;
  float lipo90 = 4.13;
  float lipo80 = 4.06;
  float lipo70 = 3.99;
  float lipo60 = 3.92;
  float lipo50 = 3.85;
  float lipo40 = 3.78;
  float lipo30 = 3.71;
  float lipo20 = 3.64;
  float lipo10 = 3.57;
  float lipo0 = 3.5;

  if(Voltage.State > 4.2){
    BatteryPercentage = 100;
  }
  else 
    if(Voltage.State > 4.13){
      BatteryPercentage = map(Voltage.State, lipo90, lipo100, 90, 100);
    }
    else 
      if(Voltage.State > 4.06){
        BatteryPercentage = map(Voltage.State, lipo80, lipo90, 80, 90); 
      }
      else
        if(Voltage.State > 3.99){
          BatteryPercentage = map(Voltage.State, lipo70, lipo80, 70, 80);
        }
        else
          if(Voltage.State > 3.92){
            BatteryPercentage = map(Voltage.State, lipo60, lipo70, 60, 70);
          }
          else
            if(Voltage.State > 3.85){
              BatteryPercentage = map(Voltage.State, lipo50, lipo60, 50, 60);
            }
            else
              if(Voltage.State > 3.78){
                BatteryPercentage = map(Voltage.State, lipo40, lipo50, 40, 50);
              }
              else 
                if(Voltage.State > 3.71){
                  BatteryPercentage = map(Voltage.State, lipo30, lipo40, 30, 40);
                }
                else 
                  if(Voltage.State > 3.64){
                    BatteryPercentage = map(Voltage.State, lipo20, lipo30, 20, 30);
                  }
                  else
                    if(Voltage.State > 3.57){
                      BatteryPercentage = map(Voltage.State, lipo10, lipo20, 10, 20);
                    }
                    else
                      if(Voltage.State > 3.5){
                        BatteryPercentage = map(Voltage.State, lipo0, lipo10, 0, 10);
                      }
                      else BatteryPercentage = 0;
}
