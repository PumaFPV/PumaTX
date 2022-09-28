#include <Wire.h>

// lib 1 h
class Screen {
public:
  Screen(TwoWire *i2c);
  void doThings();
private:
  TwoWire *_i2c;
};



// lib 2 h
class Receptor {
public:
  Receptor(TwoWire *i2c);
  void doThings();
private:
  TwoWire *_i2c;
};


//lib 1 c
Screen::Screen(TwoWire *i2c) {
  _i2c = i2c;
}
void Screen::doThings(){
  _i2c->beginTransmission(0x01);
  _i2c->write(0xDE);
  _i2c->write(0xAD);    
  _i2c->endTransmission();    
  delay(500);
}




// lib 2 c
Receptor::Receptor(TwoWire *i2c) {
  _i2c = i2c;
}
void Receptor::doThings(){
  _i2c->beginTransmission(0x01);
  _i2c->write(0xBE);
  _i2c->write(0xEF);    
  _i2c->endTransmission();    
  delay(500);
}


// code main
void setup() {
  // put your setup code here, to run once:

  TwoWire i2c1 = TwoWire(0);
  TwoWire i2c2 = TwoWire(1); 

  Receptor receptor = Receptor(&i2c1);
  Screen screen = Screen(&i2c2);
}

void loop() {
  // put your main code here, to run repeatedly:

}