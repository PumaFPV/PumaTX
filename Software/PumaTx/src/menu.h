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
    - Flash
  4- Function stats
    -mlx
    -button
    -rc
    -crsf
    -menu
    -haptic
 */


#define PAGES 4
#define TELEM_LINES 4
#define RC_LINES 3
#define RF_LINES 6
#define FUNC_LINES 6

volatile int page = 0;
volatile int line = 0;

uint8_t mW = 250;
String countryCode = "eu";

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
void rfConfigLine_6();

void funcStatPage();

void navigation();

void displayTxBattery();
void computeBattery();
void displayThrottle();
void displayPitch();

uint8_t maxLines = 0;
uint8_t lastPage = 255;
uint8_t lastLine = 255;
uint32_t update = 0; 
uint32_t lastUpdate = 0;
uint8_t lastTest = 0;
uint8_t lastTestP = 0;

int test, testP;

void menuHandler()
{

  //int test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 1, 6);
  //display.setLeftGraph(test, 1);
  //display.update();

  if(line == 0)
  {
    display.setName("page");
    display.setNamedRssi(page, PAGES);
  }

  switch(page)
  {

    case 0:
      maxLines = 0;
      if(line == 0)
      {
        display.setText("  pumatx  ");
      }
      displayThrottle();
      displayPitch();
      break;
  
    case 1: //Telem
      maxLines = TELEM_LINES;
      if(line == 0)
      {
        display.setText("telem");
      }
      telemPage();   
      break;
      
    case 2: //RC Config
      maxLines = RC_LINES;
      if(line == 0)
      {
        display.setText("rc config");
      }
      rcConfigPage();
      break;
      
    case 3: //RF Config
      maxLines = RF_LINES;
      if(line == 0)
      {
        display.setText("rf config");
      }
      rfConfigPage();
      break;

    case 4: //Func stat
      maxLines = FUNC_LINES;
      if(line == 0)
      {
        display.setText("func stat");
      }
      funcStatPage();
  }
}  


void telemPage()
{
  if(line != 0)
  {
    display.setName("tlm");
  }

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
  display.setNamedRssi(1, TELEM_LINES);
  display.setText("telem p1");
}

void telemLine_2()
{
  display.setNamedRssi(2, TELEM_LINES);
  display.setText("telem p2");
}

void telemLine_3()
{
  display.setNamedRssi(3, TELEM_LINES);
  display.setText("telem p3");
}

void telemLine_4()
{
  display.setNamedRssi(4, TELEM_LINES);
  display.setText("telem p4");
}



void rcConfigPage()
{
  if(line != 0)
  {
    display.setName("rc");
  }

  switch(line)
  {
    case 1:
      rcConfigLine_1();
      break;
    case 2:
      rcConfigLine_2();
      break;
    case 3:
      rcConfigLine_3();
      break;
  }
}


void rcConfigLine_1()
{
  display.setNamedRssi(1, RC_LINES);
  display.setText("model nb");
}

void rcConfigLine_2()
{
  display.setNamedRssi(2, RC_LINES);
  display.setText("type");
}

void rcConfigLine_3()
{
  display.setNamedRssi(3, RC_LINES);
  display.setText("calib");
}



void rfConfigPage()
{
  if(line != 0)
  {
    display.setName("rf");
  }

  switch(line)
  {
    case 1:
      rfConfigLine_1();
      break;
    case 2:
      rfConfigLine_2();
      break;
    case 3:
      rfConfigLine_3();
      break;
    case 4:
      rfConfigLine_4();
      break;
    case 5:
      rfConfigLine_5(); 
      break;
    case 6:
      rfConfigLine_6();
      break;
  }
}


void rfConfigLine_1()
{
  display.setNamedRssi(1, RF_LINES);
  display.setText("protocol");
}

void rfConfigLine_2()
{
  display.setNamedRssi(2, RF_LINES);
  display.setText("rx nb");
}

void rfConfigLine_3()
{
  display.setNamedRssi(3, RF_LINES);
  display.setText("mode eu 10");
  display.setClearance(countryCode, 0);
  display.setEv(mW, 0);
}

void rfConfigLine_4()
{
  display.setNamedRssi(4, RF_LINES);
  display.setText("bind");
}

void rfConfigLine_5()
{
  display.setNamedRssi(5, RF_LINES);
  display.setText("range");
}

void rfConfigLine_6()
{
  display.setNamedRssi(6, RF_LINES);
  display.setText("flash");
}



void funcStatPage()
{
  if(line != 0)
  {
    display.setName("func");
  }

  switch(line)
  {
    case 1:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("mlx");
      display.setRpm(mlxTask.duration, 0);
      display.setHeight(mlxTask.frequency, 0);
      display.setDistance(mlxTask.inBetweenTime + mlxTask.duration, 0);
      break;
      
    case 2:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("button");
      display.setRpm(buttonTask.duration, 0);
      display.setHeight(buttonTask.frequency, 0);
      display.setDistance(buttonTask.inBetweenTime + buttonTask.duration, 0);
      break;
    
    case 3:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("rc");
      display.setRpm(rcTask.duration, 0);
      display.setHeight(rcTask.frequency, 0);
      display.setDistance(rcTask.inBetweenTime + rcTask.duration, 0);
      break;
      
    case 4:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("crsf");
      display.setRpm(crsfTask.duration, 0);
      display.setHeight(crsfTask.frequency, 0);
      display.setDistance(crsfTask.inBetweenTime + crsfTask.duration, 0);
      break;

    case 5:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("menu");
      display.setRpm(menuTask.duration, 0);
      display.setHeight(menuTask.frequency, 0);
      display.setDistance(menuTask.inBetweenTime + menuTask.duration, 0);
      break;

    case 6:
      display.setNamedRssi(line, FUNC_LINES);
      display.setText("haptic");
      display.setRpm(hapticTask.duration, 0);
      display.setHeight(hapticTask.frequency, 0);
      display.setDistance(hapticTask.inBetweenTime + hapticTask.duration, 0);
      break;
  }
}



void navigation(){
    
  if (right.state == 0 && page < PAGES && right.prev == 1 && millis() - right.currentTime > debounceDelay) 
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
  
  if(down.state == 0 && line < maxLines && down.prev == 1 && millis() - down.currentTime > debounceDelay)
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
    display.begin();
    display.off();
    display.update();
  }
}

void displayTxBattery()
{
  display.setTxBatteryPercentage(voltage.output, 1);
  display.setTxBatteryBar(map(voltage.output, -20, 80, 0, 3), 1);
}

void computeBattery()
{
  voltage.state = analogRead(voltage.pin);
  voltage.process = (-0.023 * (voltage.state * voltage.state) + 233.93 * voltage.state - 145559) / 100;  //output in mV
  
  voltage.process = 4200;
  
  voltage.output = constrain(map(voltage.process, 3500, 4190, 0, 100), 0, 100);
  if(voltage.output != voltage.previous)
  {
    voltage.previous = voltage.output;
    update++;
  }
}

void displayThrottle()
{
  display.setLeftGraph(test, 1);
}

void computeThrottle()
{
  test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 0, 7);
  if(test != lastTest)
  {
    lastTest = test;
    update++;
  }

}

void displayPitch()
{
  display.setRightGraph(testP, 1);
}

void computePitch()
{
  testP = map(pitch.output, LOWER_CHAN, UPPER_CHAN, 0, 7);
  if(testP != lastTestP)
  {
    lastTestP = testP;
    update++;
  }

}