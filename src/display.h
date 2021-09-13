/*
	This lib allows to controll mavic pro controller GL200A display with any arduino compatible microcontroller
	It uses the I2C bus at address 0x38
*/

#ifndef display_h
#define display_h

#include "Arduino.h"

class display
{
	public:
		display(uint8_t BL_pin);
		void begin();
		void update();
		
		void set_left_graph(uint8_t bar, bool pic);
		void set_right_graph(uint8_t bar, bool pic);
		void set_rc_rssi(uint8_t bar, bool pic);
		void set_named_rssi(uint8_t bar, bool pic);
		void set_tx_battery_bar(uint8_t bar, bool pic);
		void set_tx_battery_percentage(uint8_t percentage, bool pic);
		void set_drone_battery_bar(uint8_t bar, bool pic);
    	void set_drone_battery_percentage(uint8_t percentage, bool pic);  //drone battery percentage middle top
    
		void set_rpm(int rpm, bool pic); 
		void set_speed(int speed, bool pic);
		void set_distance(int distance, bool pic);
		void set_altitude(int altitude, bool pic);
		void set_clearance(int clearance, bool pic);
		void set_ev(int ev, bool pic);
		void set_sd(bool sd);
		void set_sport(bool sport);
		void set_vision(bool vision);
		void set_rec(bool rec);
		void set_text(String text, int scroll_speed);
    void set_text(String text);
		void set_name(String text, int scroll_speed);
    void set_name(String text);
    
		void display_default();  //display 0 everywhere

	private:
		byte display_byte[68];
		
		byte char_to_7_segment(char digit);
		int char_to_14_segment(int chara);	
		
		void draw_tx_battery_percentage(uint8_t display, char digit);  //draw to individual digit of battery percentage
		void draw_drone_battery_percentage(uint8_t display, char digit);  //draw to individual digit of drone percentage
		void draw_rpm(uint8_t display, char digit);  //draw to individual digit of rpm
		void draw_speed(uint8_t display, char digit);  //draw to individual digit of speed
		void draw_distance(uint8_t display, char digit);  //draw to individual digit of distance
		void draw_altitude(uint8_t display, char digit);  //draw to individual digit of altitude
		void draw_clearance(uint8_t display, char digit);  //draw to individual digit of clearance
		void draw_ev(uint8_t display, char digit); //draw to individual digit of ev 
		void draw_text(int display, int chara);	//draw to individual digit of 14 segment displays
		void draw_name(int display, int chara);	//draw to individual digit of 14 segment displays
		
    uint8_t _BL_pin;
};

#endif
