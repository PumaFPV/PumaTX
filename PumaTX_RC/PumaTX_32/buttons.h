void PinModeDef(){

  pinMode(Right.Pin, INPUT_PULLUP);
  pinMode(Left.Pin, INPUT_PULLUP);
  pinMode(Up.Pin, INPUT_PULLUP);
  pinMode(Down.Pin, INPUT_PULLUP);
  pinMode(Ok.Pin, INPUT_PULLUP);
  pinMode(RTH.Pin, INPUT_PULLUP);    
  pinMode(Arm.Pin, INPUT_PULLUP);
  pinMode(Pre.Pin, INPUT_PULLUP);
  pinMode(Pwr.Pin, INPUT_PULLUP);
  pinMode(Pause.Pin, INPUT_PULLUP);
  pinMode(LED.Pin, OUTPUT);
  
}


void ProcessButtons(){
  
  //Navigation joystick
  Ok.State = digitalRead(Ok.Pin);
  Right.State = digitalRead(Right.Pin);
  Left.State = digitalRead(Left.Pin);
  Up.State = digitalRead(Up.Pin);
  Down.State = digitalRead(Down.Pin);

  //Switches
  RightPot.State = analogRead(RightPot.Pin);
  LeftPot.State = analogRead(LeftPot.Pin);
  Arm.State = digitalRead(Arm.Pin);
  Pre.State = digitalRead(Pre.Pin);
  RTH.State = digitalRead(RTH.Pin);
  Pwr.State = digitalRead(Pwr.Pin);
  Pause.State = digitalRead(Pause.Pin);
  
}