#ifndef variables_h
#define variables_h



//--------------------------------------------------Include libraries--------------------------------------------------
#include <Arduino.h>
#include <Wire.h>
//#include <MLX.h>
#include "dsm2_tx.h"


//--------------------------------------------------Define--------------------------------------------------
#define I2C_SDA 4
#define I2C_SCL 13
//--------------------------------------------------Initialize libraries--------------------------------------------------
//MLX mlx(0x0C, 0x0D);



//--------------------------------------------------Structs--------------------------------------------------
struct Channel{
    int Reading;    //MLX reading from -10000 to 10000
    int Min;    //MLX scale
    int Max;    //MLX scale
    int Trim;   //MLX scale
    int Inter;  //Used to do some computation 
    int Output; //from -100 to 100
    bool Reverse;
};

struct Button{
    uint8_t Pin;    //which pin the button is connected to
    bool State;  //what state the button is
    int Output; //from -100 to 100
    bool Prev;   //Previous state
    unsigned long current_time; 
};

struct ADC{
    uint8_t Pin;    //which pin the analog device is connected to
    float State;    //What state the analog device is at
    int Process;
    int Output; //from -100 to 100
    unsigned long current_time;
};



//--------------------------------------------------Channel struct--------------------------------------------------
Channel Throttle = {0, //Reading
                    -10000, //Min
                    10000,  //Max
                    0,  //Trim
                    0,  //Inter
                    0,  //Output
                    0   //Reverse
                    };

Channel Yaw = {0, //Reading
                    -10000, //Min
                    10000,  //Max
                    0,  //Trim
                    0,  //Inter
                    0,  //Output
                    0   //Reverse
                    };

Channel Pitch = {0, //Reading
                    -10000, //Min
                    10000,  //Max
                    0,  //Trim
                    0,  //Inter
                    0,  //Output
                    0   //Reverse
                    };

Channel Roll = {0, //Reading
                    -10000, //Min
                    10000,  //Max
                    0,  //Trim
                    0,  //Inter
                    0,  //Output
                    0   //Reverse
                    };



//--------------------------------------------------Button struct--------------------------------------------------
Button Right = {25, 1, 1, 1, 0};
Button Left = {26, 1, 1, 1, 0};
Button Up = {14, 1, 1, 1, 0};
Button Down = {12, 1, 1, 1, 0};
Button Ok = {19, 1, 1, 1, 0};
Button RTH = {18, 1, -100, 1, 0};
Button Pause = {5, 1, -100, 1, 0};
Button Pwr = {27, 1, 1, 1, 0};  
Button Arm = {15, 1, -100, 1, 0};
Button Pre = {23, 1, -100, 1, 0};
Button LED = {22, 1, 1, 1, 0};   

//--------------------------------------------------ADC struct--------------------------------------------------
ADC Voltage = {33, 0.00, 0, 0}; //GPIO35
ADC LeftPot = {32, 0, 0, 0};  //GPIO34
ADC RightPot = {35, 0, 0, 0}; //GPIO39



//--------------------------------------------------Variables--------------------------------------------------
String Firmware;

unsigned long debouncedelay = 200;
unsigned long currenttime = 0;

int channels[10];

uint8_t page = 0;

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
