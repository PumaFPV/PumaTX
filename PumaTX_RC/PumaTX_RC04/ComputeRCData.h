 void GetMLXData(){
  
  Throttle.Reading = mlx.getThrottle();
  Yaw.Reading = mlx.getYaw();
  Pitch.Reading = mlx.getPitch();
  Roll.Reading = mlx.getRoll();
  
}

void ComputeRC3(){
  
if (Throttle.Reading > 50000){
  Throttle.Inter = Throttle.Reading - 65535;
}
else Throttle.Inter = Throttle.Reading;
Throttle.Output = map(Throttle.Inter, Throttle.Min, Throttle.Max, -100, 100);


if (Yaw.Reading > 50000){
  Yaw.Inter = Yaw.Reading - 65535;
}
else Yaw.Inter = Yaw.Reading;
Yaw.Output = map(Yaw.Inter, Yaw.Min, Yaw.Max, -100, 100);

if (Pitch.Reading > 50000){
  Pitch.Inter = Pitch.Reading - 65535;
}
else Pitch.Inter = Pitch.Reading;
Pitch.Output = map(Pitch.Inter, Pitch.Min, Pitch.Max, -100, 100);

if( Roll.Reading > 50000){
  Roll.Inter = Roll.Reading - 65535;
}
else Roll.Inter = Roll.Reading;
Roll.Output = map(Roll.Inter, Roll.Min, Roll.Max, -100, 100);


  RightPot.Output = map(RightPot.State, 0, 4095, 100, -100);
  RightPot.Output = constrain(RightPot.Output, -100, 100);

  LeftPot.Output = map(LeftPot.State, 3570, 440, -100, 100);
  constrain(LeftPot.Output, -100, 100);

/*
  LeftPot.Process = map(LeftPot.State, 3570, 440, -100, 100);
  currenttime = millis();
  if (LeftPot.Process > 75 && millis() - currenttime > debouncedelay && LeftPot.Output < 80){
    LeftPot.Output += 40;
  }
  currenttime .= millis();
  if (LeftPot.Process > -75 && millis() - currenttime > debouncedelay && LeftPot.Output > -80){
    LeftPot.Output -= 40;
  }
  currenttime = millis();
*/

  if (Arm.State == 0 && Arm.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (Arm.Output == -100)
      Arm.Output = 100;
    else
      Arm.Output = -100;

  }

  Arm.Prev = Arm.State;

  if (RTH.State == 0 && RTH.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (RTH.Output == -100)
      RTH.Output = 100;
    else
      RTH.Output = -100;

    currenttime = millis();    
  }

  RTH.Prev = RTH.State;

  Pre.Output = map(Pre.State, 0, 1, 100, -100);
  
}


void ComputeRC2(){
  
  if(Throttle.Reading < 35){
    Throttle.Output = map(Throttle.Reading, 0, 34, 0, 100);
  }
  else 
  Throttle.Output = map(Throttle.Reading, 255, 223, -1, -100);

  if(Yaw.Reading < 38){
    Yaw.Output = map(Yaw.Reading, 37, 5, -100, 0);
  }
  else 
  Yaw.Output = map(Yaw.Reading, 255, 224, 0, 100);
  
  if(Pitch.Reading < 36){
    Pitch.Output = map(Pitch.Reading, 4, 35, 0, 100);
  }
  else 
  Pitch.Output = map(Pitch.Reading, 255, 224, -1, -100);  //May need to be adjusted
  
  if(Roll.Reading < 36){
    Roll.Output = map(Roll.Reading, 0, 35, -1, -100);
  }
  else 
  Roll.Output = map(Roll.Reading, 255, 221, 1, 100);

  RightPot.Output = map(RightPot.State, 0, 4095, 100, -100);
  constrain(RightPot.Output, -100, 100);

/*
  LeftPot.Output = map(LeftPot.State, 3570, 440, -100, 100);
  constrain(LeftPot.Output, -100, 100);
*/
  LeftPot.Process = map(LeftPot.State, 3570, 440, -100, 100);
  currenttime = millis();
  if (LeftPot.Process > 75 && millis() - currenttime > debouncedelay && LeftPot.Output < 80){
    LeftPot.Output += 40;
  }
  currenttime = millis();
  if (LeftPot.Process > -75 && millis() - currenttime > debouncedelay && LeftPot.Output > -80){
    LeftPot.Output -= 40;
  }
  currenttime = millis();


  if (Arm.State == 0 && Arm.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (Arm.Output == -100)
      Arm.Output = 100;
    else
      Arm.Output = -100;

  }

  Arm.Prev = Arm.State;

  if (RTH.State == 0 && RTH.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (RTH.Output == -100)
      RTH.Output = 100;
    else
      RTH.Output = -100;

    currenttime = millis();    
  }

  RTH.Prev = RTH.State;

  Pre.Output = map(Pre.State, 0, 1, 100, -100);
  
}



void Calibrate(){
  if (page == 1){   //Calibrate procedure begin
  //Serial.print(readingmenuok);
  //Serial.print("  ");
  
 if (Ok.State == 0 || calibrate == 1) {
    lastdebouncetime = millis();
 }
  if (millis() - lastdebouncetime > 2000){
    calibrate = 1;

    //Calibrate trims
    Throttle.Trim = Throttle.Reading;  
    Yaw.Trim = Yaw.Reading;
    Pitch.Trim = Pitch.Reading;
    Roll.Trim = Roll.Reading;  

    //Calibrate min/max----------Throttle
    if (Throttle.Reading > Throttle.Max){
      Throttle.Max = Throttle.Reading - 20;
    }
    if(Throttle.Reading < Throttle.Min){
      Throttle.Min = Throttle.Reading + 20;
    }
    
    //Calibrate min/max----------Yaw
    if (Yaw.Reading > Yaw.Max){
      Yaw.Max = Yaw.Reading - 20;
    }
    if (Yaw.Reading < Yaw.Min){
      Yaw.Min = Yaw.Reading + 20;
    }

    //Calibrate min/max----------Pitch
    if (Pitch.Reading > Pitch.Max){
      Pitch.Max = Pitch.Reading - 20;
    }
    if (Pitch.Reading < Pitch.Min){
      Pitch.Min = Pitch.Reading + 20;
    }

    //Calibrate min/max----------Roll
    if (Roll.Reading > Roll.Max){
      Roll.Max = Roll.Reading - 20;
    }
    if (Roll.Reading < Roll.Min){
      Roll.Min = Roll.Reading + 20;
    }
//--------------------------------------------------EEPROM--------------------------------------------------
    //compact way to store calibration values in eeprom
    EEPROM.write(Throttle.EepromAddrMin, map(Throttle.Min, HallSensorMin, HallSensorMax, 0, 255));    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(Throttle.EepromAddrMax, map(Throttle.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Throttle.EepromAddrTrim, map(Throttle.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Yaw.EepromAddrMin, map(Yaw.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Yaw.EepromAddrMax, map(Yaw.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Yaw.EepromAddrTrim, map(Yaw.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Pitch.EepromAddrMin, map(Pitch.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Pitch.EepromAddrMax, map(Pitch.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Pitch.EepromAddrTrim, map(Pitch.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Roll.EepromAddrMin, map(Roll.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Roll.EepromAddrMax, map(Roll.Max, HallSensorMin, HallSensorMax, 0, 255)); 
    EEPROM.write(Roll.EepromAddrTrim, map(Roll.Trim, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.commit();
  } //END Calibration
  
  //Serial.print("Calibration State:");
  //Serial.println(calibrate);
}
else 
  calibrate = 0;

}



void ComputeRC(){
  if (Throttle.Reading <= Throttle.Trim) {    //------------------------------Throttle
    Throttle.Output = map(Throttle.Reading, Throttle.Min, Throttle.Trim, -100, 0);
  }
  else if (Throttle.Reading > Throttle.Trim) {
    Throttle.Output = map(Throttle.Reading, Throttle.Trim, Throttle.Max, 0, 100);
  }

  if (Yaw.Reading <= Yaw.Trim) {    //------------------------------yaw
    Yaw.Output = map(Yaw.Reading, Yaw.Min, Yaw.Trim, -100, 0);
  }
  else if (Yaw.Reading > Yaw.Trim) {
    Yaw.Output = map(Yaw.Reading, Yaw.Trim, Yaw.Max, 0, 100);
  }
  Yaw.Output = - Yaw.Output; //reverse yaw output

  if (Pitch.Reading <= Pitch.Trim) {    //------------------------------pitch
   Pitch.Output = map(Pitch.Reading, Pitch.Min, Pitch.Trim, -100, 0);
  }
  else if (Pitch.Reading > Pitch.Trim) {
    Pitch.Output = map(Pitch.Reading, Pitch.Trim, Pitch.Max, 0, 100);
  }

  if (Roll.Reading <= Roll.Trim) {    //------------------------------roll
    Roll.Output = map(Roll.Reading, Roll.Min, Roll.Trim, -100, 0);
  }
  else if (Roll.Reading > Roll.Trim) {
    Roll.Output = map(Roll.Reading, Roll.Trim, Roll.Max, 0, 100);
  }
    Roll.Output = - Roll.Output; //reverse roll output

  Throttle.Output = constrain(Throttle.Output, -100, 100);
  Yaw.Output = constrain(Yaw.Output, -100, 100);
  Pitch.Output = constrain(Pitch.Output, -100, 100);
  Roll.Output = constrain(Roll.Output, -100, 100);

}
