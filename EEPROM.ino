/************************************EEPROM FUNCTIONS**************************/
/*                                                                            */
/******************************************************************************/





/*****************************WRITE************************************/

//writes one byte of data to stated address in memory
void EepromWrite(int address, byte data){
  
  //split new 16 bit address into high and low bytes
  byte add_lo = address;
  byte add_hi = (address >> 8);
  
  //start transmission & send control byte
  Wire.beginTransmission(eeprom_cntrl);
  //send address high byte
  Wire.write(add_hi);
  //send address low byte
  Wire.write(add_lo);
  //send data byte
  Wire.write(data);
  //end transmission
  Wire.endTransmission();
  delay(5);
  
} 

/*********************************READ***********************************/

//reads one byte of data from stated address in memory
byte EepromRead(int address){
  
  //split 16 bit address into high and low bytes
  byte add_lo = address;
  byte add_hi = (address >> 8);
    
  
  //start transmission & send control byte
  Wire.beginTransmission(eeprom_cntrl);
  //send address high byte
  Wire.write(add_hi);
  //send address low byte
  Wire.write(add_lo);
  //end write
  Wire.endTransmission();
  delay(5);
  //read one byte from eeprom
  Wire.requestFrom(eeprom_cntrl, 1);
  //wait for receipt
  while(Wire.available() == 0);
  //assign value
  byte receivedValue = Wire.read();
  
  return receivedValue;
}


/*************************************STOREDATA**********************************/

/**USE THIS FUNCTION TO WRITE DATA TO EEPROM**/
void StoreData(int address, int data){

  //scale value from adc from 10 bits to 8 bits
  unsigned long int scaledData = (data);    //***removed scaling factor
  byte newdata = (byte)scaledData;
  
  EepromWrite(address, newdata);    //Tx data_hi byte
  ee_address++;                        //increment eeprom address
  storedcount++;                    //increment global storedcount
  
  //update counter value stored in eeprom
  UpdateCounter();
}


/***************************UPDATE COUNTER**************************************/

void UpdateCounter(){
  /* function for keeping track of number of 
  *  values in eeprom and writing that value into 
  *  first 2 memory locations in eeprom        */
  
  //split storedcount int into two 8 bit values
  byte count_lo = storedcount;
  byte count_hi = (storedcount >> 8);
  
  //Update Counter Value in eeprom addresses 0x00 and 0x01
  EepromWrite(0x00, count_hi);    //tx count_hi byte
  EepromWrite(0x01, count_lo);    //tx count_lo byte

}

/****************************READALLDATA******************************************/

void ReadAllData(){
  /* function for reading all of eeprom */
  byte receivedValue = 0;
  byte receivedBuffer[1024];
  
  //get count bytes from eeprom and convert to integer
  byte count_hi = EepromRead(0x00);
  byte count_lo = EepromRead(0x01);
  int countEE = count_lo | (count_hi << 8);
  
  /*READ storedcount NUMBER OF VALUES FROM EEPROM
    AND STORE SEQUENTIALLY IN A BUFFER*/
  
  int address = 0x03;        //start at first address
  int y = 0;
  for (y=0; y<countEE; y++){
    
    receivedValue = EepromRead(address);    //read hi data value
    address++;                                 //increment eeprom address
    
    //convert to int and store in buffer
    payload[y] = receivedValue;
    
  }

  storedcount = 0;        //reset global storedcount
  ee_address = 0x02;      //reset global eeprom address
  
}
