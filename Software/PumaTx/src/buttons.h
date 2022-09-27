void pinModeDef()
{

  pinMode(right.pin, INPUT_PULLUP);
  pinMode(left.pin, INPUT_PULLUP);
  pinMode(up.pin, INPUT_PULLUP);
  pinMode(down.pin, INPUT_PULLUP);
  pinMode(ok.pin, INPUT_PULLUP);
  pinMode(rth.pin, INPUT_PULLUP);    
  pinMode(arm.pin, INPUT_PULLUP);
  pinMode(pre.pin, INPUT_PULLUP);
  pinMode(pwr.pin, INPUT_PULLUP);
  pinMode(play.pin, INPUT_PULLUP);
  pinMode(led.pin, OUTPUT);
  pinMode(17, OUTPUT);
  
}


void processButtons()
{
  
  //Navigation joystick
  ok.state = digitalRead(ok.pin);
  right.state = digitalRead(right.pin);
  left.state = digitalRead(left.pin);
  up.state = digitalRead(up.pin);
  down.state = digitalRead(down.pin);

  //Switches
  rightPot.state = analogRead(rightPot.pin);
  leftPot.state = analogRead(leftPot.pin);
  arm.state = digitalRead(arm.pin);
  pre.state = digitalRead(pre.pin);
  rth.state = digitalRead(rth.pin);
  pwr.state = digitalRead(pwr.pin);
  play.state = digitalRead(play.pin);
  
}
