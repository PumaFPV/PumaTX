void Power(){   //What should be done when power on/off protocol has been done
  digitalWrite(LED.Pin, HIGH);
  Serial.println("power on");
  delay(1000);
  digitalWrite(LED.Pin, LOW);
}

void SoftPower(){
Pwr.Time = 0; 


  if(Step == 0 && Pre.State == 0){
    Pwr.Time = millis();
  }
    if(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700){
      Step = 1;
      Serial.println("step1");
    }



  if(Step == 1 && Pre.State == 1){
      Pwr.Time = millis();
  }
    if(millis() - Pwr.Time > 900 && millis() - Pwr.Time < 1100){
        Step = 2;
        Serial.println("step2");
    }



  if(Step == 2 && Pre.State == 0){
    Pwr.Time = millis();
  }
    if(millis() - Pwr.Time > 1800 && millis() - Pwr.Time <  2200){
    Step = 3;
    Serial.println("step3");
    }

  if(Step == 3 && Pre.State == 1){
    Power(); 
  }
  
}
