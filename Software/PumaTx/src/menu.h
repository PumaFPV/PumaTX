/*
  Here si the structure I want to have yet:
  1-Telem
    - page 1
    - page 2
    - page 3
    - page 4
  2- RC Config
    - Model number
    - Model type
    - Calibrate
  3- RF Config
    - Protocol: PXX, DSM2, SX1280
    - RX number
    - RF mode: EU, US / 10mw, 100mw
    - Bind
    - Range
 */


volatile int page = 0;
volatile int line = 0;

void menuLoop();

void telemPage();
void telemLine_1();
void telemLine_2();
void telemLine_3();
void telemLine_4();

void rcConfigPage();
void rcConfigLine_1();
void rcConfigLine_2();
void rcConfigLine_3();

void rfConfigPage();
void rfConfigLine_1();
void rfConfigLine_2();
void rfConfigLine_3();
void rfConfigLine_4();
void rfConfigLine_5();

void navigation();

void menuLoop()
{

  //int test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 1, 6);
  //display.setLeftGraph(test, 1);
  //display.update();
  int test;
  switch(page)
  {
    case 0:
      //Serial.println("  pumatx  ");
      display.setText("  pumatx  ");
      test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 1, 6);
      display.setLeftGraph(test, 1);
      display.setNamedRssi(page, 3);
      display.update();
      break;
   
    case 1: //Telem
      display.setText("telem");
      display.setName("page");
      display.setNamedRssi(page, 3);
      display.update();
      if(ok.state)  //if selected
      {
        telemPage();   
      }
      break;
      
    case 2: //RC Config
      display.setText("rc config");
      display.setName("page");
      display.setNamedRssi(page, 3);
      display.update();
      rcConfigPage();
      break;
      
    case 3: //RF Config
      display.setText("rf config");
      display.setName("page");
      display.setNamedRssi(page, 3);
      display.update();
      rfConfigPage();
      break;
  }
  
}



void telemPage()
{
  switch(line)
  {
    case 1:
      telemLine_1();
      break;
      
    case 2:
      telemLine_2();
      break;
    
    case 3:
      telemLine_3();
      break;
      
    case 4:
      telemLine_4();
      break;
  }
}

void telemLine_1()
{
  display.setName("line");
  display.setNamedRssi(1, 4);
  display.setText("telem p1");
}

void telemLine_2()
{
  display.setName("line");
  display.setNamedRssi(2, 4);
  display.setText("telem p2");
}

void telemLine_3()
{
  display.setName("line");
  display.setNamedRssi(3, 4);
  display.setText("telem p3");
}

void telemLine_4()
{
  display.setName("line");
  display.setNamedRssi(4, 4);
  display.setText("telem p4");
}



void rcConfigPage()
{
  switch(line)
  {
    case 1:
      rcConfigLine_1();
    case 2:
      rcConfigLine_2();
    case 3:
      rcConfigLine_3();
  }
}

void rcConfigLine_1()
{
  display.setName("line");
  display.setNamedRssi(1, 3);
  display.setText("rc1");
}

void rcConfigLine_2()
{
  display.setName("line");
  display.setNamedRssi(2, 3);
  display.setText("rc2");
}

void rcConfigLine_3()
{
  display.setName("line");
  display.setNamedRssi(3, 3);
  display.setText("rc3");
}



void rfConfigPage()
{
  Serial.print("line: ");
  Serial.println(line);

  switch(line)
  {
    case 1:
      rfConfigLine_1();
    case 2:
      rfConfigLine_2();
    case 3:
      rfConfigLine_3();
    case 4:
      rfConfigLine_4();
    case 5:
      rfConfigLine_5(); 
  }
}

void rfConfigLine_1()
{
  display.setName("line");
  display.setNamedRssi(1, 5);
  display.setText("rf1");
}

void rfConfigLine_2()
{
  display.setName("line");
  display.setNamedRssi(2, 5);
  display.setText("rf2");
}

void rfConfigLine_3()
{
  display.setName("line");
  display.setNamedRssi(3, 5);
  display.setText("rf3");
}

void rfConfigLine_4()
{
  display.setName("line");
  display.setNamedRssi(4, 5);
  display.setText("rf4");
}

void rfConfigLine_5()
{
  display.setName("line");
  display.setNamedRssi(5, 5);
  display.setText("rf5");
}

void navigation(){
    
  if (right.state == 0 && page < 3 && right.prev == 1 && millis() - right.currentTime > debounceDelay) 
  { //menu right -> page+
    ++page;
    line = 0;
    right.currentTime = millis(); 
  }

  if (left.state == 0 && page > 0 && left.prev == 1 && millis() - left.currentTime > debounceDelay)
  { //menu left -> page-
    --page;
    line = 0;
    left.currentTime = millis();
  }
  
  if(down.state == 0 && line < 5 && down.prev == 1 && millis() - down.currentTime > debounceDelay)
  {
    ++line;
    down.currentTime = millis();
  }

  if(up.state == 0 && line > 0 && up.prev == 1 && millis() - up.currentTime > debounceDelay)
  {
    --line;
    up.currentTime = millis();
  }

  if(play.state == 0)
  {
    page = 0;
    line = 0;
  }
}

void displayTxBattery()
{
    voltage.state = analogRead(voltage.pin);
    voltage.process = (-0.023 * (voltage.state * voltage.state) + 233.93 * voltage.state - 145559) / 100;  //output in mV
    voltage.output = constrain(map(voltage.process, 3500, 4190, 0, 100), 0, 100);
    display.setTxBatteryPercentage(voltage.output, 1);
    display.setTxBatteryBar(map(voltage.output, -20, 80, 0, 3), 1);

}