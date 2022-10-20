#ifndef variables_h
#define variables_h

#define LOWER_CHAN  172
#define UPPER_CHAN  1811



//--------------------------------------------------Define--------------------------------------------------
//----------GPI
//--RC
#define PHOTO 38
#define LEFT_POT 36
#define RECORD 37
#define RIGHT_POT 32
#define RTH 9
#define ON_OFF 10
#define C1 23
#define C2 35
#define SPORT 21
//---Navigation
#define PAUSE 15
#define UP 5
#define RIGHT 22
#define DOWN 39
#define LEFT 19
#define OK 18

//----------GPO
#define LED 2
#define DISPLAY_BACKLIGHT 4

//----------ADC
#define BATTERY_VOLTAGE 33

//----------PWM
#define HAPTIC_PWM 12

//----------Peripherals
//---UART
#define RX_PUMATX_DEBUG 3
#define TX_PUMATX_DEBUG 1
#define CRSF 13
//---I2C
#define PUMATX_SDA 25
#define PUMATX_SCL 26
#define DISPLAY_SCL 27
#define DISPLAY_SDA 14


//--------------------------------------------------Initialize libraries--------------------------------------------------
HardwareSerial debug = HardwareSerial(0);
HardwareSerial crsf  = HardwareSerial(1);

TwoWire mlxI2C     = TwoWire(0);
TwoWire displayI2C = TwoWire(1); 

MLX               mlx(&mlxI2C, 0x0C, 0x0D);  //I2C pointer, SDA, SCL, freqI2C, Left address, Right address 
GL200ADisplay display(&displayI2C, DISPLAY_BACKLIGHT);

//--------------------------------------------------Structs--------------------------------------------------
struct Channel
{
  int reading;    //MLX reading from -10000 to 10000
  int min;    //MLX scale
  int max;    //MLX scale
  int trim;   //MLX scale
  int intermediate;  //Used to do some computation 
  int output; //from -100 to 100
  bool reverse;
};

struct Button
{
  uint8_t pin;    //which pin the button is connected to
  bool state;  //what state the button is
  int output; //from -100 to 100
  bool prev;   //previous state
  unsigned long currentTime; 
};

struct ADC
{
  uint8_t pin;    //which pin the analog device is connected to
  uint16_t state;    //What state the analog device is at
  int process;
  int output; //from -100 to 100
  unsigned long currentTime;
  int intermediate;
  int min;
  int max;
  uint16_t previous;
};

struct Func
{
  uint16_t counter;
  unsigned long beginTime;
  unsigned long startTime;
  unsigned long endTime;
  unsigned long previousTime;
  unsigned long duration;
  const    long interval;
  uint8_t frequency;
};

//--------------------------------------------------Channel struct--------------------------------------------------
Channel throttle = {
  0, //reading
  -7950, //Min
  9150,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel yaw = {
  0, //reading
  -8250, //Min
  9530,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel pitch = {
  0, //reading
  -7640, //Min
  8540,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

Channel roll = {
  0, //reading
  -8920, //Min
  8200,  //Max
  0,  //Trim
  0,  //Inter
  0,  //ouput
  0   //Reverse
};

//--------------------------------------------------Button struct--------------------------------------------------
Button right = {RIGHT, 1, 1, 1, 0};
Button left =  {LEFT, 1, 1, 1, 0};
Button up =    {UP, 1, 1, 1, 0};
Button down =  {DOWN, 1, 1, 1, 0};
Button ok =    {OK, 1, 1, 1, 0};

Button rth =   {RTH, 1, LOWER_CHAN, 1, 0};
Button play =  {PAUSE, 1, LOWER_CHAN, 1, 0};
Button pwr =   {ON_OFF, 1, 1, 1, 0};  
Button arm =   {PHOTO, 1, LOWER_CHAN, 1, 0};
Button pre =   {RECORD, 1, LOWER_CHAN, 1, 0};
Button led =   {LED, 1, 1, 1, 0};   
Button c1 =    {C1, 1, LOWER_CHAN, 1, 0};
Button c2 =    {C2, 1, LOWER_CHAN, 1, 0};
Button sport = {SPORT, 1, 1, 1, 0};

//--------------------------------------------------ADC struct--------------------------------------------------
ADC voltage =  {BATTERY_VOLTAGE, 0, 0, 0, 0, 0, 0, 0, 0}; 
ADC leftPot =  {LEFT_POT, 0, 0, 0, 0, 0, 440, 3570, 0};  
ADC rightPot = {RIGHT_POT, 0, 0, 0, 0, 0, 440, 3600, 0}; 

//--------------------------------------------------Func struct--------------------------------------------------
Func mlxTask =    {0, 0, 0, 0, 0, 0, 10, 0};
Func buttonTask = {0, 0, 0, 0, 0, 0, 20, 0};
Func rcTask =     {0, 0, 0, 0, 0, 0, 10, 0};
Func crsfTask =   {0, 0, 0, 0, 0, 0, 10, 0};
Func menuTask =   {0, 0, 0, 0, 0, 0, 50, 0};
Func hapticTask = {0, 0, 0, 0, 0, 0, 50, 0};

//--------------------------------------------------Variables--------------------------------------------------
unsigned long debounceDelay = 200;
unsigned long currentTime = 0;

uint8_t hapticDuty = 0;

volatile int16_t channels[16] = {LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN,LOWER_CHAN};

uint16_t lipo100 = 4200;
uint16_t lipo90 =  4130;
uint16_t lipo80 =  4060;
uint16_t lipo70 =  3990;
uint16_t lipo60 =  3920;
uint16_t lipo50 =  3850;
uint16_t lipo40 =  3780;
uint16_t lipo30 =  3710;
uint16_t lipo20 =  3640;
uint16_t lipo10 =  3570;
uint16_t lipo0 =   3500;

#endif
