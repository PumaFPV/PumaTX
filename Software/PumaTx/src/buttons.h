void pinModeDef()
{

  pinMode(right.pin, INPUT_PULLUP);
  pinMode(left.pin,  INPUT_PULLUP);
  pinMode(up.pin,    INPUT_PULLUP);
  pinMode(down.pin,  INPUT_PULLUP);
  pinMode(ok.pin,    INPUT_PULLUP);
  //pinMode(rth.pin, INPUT_PULLUP);    
  pinMode(arm.pin,   INPUT_PULLUP);
  pinMode(pre.pin,   INPUT_PULLUP);
  pinMode(pwr.pin,   INPUT_PULLUP);
  pinMode(play.pin,  INPUT_PULLUP);
  pinMode(c1.pin,    INPUT_PULLUP);
  pinMode(c2.pin,    INPUT_PULLUP);
  pinMode(led.pin,   OUTPUT);

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
  c1.state = digitalRead(c1.pin);
  c2.state = digitalRead(c2.pin);
  Serial.print(arm.state);
  Serial.print(" ");
  Serial.print(pre.state);
  Serial.print(" ");
  Serial.print(pwr.state);
  Serial.print(" ");
  Serial.print(play.state);
  Serial.print(" ");
  Serial.print(c1.state);
  Serial.print(" ");
  Serial.print(c2.state);
  Serial.print(" ");
  Serial.print(right.state);
  Serial.print(" ");
  Serial.print(left.state);
  Serial.print(" ");
  Serial.print(up.state);
  Serial.print(" ");
  Serial.print(down.state);
  Serial.print(" ");
  Serial.print(ok.state);
  Serial.println();
  
}
