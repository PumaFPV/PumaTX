#ifndef variables_h
#define variables_h

#define LOWER_CHAN  256
#define UPPER_CHAN  1792

//--------------------------------------------------Include libraries--------------------------------------------------
#include <Arduino.h>
#include <Wire.h>
#include "MLX.h"
#include "display.h"

//--------------------------------------------------Define--------------------------------------------------
#define I2C_SDA 4
#define I2C_SCL 13

//--------------------------------------------------Initialize libraries--------------------------------------------------
MLX mlx(0x0C, 0x0D);  //Left, Right
display display(22);

//--------------------------------------------------Structs--------------------------------------------------
struct Channel
{
    int reading;    //MLX reading from -10000 to 10000
    int min;    //MLX scale
    int max;    //MLX scale
    int trim;   //MLX scale
    int intermediate;  //Used to do some computation 
    int output; //from -100 to 100
    bool Reverse;
};

struct Button
{
    uint8_t pin;    //which pin the button is connected to
    bool state;  //what state the button is
    int output; //from -100 to 100
    bool prev;   //previous state
    unsigned long current_time; 
};

struct ADC
{
    uint8_t pin;    //which pin the analog device is connected to
    float state;    //What state the analog device is at
    int process;
    int output; //from -100 to 100
    unsigned long current_time;
    int intermediate;
};

//--------------------------------------------------Channel struct--------------------------------------------------
Channel throttle = {
  0, //reading
  -10000, //Min
  10000,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel yaw = {
  0, //reading
  -10000, //Min
  10000,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel pitch = {
  0, //reading
  -10000, //Min
  10000,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel roll = {
  0, //reading
  -10000, //Min
  10000,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

//--------------------------------------------------Button struct--------------------------------------------------
Button right = {25, 1, 1, 1, 0};
Button left = {26, 1, 1, 1, 0};
Button up = {14, 1, 1, 1, 0};
Button down = {12, 1, 1, 1, 0};
Button ok = {19, 1, 1, 1, 0};
Button rth = {18, 1, LOWER_CHAN, 1, 0};
Button play = {5, 1, LOWER_CHAN, 1, 0};
Button pwr = {27, 1, 1, 1, 0};  
Button arm = {15, 1, LOWER_CHAN, 1, 0};
Button pre = {23, 1, LOWER_CHAN, 1, 0};
Button led = {22, 1, 1, 1, 0};   

//--------------------------------------------------ADC struct--------------------------------------------------
ADC voltage = {33, 0.00, 0, 0, 0}; //GPIO35
ADC leftpot = {35, 0, 0, 0, 0};  //GPIO34
ADC rightpot = {32, 0, 0, 0, 0}; //GPIO39

//--------------------------------------------------Variables--------------------------------------------------
unsigned long debouncedelay = 200;
unsigned long currenttime = 0;

volatile int16_t channels[16] = {LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN};

float lipo100 = 4.2;
float lipo90 = 4.13;
float lipo80 = 4.06;
float lipo70 = 3.99;
float lipo60 = 3.92;
float lipo50 = 3.85;
float lipo40 = 3.78;
float lipo30 = 3.71;
float lipo20 = 3.64;
float lipo10 = 3.57;
float lipo0 = 3.5;

#endif
