//float absoluteMaxTemp = -100;
//float absoluteMinTemp = 100;
float highestTemperatureEverRecorded =-100.0f;
float lowestTemperatureEverRecorded =100.0f;
float absoluteMaxTemp = 39.65f;
float absoluteMinTemp = 9.66f;
unsigned long sendAutoRangeTime;
const char * autoRangeApiKey = "T44K8QM9YHVD73WG";
const char * autoRangeApiKey2 = "VWNHBS4PEAS1W29I";
const unsigned long tempRangeChannelId = 639828;
//float maximaScale = 0.000000832f;
float maximaScale = 0.000000835f;
const unsigned long sendAutoRangeInterval= 18000000;
const char separator = 'p';
const int8_t NumberOfConnectionAttempts = 1;
const int8_t saveToEEPROMInterval = 5;
int8_t saveToEEPROMCounter = 0;

int humShtCtn = 0;
int tempShtCtn = 0;
int8_t autoRangeLoadingBarProgress =0;
#define SHT_MAXIMA_DURATION 600000l
#define SCALE_A 180000.0f
#define SCALE_B 2.7f
#define OUT_OF_RANGE 50

void updateLoadingBar(){
    autoRangeLoadingBarProgress++;
    drawLoadingBar(map(autoRangeLoadingBarProgress,0,8,0,128),false);
}
boolean setOutTempRange(int &index){
  String str;
  String tmp;
  float val;
  int d;
 str = getCreatedAt(NumberOfConnectionAttempts);
  str = getValue(str,'T',0);
  if(str.equals(F(""))){
    return false;
  }
 tmp = getValue(str,'-',1);
 if(tmp.charAt(0) == '0'){
  tmp = String(tmp.charAt(1));
 }
 val = tmp.toInt();
 tmp = getValue(str,'-',2);
 if(tmp.charAt(0) == '0'){
  tmp = String(tmp.charAt(1));
 }
 
  d = tmp.toInt();

  if(isnan(d)&&!isnan(val)&& val <=-1&& d <=-1&&val > 12&& d > 33){
    return false;
  }
  //displayDot();
  updateLoadingBar();
  if(val >-1&&val<=12&&(_month == (int)val||_month == (int)(val-1))){
    str = getEntry(NumberOfConnectionAttempts,2);
     if(str.equals(F(""))){
        return false;
      }
      val = getValue(str,separator,0).toFloat();
      if(!isnan(val)&&val<40&&val>-30){
        absoluteMaxTemp = val;
      }else{
        return false;
      }
      display.print(String(absoluteMaxTemp) + " ");
      val = getValue(str,separator,1).toFloat();
      if(!isnan(val)&&val<40&&val>-30){
        absoluteMinTemp = val;
      }else{
        return false;
      }
      display.println(String(absoluteMinTemp));
      displayA();
    if(_month == (int)(val-1)&&d<=29){
      absoluteMaxTemp = absoluteMaxTemp*0.45f;
      absoluteMinTemp = absoluteMinTemp*0.45f;
    }
   // displayDot();
   updateLoadingBar();
  }
  EEPROM.put(index,d);
  index+=sizeof(uint8_t);
  EEPROM.put(index,absoluteMaxTemp);
  index+=sizeof(float);
  EEPROM.put(index,absoluteMinTemp );
  index+=sizeof(float);
  _max[0]=absoluteMaxTemp;
  _min[0] =absoluteMinTemp;
  
  return true;
}

boolean loadAutoRangeFromServer(){
  int index = 300; 
  if(!setOutTempRange(index))return false; //2
  if(!setRange(maxPressureChange,minPressureChange,3,index))return false;
  if(!setRange(maxInsideTemp,minInsideTemp,4,index))return false;
  if(!setRange(maxInsideHum,minInsideHum,5,index))return false;
  if(!setRange(maxPressure,minPressure,6,index))return false;
  if(!setRange(maxAverangeWind,maxWind,7,index))return false;
  if(!setRange(maxRain,minOutsideHum,8,index))return false;
  return true;
}
void initAutoRange(){ 
  EEPROM.begin(512);
  delay(5);
  
  if(!loadAutoRangeFromEEPROM()&&!checkLock()){
      EEPROM.end();
   // clearDisplay();
 //   display.println("AutoRange mem fail");
  //  displayA();
    EEPROM.begin(512);
        delay(3);
      if(!loadAutoRangeFromServer()){
      //   clearDisplay();
         display.print(" ERROR");
         displayA();
         delay(500);
      }     
  }
  sendAutoRangeTime =millis(); 
 // maxPressure[0] = 1000; //// temporary fix!!!
  EEPROM.end();
  delay(3);
  EEPROM.begin(512);
  if(checkLock()){
      EEPROM.put(180,1);
  }
  EEPROM.end();

  _max[0]=absoluteMaxTemp;
  _min[0] =absoluteMinTemp;
  clearLoadingBar();
}
boolean checkLock(){
  byte lock;
  EEPROM.get(180,lock);
  if(lock == (byte)0){
     EEPROM.get(170,lock);
     if(lock == (byte)1){
        EEPROM.put(170,0);
        Serial.println("locked");
        return true;
     }
  }
    return false;
}
void saveAutoRangeToEEPROM(){

  EEPROM.begin(512);
  delay(5);
   int index = 300; 
  EEPROM.put(index,_day);
  index+=sizeof(uint8_t);
  EEPROM.put(index,absoluteMaxTemp);
  index+=sizeof(float);
  EEPROM.put(index,absoluteMinTemp );
  index+=sizeof(float);
  saveEntryToEEPROM(maxPressureChange,minPressureChange,index);
  saveEntryToEEPROM(maxInsideTemp,minInsideTemp,index);
  saveEntryToEEPROM(maxInsideHum,minInsideHum,index);
  saveEntryToEEPROM(maxPressure,minPressure,index);
  saveEntryToEEPROM(maxAverangeWind,maxWind,index);
  saveEntryToEEPROM(maxRain,minOutsideHum,index);
 // saveEntryToEEPROM(highestTemperatureEverRecorded,lowestTemperatureEverRecorded,index);
  EEPROM.end();
}
void saveEntryToEEPROM(float maxVal[],float minVal[],int &index2){
    EEPROM.put(index2,maxVal[0]);
    index2+=sizeof(float);
    EEPROM.put(index2,minVal[0]);
    index2+=sizeof(float);
}
void saveEntryToEEPROM(float maxVal,float minVal,int &index2){
    EEPROM.put(index2,maxVal);
    index2+=sizeof(float);
    EEPROM.put(index2,minVal);
    index2+=sizeof(float);
}
boolean setRange(float maxVal[],float minVal[],int index1, int &index2){
  float maxTemp;
  float minTemp;
  String str = getEntry(NumberOfConnectionAttempts,index1);
     //   Serial.println(String(str));
     delay(10);
//  displayDot();
updateLoadingBar();
  if(!str.equals(F(""))){
    maxTemp = getValue(str,separator,0).toFloat();
    minTemp = getValue(str,separator,1).toFloat();;
    EEPROM.put(index2,maxVal[0]);
    index2+=sizeof(float);
    EEPROM.put(index2,minVal[0]);
    index2+=sizeof(float);
  if(isnan(maxTemp)||isnan(minTemp)){
    return false;
  }
    maxVal[0] = maxTemp;
    minVal[0] = minTemp;
    maxVal[1] = maxVal[0];
    minVal[1] = minVal[0];
    return true;
  }else{
    index2+=2*sizeof(float);
    return false;
  }
}

boolean setRange(float maxVal[],float minVal[],int &index){
    float maxTemp;
    float minTemp;
    EEPROM.get(index,maxTemp);
    index+=sizeof(float);
    EEPROM.get(index,minTemp);
    index+=sizeof(float);
  //  displayDot();
  updateLoadingBar();

    if(!isnan(maxTemp)&&!isnan(minTemp)&&maxTemp!=0.0f&&minTemp!=0.0f&&maxTemp!=minTemp){
      maxVal[0] = maxTemp;
      minVal[0] = minTemp;
      maxVal[1] = maxVal[0];
      minVal[1] = minVal[0];
      return true;
    }
   return false;
}

boolean loadAutoRangeFromEEPROM(){
//  clearDisplay();
 // display.println("Auto range Overide");
 // displayA();
  float maxTemp;
  float minTemp;
  uint8_t d;

  int index = 300;
  EEPROM.get(index,d);
  index+=sizeof(uint8_t);
  
  if(isnan(d)||d <=-1||d>33||(d !=_day&&d != _day-1)){
    return false;
  }
  EEPROM.get(index,maxTemp);
  index+=sizeof(float);
  EEPROM.get(index,minTemp);
  index+=sizeof(float);

  
  if(isnan(maxTemp)||maxTemp>40.0f||maxTemp<-30.0f){
    return false;
  }
  if(isnan(minTemp)||minTemp>40.0f||maxTemp<-30.0f){
    return false;
  }
  absoluteMaxTemp = maxTemp;
  absoluteMinTemp = minTemp;

  if(!setRange(maxPressureChange,minPressureChange,index))return false;
  if(!setRange(maxInsideTemp,minInsideTemp,index))return false;
  if(!setRange(maxInsideHum,minInsideHum,index))return false;
  if(!setRange(maxPressure,minPressure,index))return false;
  if(!setRange(maxAverangeWind,maxWind,index))return false;
  if(!setRange(maxRain,minOutsideHum,index))return false;
//  setRange(highestTemperatureEverRecorded,lowestTemperatureEverRecorded,index);
  return true;
}

String getEntry(int8_t numOfAttempts,int8_t index){
   String result = ThingSpeak.readStringField(tempRangeChannelId,index, autoRangeApiKey);
   if(getStatusCode()){
      return result;
   }else if(numOfAttempts !=0){
      numOfAttempts--;
      getEntry(numOfAttempts,index);
   }else{
     return "";
   }
}

String getCreatedAt(int8_t numOfAttempts){
   String result = ThingSpeak.readCreatedAt(tempRangeChannelId, autoRangeApiKey);
   if(getStatusCode()){
      return result;
   }else if(numOfAttempts !=0){
      numOfAttempts--;
      getCreatedAt(numOfAttempts);
   }else{
     return "";
   }
}
boolean getStatusCode(){
 int statusCode =0;
 statusCode = ThingSpeak.getLastReadStatus();
 if(statusCode == 200){
  return true;
 }
 else{
  displayConnectionError(statusCode,700);
  return false;
 }
}
boolean absoluteFlag = false;
void calcMaxMinAbsolute(){
   if (shtTempO >absoluteMaxTemp&&(shtTempO -absoluteMaxTemp)<4.0f) {
    if(!absoluteFlag){
      absoluteFlag =true;
    }else{
       absoluteMaxTemp = shtTempO;
        absoluteFlag =false;
    }
    } else if (shtTempO <absoluteMinTemp&&(absoluteMinTemp - shtTempO)<4.0f) {
       if(!absoluteFlag){  
      absoluteFlag =true;
       }else{
           absoluteMinTemp = shtTempO;
            absoluteFlag =false;
       }
   } 
}
void calculateAutoRange(int monthV) {
   float gj = 0;
  if (!dataError&&!isnan(shtTempO)) {
    calcMaxMinAbsolute();
    if(monthV >=2&&monthV <=6&&absoluteMaxTemp<34.0f){
      if(shtTempO > 0){
        absoluteMaxTemp+=pow(shtTempO, SCALE_B)*(maximaScale*(sendInterval/SCALE_A))*1.4f;
      }
      absoluteMinTemp+=pow((OUT_OF_RANGE+shtTempO)-(OUT_OF_RANGE+absoluteMinTemp), SCALE_B)*(maximaScale*(sendInterval/SCALE_A));
    }else if(monthV >=9&&monthV <=11){
      gj = (OUT_OF_RANGE+absoluteMaxTemp)-(OUT_OF_RANGE+shtTempO);
      if(gj >0){
        absoluteMaxTemp-=pow(gj, SCALE_B)*(maximaScale*(sendInterval/SCALE_A));
      }
      absoluteMinTemp-=0.00026f;  
    }else{
      gj = (OUT_OF_RANGE+absoluteMaxTemp)-(OUT_OF_RANGE+shtTempO);
      if(gj >0){
        absoluteMaxTemp-=pow(gj, SCALE_B)*(maximaScale*(sendInterval/SCALE_A));
      }
      gj = (OUT_OF_RANGE+shtTempO)-(OUT_OF_RANGE+absoluteMinTemp);
      if(gj >0){
      absoluteMinTemp+=pow(gj, SCALE_B)*(maximaScale*(sendInterval/SCALE_A));
      }
    }
    _max[0]=absoluteMaxTemp;
    _min[0] =absoluteMinTemp;

    if (temp > maxInsideTemp[0]&&!isnan(temp)) {
    ///  maxInsideTemp[0] = temp;
           tempShtCtn++;
      if(tempShtCtn*sendInterval >= SHT_MAXIMA_DURATION){
          maxInsideTemp[0] = temp;
          tempShtCtn =0;
      } 
  //    saveRangeToEEPROM(maxInsideTemp[0],59);
    } else if (temp < minInsideTemp[0]&&!isnan(temp)) {
  //    minInsideTemp[0] = temp;

       tempShtCtn++;
      if(tempShtCtn*sendInterval >= SHT_MAXIMA_DURATION){
          minInsideTemp[0] = temp;
          tempShtCtn =0;
      }  
      
  //    saveRangeToEEPROM(minInsideTemp[0],59+4);
    }
    if(temp > highestTemperatureEverRecorded&&!isnan(temp)){
      highestTemperatureEverRecorded = temp;
    } else if (temp < lowestTemperatureEverRecorded&&!isnan(temp)) {
       lowestTemperatureEverRecorded = temp;
    }

    if (hum > maxInsideHum[0]&&!isnan(hum)) {
      humShtCtn++;
      if(humShtCtn*sendInterval >= SHT_MAXIMA_DURATION){
          maxInsideHum[0] = hum;
          humShtCtn =0;
      }
    } else if (hum < minInsideHum[0]&&!isnan(hum)) {
        humShtCtn++;
      if(humShtCtn*sendInterval >= SHT_MAXIMA_DURATION){
          minInsideHum[0] = hum;
          humShtCtn =0;
      }    
    }

    if (pres +PRESSURE_FIX> maxPressure[0]&&!isnan(pres)) {
      maxPressure[0] = pres;
    } else if ( pres+PRESSURE_FIX< minPressure[0]&&!isnan(pres)) {
      minPressure[0] = pres;
    }
     if (averangeWind > maxAverangeWind[0]&&!isnan(averangeWind)) {
      maxAverangeWind[0] = averangeWind;
    }
    if (maxCurrentWind > maxWind[0]&&!isnan(maxCurrentWind)) {
      maxWind[0] = maxCurrentWind;
    }
    if(rain > maxRain[0]&&!isnan(rain)){
      maxRain[0] = rain;
    }
    if(shtHumO < minOutsideHum[0]&&!isnan(shtHumO)){
      minOutsideHum[0] = shtHumO;
//      saveRangeToEEPROM(minOutsideHum[0],59+10*4);
    }
  }
}



////save all data to one entry due to slow read speed
//void sendAutoRangeToServer2(int8_t numOfAttempts){
//  if(millis() > sendAutoRangeTime +sendAutoRangeInterval){
//    sendAutoRangeTime =millis();
//    String entry = "";
//    entry +=String(_month)+separator+String(_day);
//    entry +=separator+String(absoluteMaxTemp)+separator+String(absoluteMinTemp);
//    entry +=separator+String(maxPressureChange[0])+separator+String(minPressureChange[0]);
//    entry +=separator+String(maxInsideTemp[0])+separator+String(minInsideTemp[0]);
//    entry +=separator+String(maxInsideHum[0])+separator+String(minInsideHum[0]);
//    entry +=separator+String(maxPressure[0])+separator+String(minPressure[0]);
//    entry +=separator+String(maxAverangeWind[0])+separator+String(maxWind[0]);
//    entry +=separator+String(maxRain[0])+separator+String(minOutsideHum[0]);
//    ThingSpeak.setField(1, entry);
//   if(displayConnectionError(ThingSpeak.writeFields(tempRangeChannelId,autoRangeApiKey),500)){
//      if(numOfAttempts !=0){
//        numOfAttempts--;
//        sendAutoRangeToServer(numOfAttempts);
//      }
//    }
//    
//  if(saveToEEPROMCounter < saveToEEPROMInterval){
//    saveToEEPROMCounter++;
//  }else{
//    saveRangeToEEPROM();
//    saveToEEPROMCounter = 0;
//  }
// }
//}

void sendAutoRangeToServer(int8_t numOfAttempts,boolean flag1){
  if((millis() > sendAutoRangeTime +sendAutoRangeInterval)||flag1){
    sendAutoRangeTime =millis();
    ThingSpeak.setField(1, String(_month)+separator+String(_day));
    ThingSpeak.setField(2, String(absoluteMaxTemp)+separator+String(absoluteMinTemp));
    ThingSpeak.setField(3, String(maxPressureChange[0])+separator+String(lowestTemperatureEverRecorded));
   // ThingSpeak.setField(3, String(maxPressureChange[0])+separator+String(minPressureChange[0])+separator+String(testDiffSum/testCtn)+separator+String(maxTestDiff)+separator+String(minTestDiff));/////DEBUG!!!!!!!!!!!!!!!!!!/////
    ThingSpeak.setField(4,String(maxInsideTemp[0])+separator+String(minInsideTemp[0]));
    ThingSpeak.setField(5,String(maxInsideHum[0])+separator+String(minInsideHum[0]));
    ThingSpeak.setField(6,String(maxPressure[0])+separator+String(minPressure[0]));
    ThingSpeak.setField(7,String(maxAverangeWind[0])+separator+String(maxWind[0]));
    ThingSpeak.setField(8,String(maxRain[0])+separator+String(minOutsideHum[0]));
    if(displayConnectionError(ThingSpeak.writeFields(tempRangeChannelId,autoRangeApiKey),500)){
      if(numOfAttempts !=0){
        numOfAttempts--;
        sendAutoRangeToServer(numOfAttempts,false);
      }
    }

    if(saveToEEPROMCounter < saveToEEPROMInterval){
      saveToEEPROMCounter++;
    }else{
      saveAutoRangeToEEPROM();
      saveToEEPROMCounter = 0;
    }
  }
}
float loadEntryFromEEPROM(int8_t index){
  EEPROM.begin(70);
  delay(5);
  byte v = EEPROM.read(index);
  EEPROM.end();
  if(v <= 100&& v!=0){
    return (float)v;
  }else{
    return -1;
  }
}


//float getValue(String data, char separator, int index)
//{
//  int found = 0;
//  int strIndex[] = {0, -1};
//  int maxIndex = data.length()-1;
//
//  for(int i=0; i<=maxIndex && found<=index; i++){
//    if(data.charAt(i)==separator || i==maxIndex){
//        found++;
//        strIndex[0] = strIndex[1]+1;
//        strIndex[1] = (i == maxIndex) ? i+1 : i;
//    }
//  }
//
//  return (found>index ? data.substring(strIndex[0], strIndex[1]) : "");
//}
 String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//float getDateValue(String dateString, int index){
//    char dt[] = "T-";
//   char* ptr = strtok(dateString, dt);
//   ptr = strtok(NULL, dt);
//   if(index == 0){
//    return  (String(ptr)).toInt();
//   }else{
//       ptr = strtok(NULL, dt);
//       return  (String(ptr)).toInt();
//   }
//}

void setMaxMin(){
  _max[0] = (monthMax[_month-1]*map(_day,1,_month%2?31:30,0,100)+monthMax[_month==1?12:_month-2]*map(_day,1,_month%2?31:30,100,0))/100;
  _min[0] = (monthMin[_month-1]*map(_day,1,_month%2?31:30,100,0)+monthMin[_month==12?0:_month]*map(_day,1,_month%2?31:30,0,100))/100; 
}

//
//float[] getValue2(String data, char separator){
//  
//}

//void convertEntryToFloatArr(String entry, float arr[]){
// int8_t index = 0;
// char* buff[
// char* ptr = strtok(entry, separator );
//  while(ptr != NULL&& index< ) {
//        arr[index] = ptr.toFloat();
//        index++;
//        ptr = strtok(NULL, separator);
//  }
//}
