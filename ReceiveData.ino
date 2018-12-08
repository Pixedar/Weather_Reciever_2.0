boolean getData() {
  byte data[]  = {0, 0, 0, 0, 0, 0, 0, 0};
  int8_t index =0;
 
  while (mySerial.available()) {
    int v = mySerial.read();  
    //   Serial.print(v);
    // Serial.print(" ");
    if (index ==0&&v >= 100&& v < 200) {
      currentWind =  v-100; 
      return false;
    } else {
      data[index] =v;
      if (index < 7) {
        index++;
      } else {
        break;
      }
    }
    delay(5);
  }
//  Serial.println();

   // handleTrasmitterReset
    if(data[0] == 255){
     sendError("trasmitter has been reset");
     delay(100);
     mySerial.print(7); 
     delay(100);
     return false;  
    }
    
    if (data[0] >= 50.0f) {
      tempO= (data[0]-50) + (float)(data[1]/100.0f);
      tempO*=-1;
    } else {
      tempO= data[0] + (float)(data[1]/100.0f);
    }

    if (!realTimeWind) {
      averangeWind = data[2] + (data[3]/100.0f);
      maxCurrentWind= data[4];        
      if (index > 5) {
        if(data[5] < 100){
          rain = data[5];
          humO = 100;
        }else{
          humO = data[5]-100;
          rain =0;
        }
      }else if (index > 6) {
        rain = data[6];
      }else {     
        humO = 100;
        rain =0;
      }
      
    } else if (index > 2) {
      averangeWind = windSum/windCounter;
      windSum =0;
      windCounter =0;          
    //  humO = data[2];  // tutaj pokzaywalo deszcz zamist wilg
    if(data[2] < 100){ ///// do zmiany
      rain = data[2];
      humO = 100;
    }else{
      rain = 0;
      humO =  data[2]-100;
     if (index > 3) {
        rain = data[3];
      } else {
        rain =0;
      }
    }
    
    } else {
      humO = 100;
      rain  =0;
    }

    return true;
  
}
