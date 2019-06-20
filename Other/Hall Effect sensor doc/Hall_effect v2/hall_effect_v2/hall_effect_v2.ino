//--------------------------include------------------------------------------------------
#include <Wire.h>
#include <MLX90393.h> 

MLX90393 rmlx;
MLX90393::txyz rdata; //Create a structure, called data, of four floats (t, x, y, and z)

MLX90393 lmlx;
MLX90393::txyz ldata;

//--------------------------setup--------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  Serial.println("MLX90393 Read Example");
  Wire.begin();
  rmlx.begin(0x0D);
  lmlx.begin(0x0C);
  
  //byte status = rmlx.begin();

  //Report status from configuration
  //Serial.print("Start status: 0x");
  //if(status < 0x0C) Serial.print("0"); //Pretty output
  //Serial.println(status, BIN);
  
  //rmlx.setGainSel(1);
  //rmlx.setResolution(0, 0, 0); //x, y, z
  rmlx.setOverSampling(0);
  rmlx.setDigitalFiltering(0);
  //See MLX90393.h and .cpp for additional functions including:
  //set/getOverSample, set/getTemperatureOverSample, set/getDigitalFiltering, set/getResolution
  //set/getTemperatureCompensation, setOffsets, setWThresholds
}

void loop()
{
  rmlx.readData(rdata); //Read the values from the sensor

  Serial.print("magX[");
  Serial.print(rdata.x);
  Serial.print("] magY[");
  Serial.print(rdata.y);
  Serial.print("] magZ[");
  Serial.print(rdata.z);
  Serial.print("] temperature(C)[");
  Serial.print(rdata.t);
  //Serial.print("] status[");
  //Serial.print(status);
  Serial.print("]");

  lmlx.readData(ldata); //Read the values from the sensor

  Serial.print("magX[");
  Serial.print(ldata.x);
  Serial.print("] magY[");
  Serial.print(ldata.y);
  Serial.print("] magZ[");
  Serial.print(ldata.z);
  Serial.print("] temperature(C)[");
  Serial.print(ldata.t);
  //Serial.print("] status[");
  //Serial.print(status);
  Serial.print("]");

  Serial.println();

  delay(1000);
}
