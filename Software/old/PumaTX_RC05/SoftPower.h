void DeepSleep(){
  esp_deep_sleep_start();
}

void PowerON(){
  digitalWrite(LED.Pin, HIGH);
}


void PowerOFF(){
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
  digitalWrite(LED.Pin, LOW);
  ScreenGoodBye();
  delay(1000);
  DeepSleep();
}


void Power(){   //What should be done when power on/off protocol has been done
  Serial.println("Power procedure");  
  if (Pwr.Output == 1){
    PowerOFF();
  }
  else if (Pwr.Output == 0){
    PowerON();
  }
}

void SP3(){
  Pwr.FirstPress = millis();
  while(Pwr.State == LOW){
    Pwr.PressedTime = millis() - Pwr.FirstPress; 
    if(1700 < millis() - Pwr.Time && millis() - Pwr.Time < 2300){
      Power();
    }
    else if(2300 < millis() - Pwr.Time){
      PowerOFF();
    }
  }
}

void SP2(){
  Pwr.FirstPress = millis();
  while(Pwr.State == HIGH){
	Pwr.PressedTime = millis() - Pwr.FirstPress;
    if(850 < (millis() - Pwr.Time) && (millis() - Pwr.Time) < 1150){
      SP3();
    }
    else if(1200 < millis() - Pwr.Time){
		PowerOFF();
    }
  }
}

void IRAM_ATTR SP1(){
  Pwr.FirstPress = millis();
  detachInterrupt(Pwr.Pin);
  while(Pwr.State == LOW){
    //DisplayBattery(true);
	Pwr.PressedTime = millis() - Pwr.FirstPress;
    if(350 < Pwr.PressedTime && Pwr.PressedTime < 650){
      SP2();
    }
  }
}

void IRAM_ATTR SoftPower(){ //uses steps 
 
  while(Step == 0 && Pwr.State == 0){ 
    Pwr.Time = millis();
    Step = 5;    
    detachInterrupt(Pwr.Pin);
  }

    while(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700 && Step == 5){
      Step = 1;
      Serial.println("step1");
    }

  while(Step == 1 && Pwr.State == 1){
    Pwr.Time = millis();
    Step = 15;    
  }
  while(900 < millis() - Pwr.Time && 1100 > millis() - Pwr.Time && Step == 15){
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
    attachInterrupt(Pwr.Pin, SoftPower, FALLING);
    Power(); 
    }
  
}



void IRAM_ATTR SoftPowerTimeout(){
 
  while(Step == 0 && Pwr.State == 0){ 
    Pwr.Time = millis();
    Step = 5;    
    detachInterrupt(Pwr.Pin);
  }
  if(millis() - Pwr.Time < 700){  
    while(millis() - Pwr.Time > 300 && millis() - Pwr.Time < 700 && Step == 5){
      Step = 1;
      Serial.println("step1");
    }
  }
  else if(millis() - Pwr.Time > 700){
    Step = 0;
  }
  while(Step == 1 && Pwr.State == 1){
    Pwr.Time = millis();
    Step = 15;    
  }
  while(900 < millis() - Pwr.Time && 1100 > millis() - Pwr.Time && Step == 15){
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
    attachInterrupt(Pwr.Pin, SoftPower, FALLING);
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


  
void SoftPowerInit(){
  attachInterrupt(Pwr.Pin, SoftPower/* or SP1*/, FALLING);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
}
