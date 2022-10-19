/*
	This lib allows to controll mavic pro controller GL200A display with any arduino compatible microcontroller
	It uses the I2C bus at address 0x38
*/

#ifndef display_h
#define display_h

#include "Arduino.h"
#include <Wire.h>

class GL200ADisplay
{
	public:
		GL200ADisplay(TwoWire *displayI2C, uint8_t BlPin);
		void begin();
		void update();
		
		void setLeftGraph(uint8_t bar, bool pic);
		void setRightGraph(uint8_t bar, bool pic);
		void setRcRssi(uint8_t bar, bool pic);
		void setNamedRssi(uint8_t bar, uint8_t pic);
		void setTxBatteryBar(uint8_t bar, bool pic);
		void setTxBatteryPercentage(uint8_t percentage, bool pic);
		void setDroneBatteryBar(uint8_t bar, bool pic);
    	void setDroneBatteryPercentage(uint8_t percentage, bool pic);  //drone battery percentage middle top
    
		void setRpm(int rpm, bool pic); 
		void setSpeed(int speed, bool pic);
		void setDistance(int distance, bool pic);
		void setAltitude(int altitude, bool pic);
		void setClearance(int clearance, bool pic);
		void setEv(int ev, bool pic);
		void setSd(bool sd);
		void setSport(bool sport);
		void setVision(bool vision);
		void setRec(bool rec);
		void setText(String text, int scrollSpeed);
    		void setText(String text);
		void setName(String text, int scrollSpeed);
   			void setName(String text);
    
		void displayDefault();  //display 0 everywhere

	private:
		TwoWire *_displayI2C;

		byte displayByte[68];
		
		byte charTo7Segment(char digit);
		int charTo14Segment(int chara);	
		
		void drawTxBatteryPercentage(uint8_t display, char digit);  //draw to individual digit of battery percentage
		void drawDroneBatteryPercentage(uint8_t display, char digit);  //draw to individual digit of drone percentage
		void drawRpm(uint8_t display, char digit);  //draw to individual digit of rpm
		void drawSpeed(uint8_t display, char digit);  //draw to individual digit of speed
		void drawDistance(uint8_t display, char digit);  //draw to individual digit of distance
		void drawAltitude(uint8_t display, char digit);  //draw to individual digit of altitude
		void drawClearance(uint8_t display, char digit);  //draw to individual digit of clearance
		void drawEv(uint8_t display, char digit); //draw to individual digit of ev 
		void drawText(int display, int chara);	//draw to individual digit of 14 segment displays
		void drawName(int display, int chara);	//draw to individual digit of 14 segment displays
		
    	uint8_t _BlPin;
};

#endif
