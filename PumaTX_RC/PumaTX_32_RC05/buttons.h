void pin_mode_def()
{
  //GPIO.enable_w1tc = ((uint32_t)1 << right.pin);  //input
  //GPIO.enable_w1ts = ((uint32_t)1 << rc_pin);  //output
  //gpio_set_direction(rc_pin, OUTPUT);
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
  pinMode(led.pin, OUTPUT);  //output
  pinMode(rc_pin, OUTPUT);  
  
}



void process_buttons()
{
  
  //Navigation joystick
  ok.state = (GPIO.in >> ok.pin) & 0x1;
  right.state = (GPIO.in >> right.pin) & 0x1;
  left.state = (GPIO.in >> left.pin) & 0x1;
  up.state = (GPIO.in >> up.pin) & 0x1;
  down.state = (GPIO.in >> down.pin) & 0x1;

  //Switches
  rightpot.state = analogRead(rightpot.pin);
  leftpot.state = analogRead(leftpot.pin);
  arm.state = (GPIO.in >> arm.pin) & 0x1;
  pre.state = (GPIO.in >> pre.pin) & 0x1;
  rth.state = (GPIO.in >> rth.pin) & 0x1;
  pwr.state = (GPIO.in >> pwr.pin) & 0x1;
  play.state = (GPIO.in >> play.pin) & 0x1;
  
}
