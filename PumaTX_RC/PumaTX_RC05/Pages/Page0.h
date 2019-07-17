void Page0(){
	
	u8g2.drawXBMP( 0, 0, 60, 64, Puma);
  
	u8g2.setFont(u8g2_font_fub17_tr);
	u8g2.setCursor(63, 27);
	u8g2.print("Puma");
	u8g2.setFont(u8g2_font_fub25_tr);
	u8g2.setCursor(64, 54);
	u8g2.print("TX");
  
}