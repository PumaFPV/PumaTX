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

void menu_loop();

void telem_page();
void telem_line_1();
void telem_line_2();
void telem_line_3();
void telem_line_4();

void rc_config_page();
void rc_config_line_1();
void rc_config_line_2();
void rc_config_line_3();

void rf_config_page();
void rf_config_line_1();
void rf_config_line_2();
void rf_config_line_3();
void rf_config_line_4();
void rf_config_line_5();

void navigation();

void menu_loop()
{
  switch(page)
  {
    case 0:
      Serial.println("  pumatx  ");
      display.set_text("  pumatx  ");
      display.update();
      break;
   
    case 1: //Telem
      Serial.println("telem");   
      display.set_text("telem");
      display.update();
      if(ok.state)  //if selected
      {
        telem_page();   
      }
      break;
      
    case 2: //RC Config
      Serial.println("rc config");
      display.set_text("rc config");
      display.update();
      rc_config_page();
      break;
      
    case 3: //RF Config
      Serial.println("rf config");    
      display.set_text("rf config");
      display.update();
      rf_config_page();
      break;
  }
}



void telem_page()
{
  switch(line)
  {
    case 1:
      telem_line_1();
      break;
      
    case 2:
      telem_line_2();
      break;
      
    case 3:
      telem_line_3();
      break;
      
    case 4:
      telem_line_4();
      break;
  }
}

void telem_line_1()
{
  Serial.println("telem page 1");
  display.set_name("line");
  display.set_named_rssi(1, 4);
  display.set_text("telem page 1");
}

void telem_line_2()
{
  Serial.println("telem page 2");
  display.set_name("line");
  display.set_named_rssi(2, 4);
  display.set_text("telem page 2");
}

void telem_line_3()
{
  Serial.println("telem page 3");
  display.set_name("line");
  display.set_named_rssi(3, 4);
  display.set_text("telem page 3");
}

void telem_line_4()
{
  Serial.println("telem page 4");
  display.set_name("line");
  display.set_named_rssi(4, 4);
  display.set_text("telem page 4");
}



void rc_config_page()
{
  switch(line)
  {
    case 1:
      rc_config_line_1();
    case 2:
      rc_config_line_2();
    case 3:
      rc_config_line_3();
  }
}

void rc_config_line_1()
{
  display.set_name("line");
  display.set_named_rssi(1, 3);
  display.set_text("rc1");
}

void rc_config_line_2()
{
  display.set_name("line");
  display.set_named_rssi(2, 3);
  display.set_text("rc2");
}

void rc_config_line_3()
{
  display.set_name("line");
  display.set_named_rssi(3, 3);
  display.set_text("rc3");
}



void rf_config_page()
{
  Serial.print("line: ");
  Serial.println(line);

  switch(line)
  {
    case 1:
      rf_config_line_1();
    case 2:
      rf_config_line_2();
    case 3:
      rf_config_line_3();
    case 4:
      rf_config_line_4();
    case 5:
      rf_config_line_5(); 
  }
}

void rf_config_line_1()
{
  Serial.println("rf_config_line_1");
  display.set_name("line");
  display.set_named_rssi(1, 5);
  display.set_text("rf1");
}

void rf_config_line_2()
{
  Serial.println("rf_config_line_2");
  display.set_name("line");
  display.set_named_rssi(2, 5);
  display.set_text("rf2");
}

void rf_config_line_3()
{
  Serial.println("rf_config_line_3");
  display.set_name("line");
  display.set_named_rssi(3, 5);
  display.set_text("rf3");
}

void rf_config_line_4()
{
  Serial.println("rf_config_line_4");
  display.set_name("line");
  display.set_named_rssi(4, 5);
  display.set_text("rf4");
}

void rf_config_line_5()
{
  Serial.println("rf_config_line_5");
  display.set_name("line");
  display.set_named_rssi(5, 5);
  display.set_text("rf5");
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
}
