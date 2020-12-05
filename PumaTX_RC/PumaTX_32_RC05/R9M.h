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

#define PXX_CHANNEL_WIDTH 2048
#define PXX_UPPER_LOW     2049
#define PXX_UPPER_HIGH    4094
#define PXX_LOWER_LOW     1
#define PXX_LOWER_HIGH    2046



#define HEAD 0x7E //0x7C
#define BIND 0x05
#define RANGE 0x20
#define EU_100_mw 0x48  // 0100 1000
#define EU_10_mw 0x40   // 0100 0000



//-----Default values
byte receiver_number = 0x00;
byte power_zone = EU_10_mw;
byte flag1 = 0x00;
byte CRC_1 = 0x00;
byte CRC_2 = 0x00;

byte byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12 = 0;

bool pulses[64];
int length = 0;
        
bool send_upper_channel;

int pcm_ones_count = 0;

int pcm_crc;

unsigned long interval_pxx = 9;
unsigned long previous_millis_pxx = 0;

unsigned long interval_mlx = 5;
unsigned long previous_millis_mlx = 0;

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
    pcm_crc = (pcm_crc<<8) ^ CRCTable((pcm_crc>>8)^data);
}


void serial_bit(bool bit)
{
  if(bit)
  {
    GPIO.out_w1ts = ((uint32_t)1 << 17);
    delayMicroseconds(7);
  }
  else
  {
    GPIO.out_w1tc = ((uint32_t)1 << 17);
    delayMicroseconds(7);    
  }
}

void put_pcm_part(bool bit)
{

  serial_bit(0);
  
  if(bit)
  {
    serial_bit(0);
  }

  serial_bit(1);
  
}

void put_pcm_bit(bool bit)
{
  if(pcm_ones_count >= 5)
  {
    put_pcm_part(0);
  }
  if(bit)
  {
    pcm_ones_count++;
    put_pcm_part(1);
  }
  else
  {
    pcm_ones_count = 0;
    put_pcm_part(0);
  }

}


void put_pcm_byte(uint8_t byte)
{
  crc(byte);

  for(uint8_t i = 0; i < 8; i++)
  {
    put_pcm_bit(byte & 0x80);
    byte <<= 1;  
  }
}


void put_pcm_head()
{
    // send 7E, do not CRC
    // 01111110
    put_pcm_part(0);
    put_pcm_part(1);
    put_pcm_part(1);
    put_pcm_part(1);
    put_pcm_part(1);
    put_pcm_part(1);
    put_pcm_part(1);
    put_pcm_part(0);
}

uint16_t limit(uint16_t low, uint16_t val, uint16_t high)
{
    if(val < low)
    {
        return low;
    }

    if(val > high)
    {
        return high;
    }

    return val;
}


void prepare_pxx(volatile int16_t channels[16], byte rx_number, byte bind, byte power_zone)
{
    uint16_t chan=0, chan_low=0;

    length = 0;
    pcm_crc = 0;
    pcm_ones_count = 0;

    /* preamble */
    //putPcmPart(0);
    //putPcmPart(0);
    //putPcmPart(0);
    //putPcmPart(0);

    /* Sync */
    put_pcm_part(0);
    put_pcm_head();

    // Rx Number
    put_pcm_byte(rx_number);

    // FLAG1 - Fail Safe Mode, nothing currently set, maybe want to do this
    put_pcm_byte(bind);

    // FLAG2
    put_pcm_byte(0x00);
   
    if(send_upper_channel)
    {
      put_pcm_byte(channels[0] & 0xFF);
      put_pcm_byte((channels[0] >> 8) | ((0x0F & channels[1]) << 4));
      put_pcm_byte(channels[1] >> 4);
      put_pcm_byte(channels[2] & 0xFF);
      put_pcm_byte((channels[2] >> 8) | ((0x0F & channels[3]) << 4));
      put_pcm_byte(channels[3] >> 4);
      put_pcm_byte(channels[4] & 0xFF);
      put_pcm_byte((channels[4] >> 8) | ((0x0F & channels[5]) << 4));
      put_pcm_byte(channels[5] >> 4);
      put_pcm_byte(channels[6] & 0xFF);
      put_pcm_byte((channels[6] >> 8) | ((0x0F & channels[7]) << 4));
      put_pcm_byte(channels[7] >> 4);
    }
    else
    {
      put_pcm_byte((channels[8] + 2048) & 0xFF);
      put_pcm_byte(((channels[8] + 2048) >> 8) | ((0x0F & (channels[9] + 2048)) << 4));
      put_pcm_byte((channels[9] + 2048) >> 4);
      put_pcm_byte((channels[10] + 2048) & 0xFF);
      put_pcm_byte(((channels[10] + 2048) >> 8) | ((0x0F & (channels[11] + 2048)) << 4));
      put_pcm_byte((channels[11] + 2048) >> 4);
      put_pcm_byte((channels[12] + 2048) & 0xFF);
      put_pcm_byte(((channels[12] + 2048) >> 8) | ((0x0F & (channels[13]+ 2048)) << 4));
      put_pcm_byte((channels[13] + 2048) >> 4);
      put_pcm_byte((channels[14] + 2048) & 0xFF);
      put_pcm_byte(((channels[14] + 2048) >> 8) | ((0x0F & (channels[15] + 2048)) << 4));
      put_pcm_byte((channels[15] + 2048) >> 4);      
    }

    
    put_pcm_byte(power_zone); //10mw EU

    // CRC16
    chan = pcm_crc;
    put_pcm_byte(chan>>8);
    put_pcm_byte(chan);

    // Sync
    put_pcm_head();
    GPIO.out_w1tc = ((uint32_t)1 << 17);
    send_upper_channel = !send_upper_channel;
  
}
