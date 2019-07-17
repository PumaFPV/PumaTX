
 void Power(){   //What should be done when power on/off protocol has been done
  Serial.println("PowerON");
  digitalWrite(LED.Pin, HIGH);
  Serial.println("power on");
  delay(1000);
  digitalWrite(LED.Pin, LOW);
}


void SoftPowerInit(){
  Pwr.Time = 0; 
}

void SoftPower(){
 
  while(Step == 0 && Pause.State == 0){ 
    Pwr.Time = millis();    
    }
    while(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700){
      Step = 1;
      Serial.println("step1");
    }


  while(Step == 1 && Pause.State == 1){
    while(millis() - Pwr.Time > 900 && millis() - Pwr.Time < 1100){
        Step = 2;
        Serial.println("step2");
    }
  }

  while(Step == 2 && Pause.State == 0){
    while(millis() - Pwr.Time > 1800 && millis() - Pwr.Time <  2200){
      Step = 3;
      Serial.println("step3");
    }
  }

  if(Step == 3 && Pause.State == 1){
    Power(); 
  }
  
}

void SoftPower0(){    
  if(Pre.State == 0){   //Here you should run everything that has to be run before shuting down
    u8g2.clear();
    esp_deep_sleep_start();
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


void DeepSleep(){
    esp_deep_sleep_start();
}


