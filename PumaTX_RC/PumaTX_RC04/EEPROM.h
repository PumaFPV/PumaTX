void FirstBoot(){
  
    EEPROM.write(Throttle.EepromAddrMin, 250);    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(Throttle.EepromAddrMax, 10);
    EEPROM.write(Throttle.EepromAddrTrim, 124);
    
    EEPROM.write(Yaw.EepromAddrMin, 250);
    EEPROM.write(Yaw.EepromAddrMin, 10);
    EEPROM.write(Yaw.EepromAddrTrim, 124);
    
    EEPROM.write(Pitch.EepromAddrMin, 250);
    EEPROM.write(Pitch.EepromAddrMax, 10);
    EEPROM.write(Pitch.EepromAddrTrim, 124);
    
    EEPROM.write(Roll.EepromAddrMin, 250);
    EEPROM.write(Roll.EepromAddrMax, 10); 
    EEPROM.write(Roll.EepromAddrTrim, 124);

    EEPROM.commit();
    
}


void ReadEEPROM(){
  
  Throttle.Min = map(EEPROM.read(Throttle.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Throttle.Max = map(EEPROM.read(Throttle.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Throttle.Trim = map(EEPROM.read(Throttle.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Min = map(EEPROM.read(Yaw.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Max = map(EEPROM.read(Yaw.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Trim = map(EEPROM.read(Yaw.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Min = map(EEPROM.read(Pitch.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Max = map(EEPROM.read(Pitch.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Trim = map(EEPROM.read(Pitch.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Min = map(EEPROM.read(Roll.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Max = map(EEPROM.read(Roll.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Trim = map(EEPROM.read(Roll.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);  
  
}
