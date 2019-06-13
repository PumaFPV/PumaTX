void SoftPower(){

  if(Pwr.Pin == 0){
    Step = 1;

    /*
    Pwr.Time = millis();
    if(millis() - Pwr.Time < 1200 && millis() - Pwr.Time > 800){
      Step = 1
    }
    */
  }

  if(Step == 1 && Pwr.Pin == 1){
      Pwr.Time = millis();
      if(millis() - Pwr.Time < 900 && millis() - Pwr.Time > 1100){
        Step = 2;
      }
  }

  if(Step == 2 && Pwr.Pin == 0){
    Pwr.Time = millis();
    if(millis() - Pwr.Time < 1800 && millis() - Pwr.Time >  2200)
    Step = 3;
  }
  
}
