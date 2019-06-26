/*
 void Power(){   //What should be done when power on/off protocol has been done
  digitalWrite(LED.Pin, HIGH);
  Serial.println("power on");
  delay(1000);
  digitalWrite(LED.Pin, LOW);
}
*/

void SoftPowerInit(){
  Pwr.Time = 0;
 
}



void SoftPower0(){    
  if(Pre.State == 0){   //Here you should run everything that has to be ran before shuting down
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

void SoftPower2(){
  if(Pause.State == 1 && Step == 0){
    Step = 1;
  }

  if(Pause.State == 0 && Step == 1){
    while(Step < 2 ){
      Pwr.Time = millis();
      Step = 2;
    }
    
    
  }


  
}



void DeepSleep(){
    esp_deep_sleep_start();
}
