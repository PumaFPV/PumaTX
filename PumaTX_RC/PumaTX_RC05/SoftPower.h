void DeepSleep(){
    esp_deep_sleep_start();
}

void PowerON(){
  
}


void PowerOFF(){
  ScreenGoodBye();
  delay(1000);
  DeepSleep();
}


void Power(){   //What should be done when power on/off protocol has been done
  Serial.println("Power procedure");
  digitalWrite(LED.Pin, HIGH);
  
  if (Pwr.Output = 1){
    PowerOFF();
  }
  else if (Pwr.Output = 0){
    PowerON();
  }
}


void IRAM_ATTR SoftPower(){
 
  while(Step == 0 && Pwr.State == 0){ 
    Pwr.Time = millis();
    Step = 5;    
    }
    while(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700 && Step = 5){
      Step = 1;
      Serial.println("step1");
    }


  while(Step == 1 && Pwr.State == 1){
    Pwr.Time = millis();
    Step = 15;    
    }
    while(millis() - Pwr.Time > 900 && millis() - Pwr.Time < 1100 && Step = 15){
        Step = 2;
        Serial.println("step2");
     }

  while(Step == 2 && Pwr.State == 0){
    Pwr.Time = millis();
    Step = 25;
    }
    while(millis() - Pwr.Time > 1800 && millis() - Pwr.Time <  2200){
      Step = 3;
      Serial.println("step3");
    }

  if(Step == 3 && Pwr.State == 1){
    Power(); 
  }
  
}


void SoftPower1(){
 ok = 0;
  if(Step == 0 && Pause.State == 0){ 
    if(ok == 0){
    Pwr.Time = millis();    
    ok = 1;
    }
    if(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700){
      Step = 1;
      Serial.println("step1");
    }
  }


  if(Step == 1 && Pause.State == 1){
    if(millis() - Pwr.Time > 900 && millis() - Pwr.Time < 1100){
        Step = 2;
        Serial.println("step2");
    }
  }

  if(Step == 2 && Pause.State == 0){
    if(millis() - Pwr.Time > 1800 && millis() - Pwr.Time <  2200){
      Step = 3;
      Serial.println("step3");
    }
  }

  if(Step == 3 && Pause.State == 1){
    //Power(); 
  }
  
}
