
/*
 *Edited from an open-source project. 
 *Copyright 2013 Ten Wong, wangtengoo7@gmail.com  
 *  https://github.com/awong1900/RF430CL330H_Shield 
 *  RF430CL330H datasheet reference http://www.ti.com/
 */

/*********************************************************
** sample: when reset the rf430, it will write the uri to 
** rf430 tag.
***********************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include <RF430CL330H_Shield.h>
#include "UVNFC_Arduino_Header.h"
#define IRQ   (3)
#define RESET (4)  
int led = 13;  //_MH
RF430CL330H_Shield nfc(IRQ, RESET);


//Ten's variables
volatile byte into_fired = 0;
uint16_t flags = 0;


//setup the byte arrays - should be GLOBAL!
byte msg_setup[] = MSG_SETUP;  //31b
byte mime_type[] = MIME_TYPE;  //27b
byte aar[] = AAR; //33b
byte payload[25]; //= PAYLOAD;
byte payload2[] = PAYLOAD2;
byte header[11];

//_MH's variables - Global
int timer_f = 0;
int uvRaw = 0;
int ambRaw = 0;
byte uvEE = 0;
byte ambEE = 0;
int storedcount = 0;

int NFCount = 0; //***dev

static int count = 0; //***dev

unsigned static int PAY_LEN;

/*********************************SETUP******************************/
void setup(void) 
{
    
    delay(1000);     //_MH addition
  
    //Serial.begin(115200);  ///tbr
    //Serial.println("Serial connection initiated");    //tbr
    //Serial.println("NDEF message creation");  //tbr
    Wire.begin();            //FROM _MH UVNFC_EEPROM
    delay(150);
    //pinMode(led, OUTPUT);   //tbr
    //digitalWrite(led, HIGH);  //tbr
     //Pin setup
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
  
    //reset RF430    //tbr
    nfc.begin();

    
    //_MH change from delay(1000);
    delay(1000);
    
     /***TIMER INTERRUPTS***/
  //stop interrupts 
  cli();
  //set timer1 interrupt at 0.25Hz
  TCCR1A = 0;    //set entire TCCR1A register to 0
  TCCR1B = 0;    //same for TCCR1B
  TCNT1  = 0;    //initialize counter value to 0
  //set compare match register for 0.25hz increments
  OCR1A = 46874; // = (8*10^6) / (1024*(1/6s) - 1 (must be <65536)
  //turn on CTC mode
  TCCR1B |= (1 << WGM12);
  //Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  //enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //allow interrupts
  sei();
  
  //FOR TEST _MH
  pinMode(2, OUTPUT); 

    
}

//timer interrupt subroutine 
ISR(TIMER1_COMPA_vect){
     count++;
            
     if (count == (Interval*10)){
         count = 0;
         
         if(timer_f == 0){          //if the flag isnt set...
             timer_f = 1;            //...set it.
         }
     }
}
/******************************SHOWARRAY******************************/
/*
void showarray (byte arr[], int length){
  int x;
  for (x=0; x<=length; x++){
    Serial.print(arr[x],HEX);Serial.print(",");
    //if x is divisible by ten print a new line
    if ((x+1)%10==0){
      Serial.println("");
    }
    delay(100);
  }
  Serial.println("\nEND\n");
  delay(3000);
} 

void showASCII (byte arr[], int length){
  int y;
  char z;
  for (y=0; y<=length; y++){
    if (arr[y] <=64){
      z=46;
    }
    else{
      
    z=arr[y];
    }
    Serial.print(z);
  }
}

  
*/
/******************************MAIN*********************************/



void loop(void) {
    

    
    
///_MH    Serial.println("Wait for read or write...");
    while(1)
    {
        if(into_fired)
        {
          
            
            //clear control reg to disable RF
            nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE); 
            delay(750);
            
            //read the flag register to check if a read or write occurred
            flags = nfc.Read_Register(INT_FLAG_REG); 
///_MH            Serial.print("INT_FLAG_REG = 0x");Serial.println(flags, HEX);
            
            //ACK the flags to clear
            nfc.Write_Register(INT_FLAG_REG, EOW_INT_FLAG + EOR_INT_FLAG); 
            
            if(flags & EOW_INT_FLAG)      //check if the tag was written
            {
///_MH                Serial.println("The tag was writted!");
///_MH                digitalWrite(led, HIGH);
            }
            else if(flags & EOR_INT_FLAG) //check if the tag was read
            {
///_MH                Serial.println("The tag was readed!");
///_MH                digitalWrite(led, LOW);
                      //RESET EEPROM POINTER
            }
            flags = 0;
            into_fired = 0; //we have serviced INT1

            //Enable interrupts for End of Read and End of Write
            nfc.Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

            //Configure INTO pin for active low and re-enable RF
            nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE);

            //re-enable INTO
            attachInterrupt(1, RF430_Interrupt, FALLING);
        }
        
        /****************************************OUR STUFF HERE***********************/
        
        else{
        
          if (timer_f==1){
            timer_f=0;
            NFCount++;
            /*
            //uvRaw = analogRead(A0);
            StoreData(ee_address, NFCount );
            //ambRaw = analogRead(A1);
            StoreData(ee_address, (NFCount+30));
            //uvEE = EepromRead(0x03);
            //ambEE = EepromRead(0x04);
            */
            }          
          
            int z;
            for (z=65; z<89; z++){
              StoreData(ee_address, (z+NFCount));
            }
            
            
            //payload[0] = count;
            //payload[1]=NFCount;
            ReadAllData();
            PAY_LEN=sizeof(payload);                    //find the length of the payload
       
            /*sets the length of the NDEF message, depending upon the payload size*/
            byte NDEF_MSG[PAY_LEN + PRE_PAY_LEN-1];     
            int NDEF_LEN = sizeof(NDEF_MSG);            //store its length in an int
            
            //Function call prepares the full NDEF message
            NDEF_prep(NDEF_MSG, PAY_LEN);    

    
      
      
/******************************TENWONG*********************************/    
      
   
    
    
   
    while(!(nfc.Read_Register(STATUS_REG) & READY)); //wait until READY bit has been set
///_MH    Serial.print("Fireware Version:"); Serial.println(nfc.Read_Register(VERSION_REG), HEX);    

    //write NDEF memory with Capability Container + NDEF message
    nfc.Write_Continuous(0, NDEF_MSG, (sizeof(NDEF_MSG)+1));

    //Enable interrupts for End of Read and End of Write
    nfc.Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

    //Configure INTO pin for active low and enable RF
    nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE );

    //enable interrupt 1
    attachInterrupt(1, RF430_Interrupt, FALLING);
        ////
        
        }
    }
}

/**
**  @brief  interrupt service
**/
void RF430_Interrupt(){
    into_fired = 1;
    detachInterrupt(1);//cancel interrupt
}



