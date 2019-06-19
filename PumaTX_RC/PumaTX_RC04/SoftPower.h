void SoftPower(){

  if(Pwr.Pin == 0 && Step == 0){
    Pwr.Time = millis();
    if(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700){
      Step = 1;
    }
    
  }

  if(Step == 1 && Pwr.Pin == 1){
      Pwr.Time = millis();
      if(millis() - Pwr.Time > 900 && millis() - Pwr.Time < 1100){
        Step = 2;
      }
  }

  if(Step == 2 && Pwr.Pin == 0){
    Pwr.Time = millis();
    if(millis() - Pwr.Time > 1800 && millis() - Pwr.Time <  2200)
    Step = 3;
  }

  if(Step == 3 && Pwr.Pin == 1){
    PowerProcedure = 1; 
  }
  
}



void Power(){   //What should be done when power on/off protocol has been done
  
}
