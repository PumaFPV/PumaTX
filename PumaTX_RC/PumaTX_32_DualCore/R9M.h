/*
 * 
R9M PXX/PCM is based on 20 bytes of data organised in the following structure:

1 - Head        - 0111 1110 - 0x7E
2 - Rx nbr      - 0000 0000 - 0x00
3 - Bind/Range  - 0000 0000 - 0x00 / 0x05 Bind / 0x20 Range
4 - Flag2       - 0000 0000
5 - chan1       - 
6 - Chan2/1     -
7 - Chan2       -
8 - Chan3       -
9 - Chan4/3     -
10- Chan4       -
11- Chan5       -
12- Chan6/5     -
13- Chan6       -
14- Chan7       -
15- Chan8/7     - 
16- Chan8       -
17- Pwr EU/US   - 0100 1000 - 0x48 EU 100mw
18- CRC         - 
19- CRC         -
20- Tail        - 0111 1110 - 0x7E

Infos for each packets:
Bind/Range: 0000 0101 bind | 0010 0000 range
Pwr/zone: EU 868: 0100 0000 10mw | 0100 1000 100mw | 0101 0000 500mw | 0101 1000 1W || US 915: 0000 0000 10mw | 0000 1000 100mw | 0001 0000 500mw | 0001 1000 1W
CRC: CRC16 0x1189
Failsafe: Not found yet

*/

#include <rom/crc.h>

#define interval_PXX  9 //9ms between each packets
unsigned long previous_millis_PXX = 0;    
   
#define HEAD 0x7E
#define BIND 0x05
#define RANGE 0x20
#define EU_100_mw 0x48
#define EU_10_mw 0x40



//-----Default values
byte receiver_number = 0x00;
byte power_zone = EU_10_mw;
byte flag1 = 0x00;
byte CRC_1 = 0x00;
byte CRC_2 = 0x00;

byte byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12 = 0;

bool pulses[400];
int length = 0;

int pcm_one_count = 0;

int pcmCrc = 0;

unsigned long interval_pxx = 8;
unsigned long previous_millis_pxx = 0;

const uint16_t CRC_Short[] =
{
   0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
   0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7
};

uint16_t CRCTable(uint8_t val)
{
  return CRC_Short[val&0x0F] ^ (0x1081 * (val>>4));
}

void crc( uint8_t data )
{
    pcmCrc = (pcmCrc<<8) ^ CRCTable((pcmCrc>>8)^data);
}


void serial_bit(bool bit){
  pulses[length++] = bit;
}

void put_pcm_part(bool bit){

  serial_bit(0);
  
  if(bit){
    serial_bit(0);
  }

  serial_bit(1);
  
}

void put_pcm_bit(bool bit){
  if(bit){
    pcm_one_count++;
    put_pcm_part(1);
  }
  else{
    pcm_one_count = 0;
    put_pcm_part(0);
  }

  if(pcm_one_count >= 5){
    serial_bit(0);
  }
}


void put_pcm_byte(byte byte){
  crc(byte);

  for(int i = 0; i < 8; i++){
    put_pcm_bit(byte & 0x80);
    byte <<= 1;  
  }
}


/*
void send_PXX(){
  for(int i = 0; i < length; i++){
    unsigned long current_millis_pxx = millis();

    if(current_millis_pxx - previous_millis_pxx >= interval_pxx){

      previous_millis_pxx = current_millis_pxx;
    
      if(1){
        GPIO.out_w1ts = ((uint32_t)1 << 17);
      }
      else{
        GPIO.out_w1tc = ((uint32_t)1 << 17);
      }
    }
    delayMicroseconds(8);
  }
}
*/


void prepare_PXX(){
  
  int chan1 = map(channels[0], -100, 100, 256, 1792);
  int chan2 = map(channels[1], -100, 100, 256, 1792);
  int chan3 = map(channels[2], -100, 100, 256, 1792);
  int chan4 = map(channels[3], -100, 100, 256, 1792);
  int chan5 = map(channels[4], -100, 100, 256, 1792);
  int chan6 = map(channels[5], -100, 100, 256, 1792);
  int chan7 = map(channels[6], -100, 100, 256, 1792);
  int chan8 = map(channels[7], -100, 100, 256, 1792);

  pcmCrc = 0;
    
  byte1  = chan1 & 0xFF;
  byte2  = (chan1>>8)|((0x0F&chan2)<<4);
  byte3  = chan2>>4;
  
  byte4  = chan3 & 0xFF;
  byte5  = (chan3>>8)|((0x0F&chan4)<<4);
  byte6  = chan4>>4;
  
  byte7  = chan5 & 0xFF;
  byte8  = (chan5>>8)|((0x0F&chan6)<<4);
  byte9  = chan6>>4;
  
  byte10 = chan7 & 0xFF;
  byte11 = (chan7>>8)|((0x0F&chan8)<<4);
  byte12 = chan8>>4;
/*
  put_pcm_byte(HEAD);
  put_pcm_byte(flag1);
  put_pcm_byte(0x00);
  
  
  put_pcm_byte(byte1);
  put_pcm_byte(byte2);
  put_pcm_byte(byte3);
  put_pcm_byte(byte4);
  put_pcm_byte(byte5);
  put_pcm_byte(byte6);
  put_pcm_byte(byte7);
  put_pcm_byte(byte8);
  put_pcm_byte(byte9);
  put_pcm_byte(byte10);
  put_pcm_byte(byte11);
  put_pcm_byte(byte12);
*/
}



void setup_pxx(byte rx, byte pwr){
  
  //Serial2.begin(125000, true);
  receiver_number = rx;
  power_zone = pwr;
  
}



void loop_pxx(){

  prepare_PXX();  //receive channels data and prepare then for PXX
  
  unsigned long current_millis_PXX = millis();

  if(current_millis_PXX - previous_millis_PXX >= interval_PXX) {
    previous_millis_PXX = current_millis_PXX;
    //send_PXX();
    
  }

}
