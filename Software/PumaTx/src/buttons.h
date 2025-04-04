void pinModeDef()
{

  pinMode(right.pin, INPUT);
  pinMode(left.pin,  INPUT);
  pinMode(up.pin,    INPUT);
  pinMode(down.pin,  INPUT);
  pinMode(ok.pin,    INPUT);
  pinMode(rth.pin,   INPUT);    
  pinMode(arm.pin,   INPUT);
  pinMode(pre.pin,   INPUT);
  pinMode(pwr.pin,   INPUT_PULLUP);
  pinMode(play.pin,  INPUT);
  pinMode(c1.pin,    INPUT);
  pinMode(c2.pin,    INPUT);
  pinMode(sport.pin, INPUT_PULLUP);
  pinMode(led.pin,   OUTPUT);
  
}


void processRcButtons()
{
  //Buttons
  arm.state = digitalRead(arm.pin);
  pre.state = digitalRead(pre.pin);
  rth.state = digitalRead(rth.pin);
  c1.state = digitalRead(c1.pin);
  c2.state = digitalRead(c2.pin);
  sport.state = digitalRead(sport.pin);

  //Pots
  rightPot.state = analogRead(rightPot.pin);
  leftPot.state = analogRead(leftPot.pin);

}

void processNavButtons()
{
  //Navigation joystick
  ok.state = digitalRead(ok.pin);
  right.state = digitalRead(right.pin);
  left.state = digitalRead(left.pin);
  up.state = digitalRead(up.pin);
  down.state = digitalRead(down.pin);
  pwr.state = digitalRead(pwr.pin);
  play.state = digitalRead(play.pin);

}

