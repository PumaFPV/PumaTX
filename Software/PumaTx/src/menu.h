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
  int test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 1, 6);
  display.setLeftGraph(test, 1);
  display.update();
  /*
  switch(page)
  {
    case 0:
      Serial.println("  pumatx  ");
      //display.setText("  pumatx  ");
      int test = map(throttle.output, LOWER_CHAN, UPPER_CHAN, 1, 6);
      display.setLeftGraph(test, 1);
      display.update();
      break;
   
    case 1: //Telem
      Serial.println("telem");   
      display.setText("telem");
      display.update();
      if(ok.state)  //if selected
      {
        telemPage();   
      }
      break;
      
    case 2: //RC Config
      Serial.println("rc config");
      display.setText("rc config");
      display.update();
      rcConfigPage();
      break;
      
    case 3: //RF Config
      Serial.println("rf config");    
      display.setText("rf config");
      display.update();
      rfConfigPage();
      break;
  }
  */
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
  Serial.println("telem page 1");
  display.setName("line");
  display.setNamedRssi(1, 4);
  display.setText("telem page 1");
}

void telemLine_2()
{
  Serial.println("telem page 2");
  display.setName("line");
  display.setNamedRssi(2, 4);
  display.setText("telem page 2");
}

void telemLine_3()
{
  Serial.println("telem page 3");
  display.setName("line");
  display.setNamedRssi(3, 4);
  display.setText("telem page 3");
}

void telemLine_4()
{
  Serial.println("telem page 4");
  display.setName("line");
  display.setNamedRssi(4, 4);
  display.setText("telem page 4");
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
  Serial.println("rf_config_line_1");
  display.setName("line");
  display.setNamedRssi(1, 5);
  display.setText("rf1");
}

void rfConfigLine_2()
{
  Serial.println("rf_config_line_2");
  display.setName("line");
  display.setNamedRssi(2, 5);
  display.setText("rf2");
}

void rfConfigLine_3()
{
  Serial.println("rf_config_line_3");
  display.setName("line");
  display.setNamedRssi(3, 5);
  display.setText("rf3");
}

void rfConfigLine_4()
{
  Serial.println("rf_config_line_4");
  display.setName("line");
  display.setNamedRssi(4, 5);
  display.setText("rf4");
}

void rfConfigLine_5()
{
  Serial.println("rf_config_line_5");
  display.setName("line");
  display.setNamedRssi(5, 5);
  display.setText("rf5");
}

void navigation(){
    
  if (right.state == 0 && page < 3) 
  { //menu right -> page+
    ++page;
    line = 0;
    delay(50);
  }

  if (left.state == 0 && page > 0)
  { //menu left -> page-
    --page;
    line = 0;
    delay(50);
  }
  
  if(down.state == 0 && line < 5)
  {
    line++;
    delay(50);
  }

  if(up.state == 0 && line > 0)
  {
    line--;
    delay(50);
  }

  if(play.state == 0)
  {
    page = 0;
    line = 0;
  }
}
