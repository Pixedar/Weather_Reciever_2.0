float absoluteMaxTemp = -100;
float absoluteMinTemp = 100;
unsigned long sendAutoRangeTime;
const char * autoRangeApiKey = "T44K8QM9YHVD73WG";
const unsigned long tempRangeChannelId = 639828;
float maximaScale = 0.000000832f;
const unsigned long sendAutoRangeInterval= 18000000;
const char separator = 'p';
const int8_t NumberOfConnectionAttempts = 1;
const int8_t saveToEEPROMInterval = 28;
int8_t saveToEEPROMCounter = 0;

void initAutoRange(){
  String str;
  float val;
  int d;
  str = getEntry(NumberOfConnectionAttempts,1);
  val = getValue(str,separator,0);
  d = (int)getValue(str,separator,1);
  displayDot();
  if(val >-1&&val<=12&&(_month == (int)val||_month == (int)(val-1))){
    str = getEntry(NumberOfConnectionAttempts,2);
    if(!str.equals(F(""))){
      val = getValue(str,separator,0);
      if(val !=-1&&val<37&&val>-30){
        absoluteMaxTemp = val;
      }
      val = getValue(str,separator,1);
      if(val !=-1&&val<37&&val>-30){
        absoluteMinTemp = val;
      }
    }
    if(_month == (int)(val-1)&&d<=29){
      absoluteMaxTemp = absoluteMaxTemp*0.45f;
      absoluteMinTemp = absoluteMinTemp*0.45f;
    }
    displayDot();
  }
   str = getEntry(NumberOfConnectionAttempts,4);
   displayDot();
   if(!str.equals(F(""))){
      maxInsideTemp[0] = getValue(str,separator,0);
      minInsideTemp[0] = getValue(str,separator,1);
      maxInsideTemp[1] = maxInsideTemp[0];
      minInsideTemp[1] = minInsideTemp[0];
   }
   str = getEntry(NumberOfConnectionAttempts,5);
   displayDot();
   if(!str.equals(F(""))){
      maxInsideHum[0] = getValue(str,separator,0);
      minInsideHum[0] = getValue(str,separator,1);
      maxInsideHum[1] = maxInsideHum[0];
      minInsideHum[1] = minInsideHum[0];
   }
  str =getEntry(NumberOfConnectionAttempts,6);
  displayDot();
   if(!str.equals(F(""))){
      maxPressure[0] = getValue(str,separator,0);
      minPressure[0] = getValue(str,separator,1);
      maxPressure[1] = maxPressure[0];
      minPressure[1]= minPressure[0];
   }
  str = getEntry(NumberOfConnectionAttempts,7);
  displayDot();
   if(!str.equals(F(""))){
      maxAverangeWind[0] = getValue(str,separator,0);
      maxAverangeWind[1] = maxAverangeWind[0];
      maxWind[0] = getValue(str,separator,1);
      maxWind[1] = maxWind[0];
   }
  str = getEntry(NumberOfConnectionAttempts,8);
  displayDot();
   if(!str.equals(F(""))){
      maxRain[0] = (int8_t)getValue(str,separator,0);
      maxRain[1] = maxRain[1];
      minOutsideHum[0] = (int8_t)getValue(str,separator,1);
      minOutsideHum[1] = minOutsideHum[0];
   }

  _max[0]=absoluteMaxTemp;
  _min[0] =absoluteMinTemp;
  maxPressure[0] = 995; //// temporary fix!!!
  sendAutoRangeTime =millis(); 
}
//int getMonth(int8_t numOfAttempts){
//   int result = ThingSpeak.readIntField(tempRangeChannelId,1, autoRangeApiKey);
//   if(getStatusCode()){
//      return result;
//   }else if(numOfAttempts !=0){
//      numOfAttempts--;
//      getMonth(numOfAttempts);
//   }else{
//      return -1;
//   }
//}
String getEntry(int8_t numOfAttempts,int8_t index){
   String result = ThingSpeak.readStringField(tempRangeChannelId,index, autoRangeApiKey);
   if(getStatusCode()){
      return result;
   }else if(numOfAttempts !=0){
      numOfAttempts--;
      getEntry(numOfAttempts,index);
   }else{
      float v1 = loadEntryFromEEPROM(index);
      float v2 = loadEntryFromEEPROM(index+1);
      if(v1 != -1&&v2!=-1){
        return String(v1)+String(separator)+String(v2);
      }else{
        return "";
      }
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
void calculateAutoRange(int monthV) {
  if (!dataError) {
    if (tempO >absoluteMaxTemp) {
      absoluteMaxTemp = tempO;
    } else if (tempO <absoluteMinTemp) {
      absoluteMinTemp = tempO;
    } 
    if(monthV >=2&&monthV <=6){
      if(tempO > 0){
        absoluteMaxTemp+=pow(tempO, 2)*(maximaScale*(sendInterval/180000.0f))*1.4f;
      }
      absoluteMinTemp+=pow((50+tempO)-(50+absoluteMinTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }else if(monthV >=9&&monthV <=11){
       absoluteMaxTemp-=pow((50+tempO)-(50+absoluteMaxTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }else{
      absoluteMaxTemp-=pow((50+tempO)-(50+absoluteMaxTemp), 2)*(maximaScale*(sendInterval/180000.0f));
      absoluteMinTemp+=pow((50+tempO)-(50+absoluteMinTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }
    _max[0]=absoluteMaxTemp;
    _min[0] =absoluteMinTemp;

    if (temp > maxInsideTemp[0]) {
      maxInsideTemp[0] = temp;
    } else if (temp < minInsideTemp[0]) {
      minInsideTemp[0] = temp;
    }

    if (hum > maxInsideHum[0]) {
      maxInsideHum[0] = hum;
    } else if (hum < minInsideHum[0]) {
      minInsideHum[0] = hum;
    }

    if (pres > maxPressure[0]) {
      maxPressure[0] = pres;
    } else if ( pres< minPressure[0]) {
      minPressure[0] = pres;
    }
     if (averangeWind > maxAverangeWind[0]) {
      maxAverangeWind[0] = averangeWind;
    }
    if (maxCurrentWind > maxWind[0]) {
      maxWind[0] = maxCurrentWind;
    }
    if(rain > maxRain[0]){
      maxRain[0] = rain;
    }
    if(humO < minOutsideHum[0]){
      minOutsideHum[0] = humO;
    }
  }
}


void sendAutoRangeToServer(int8_t numOfAttempts){
  if(millis() > sendAutoRangeTime +sendAutoRangeInterval){
    sendAutoRangeTime =millis();
    ThingSpeak.setField(1, String(_month)+separator+String(_day));
    ThingSpeak.setField(2, String(absoluteMaxTemp)+separator+String(absoluteMinTemp));
    ThingSpeak.setField(4,String(maxInsideTemp[0])+separator+String(minInsideTemp[0]));
    ThingSpeak.setField(5,String(maxInsideHum[0])+separator+String(minInsideHum[0]));
    ThingSpeak.setField(6,String(maxPressure[0])+separator+String(minPressure[0]));
    ThingSpeak.setField(7,String(maxAverangeWind[0])+separator+String(maxWind[0]));
    ThingSpeak.setField(8,String(maxRain[0])+separator+String(minOutsideHum[0]));
    if(displayConnectionError(ThingSpeak.writeFields(tempRangeChannelId,autoRangeApiKey),500)){
      if(numOfAttempts !=0){
        numOfAttempts--;
        sendAutoRangeToServer(numOfAttempts);
      }
    }
    if(saveToEEPROMCounter < saveToEEPROMInterval){
      saveToEEPROMCounter++;
    }else{
      saveRangeToEEPROM();
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
void saveRangeToEEPROM(){
  int c = 50;
   EEPROM.begin(70);
   delay(5);
   EEPROM.put(51,_month);
   EEPROM.put(52,_day);
   EEPROM.put(53,(byte)(c+absoluteMaxTemp));
   EEPROM.put(54,(byte)(c+absoluteMinTemp));
   EEPROM.put(55,(byte)(maxInsideTemp[0]));
   EEPROM.put(56,(byte)(minInsideTemp[0]));
   EEPROM.put(57,(byte)(maxInsideHum[0]));
   EEPROM.put(58,(byte)(minInsideHum[0]));
   EEPROM.put(59,(byte)(maxPressure[0]));
   EEPROM.put(60,(byte)(minPressure[0]));
   EEPROM.end();
}
float getValue(String data, char separator, int index)
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

  return (found>index ? data.substring(strIndex[0], strIndex[1]) : "").toFloat();
}


