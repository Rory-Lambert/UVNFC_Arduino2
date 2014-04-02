/*Attempt at encapsulating the message preparation process into a stand-alone function*/

void NDEF_prep (byte arr[], int Nlen){
  
  int p =0;
  
  
    /****************Add the message setup*********************************/
    //fill with setup 
    for(p=0; p<=SETUP_LEN; p++){
      arr[p]=msg_setup[p];
    }
    
  
    /********************Add the mime type****************************/
    for (p; p<(SETUP_LEN+MIME_LEN); p++){
      arr[p]=mime_type[p-SETUP_LEN];
    }
   
   
    /********************Add the payload****************************/
    for (p; p<(SETUP_LEN+MIME_LEN+PAY_LEN); p++){
      arr[p]=payload[p-SETUP_LEN-MIME_LEN];
    }
  
    
     /********************Add the AAR ****************************/

    for (p; p<(SETUP_LEN+MIME_LEN+PAY_LEN+AAR_LEN); p++){
      arr[p]=aar[p-SETUP_LEN-MIME_LEN-PAY_LEN];
    }
///_MH    Serial.print("Full NDEF length (bytes): "); Serial.println(p);
    //showarray(arr, NDEF_LEN); 
    delay(1000);
    

  
    arr[LEN_BYTE_PACK] = (PACK_LEN + PAY_LEN);
    arr[LEN_BYTE_PAY] = PAY_LEN;
///_MH    Serial.print("Pack length (after setup) in bytes (loc:LEN_BYTE_PACK): "); Serial.println(arr[LEN_BYTE_PACK]);
///_MH    Serial.print("Payload length in bytes(loc:LEN_BYTE_PAY): "); Serial.println(arr[LEN_BYTE_PAY]);
///_MH    showarray(arr, p);
///_MH    showASCII(arr, p);
}
  


