int getBatteryVoltage(){
  
  Voltage.State = analogRead(Voltage.Pin) * ( 5.00 / 1023.00) / 0.4443;
  return Voltage.State;

}



int getBatteryPercentage(){

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
  
return BatteryPercentage;
  
}
