
const long id = 333150;
const char * key = "12RUI1FA6G5C5CWM"; 

boolean getData() {
  if(networkOnlyMode){
      return networkVersion();
  }else{
      return rfVersion();
  }  
}
void loadValue(int8_t index,float &v,boolean &errorFlag){
  const float tmp =ThingSpeak.readFloatField(id,index, key);
  if(ThingSpeak.getLastReadStatus()!=200){
    errorFlag = true;
    return;
  } 
  v = tmp;
}
void loadValue(int8_t index,byte &v,boolean &errorFlag){
  const float tmp =ThingSpeak.readFloatField(id,index, key);
  if(ThingSpeak.getLastReadStatus()!=200){
    errorFlag = true;
    return;
  } 
  v = tmp;
}

boolean networkVersion(){
//  boolean errorFlag = false;
//  loadValue(1,tempO,errorFlag);
//  loadValue(3,humO,errorFlag);
//  loadValue(6,averangeWind,errorFlag);
//  loadValue(7,maxCurrentWind,errorFlag);
//  if(errorFlag){
//    displayConnectionError(0);
//  }   
//readTalkBackCommands();         
 // tempO = ThingSpeak.readFloatField(id,1, key);
 // humO = ThingSpeak.readFloatField(id,3, key);
 // averangeWind = ThingSpeak.readFloatField(id,6, key);
//  maxCurrentWind = ThingSpeak.readFloatField(id,7, key);
//  displayConnectionError(ThingSpeak.getLastReadStatus());
  
  if(!clockColorMode){
    displayWeatherData(true);
    display.setCursor(74,52);
    display.print(F("receiving"));
  }else{
    display.setCursor(120,52);
    display.print(F("r"));
  }
  displayA();  
String stat = "";
const char delC = 'Q';
 stat = ThingSpeak.readStatus(id, key);
 if(stat != ""){
  tempO = getValue(stat,delC,0).toFloat();
  humO = getValue(stat,delC,1).toInt();
  averangeWind = getValue(stat,delC,2).toFloat();
  maxCurrentWind = getValue(stat,delC,3).toInt();
  rain = getValue(stat,delC,4).toInt();
 }
  displayWeatherData(true);
  return true;
}



boolean rfVersion(){
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

