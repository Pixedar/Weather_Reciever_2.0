float absoluteMaxTemp = -100;
float absoluteMinTemp = 100;
unsigned long sendTempRangeTime;
const char * tempRangeApiKey = "T44K8QM9YHVD73WG";
const unsigned long tempRangeChannelId = 639828;
float maximaScale = 0.00000071f;
unsigned long sendTempRangeInterval= 18000000;
char separator = 'p';

void initAutoRange(){
  int m = ThingSpeak.readIntField(tempRangeChannelId,1, tempRangeApiKey);
  String str;
  float val;
  displayDot();
  if(m >-1&&m<=12&&_month == m){
    str = ThingSpeak.readStringField(tempRangeChannelId,2, tempRangeApiKey);
    val = getValue(str,separator,0);
    if(getStatusCode()&&val !=-1&&val<37&&val>-30){
      absoluteMaxTemp = val;
    }
    val = getValue(str,separator,1);
    if(getStatusCode()&&val !=-1&&val<37&&val>-30){
       absoluteMinTemp = val;
    }
    displayDot();
  }
   str = ThingSpeak.readStringField(tempRangeChannelId,4, tempRangeApiKey);
   displayDot();
   if(getStatusCode()){
      maxInsideTemp = getValue(str,separator,0);
      minInsideTemp = getValue(str,separator,1);
   }
   str = ThingSpeak.readStringField(tempRangeChannelId,5, tempRangeApiKey);
   displayDot();
   if(getStatusCode()){
      maxInsideHum = getValue(str,separator,0);
      minInsideHum = getValue(str,separator,1);
   }
  str = ThingSpeak.readStringField(tempRangeChannelId,6, tempRangeApiKey);
  displayDot();
   if(getStatusCode()){
      maxPressure = getValue(str,separator,0);
      minPressure = getValue(str,separator,1);
   }
  str = ThingSpeak.readStringField(tempRangeChannelId,7, tempRangeApiKey);
  displayDot();
   if(getStatusCode()){
      maxAverangeWind = getValue(str,separator,0);
      maxWind = getValue(str,separator,1);
   }
  str = ThingSpeak.readStringField(tempRangeChannelId,8, tempRangeApiKey);
  displayDot();
   if(getStatusCode()){
      maxRain = (int8_t)getValue(str,separator,0);
      minOutsideHum = (int8_t)getValue(str,separator,1);
   }

  _max=absoluteMaxTemp;
  _min =absoluteMinTemp;
  sendTempRangeTime =millis(); 
}
boolean getStatusCode(){
 int statusCode =0;
 statusCode = ThingSpeak.getLastReadStatus();
 if(statusCode == 200){
  return true;
 }
 else{
  displayConnectionError(statusCode);
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
        absoluteMaxTemp+=pow(tempO, 2)*(maximaScale*(sendInterval/180000.0f));
      }
      absoluteMinTemp+=pow((50+tempO)-(50+absoluteMinTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }else if(monthV >=9&&monthV <=11){
       absoluteMaxTemp-=pow((50+tempO)-(50+absoluteMaxTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }else{
      absoluteMaxTemp-=pow((50+tempO)-(50+absoluteMaxTemp), 2)*(maximaScale*(sendInterval/180000.0f));
      absoluteMinTemp+=pow((50+tempO)-(50+absoluteMinTemp), 2)*(maximaScale*(sendInterval/180000.0f));
    }
    _max=absoluteMaxTemp;
    _min =absoluteMinTemp;

    if (temp > maxInsideTemp) {
      maxInsideTemp = temp;
    } else if (temp < minInsideTemp) {
      minInsideTemp = temp;
    }

    if (hum > maxInsideHum) {
      maxInsideHum = hum;
    } else if (temp < minInsideTemp) {
      minInsideHum = hum;
    }

    if (pres > maxPressure) {
      maxPressure = pres;
    } else if ( pres< minPressure) {
      minPressure = pres;
    }
     if (averangeWind > maxAverangeWind) {
      maxAverangeWind = averangeWind;
    }
    if (maxCurrentWind > maxWind) {
      maxWind = maxCurrentWind;
    }
    if(rain > maxRain){
      maxRain = rain;
    }
    if(humO < minOutsideHum){
      minOutsideHum = humO;
    }
  }
}


void sendAutoRangeToServer(){
  if(millis() > sendTempRangeTime +sendTempRangeInterval){
    sendTempRangeTime =millis();
    ThingSpeak.setField(1, _month);
    ThingSpeak.setField(2, String(absoluteMaxTemp)+separator+String(absoluteMinTemp));
    ThingSpeak.setField(4,String(maxInsideTemp)+separator+String(minInsideTemp));
    ThingSpeak.setField(5,String(maxInsideHum)+separator+String(minInsideHum));
    ThingSpeak.setField(6,String(maxPressure)+separator+String(minPressure));
    ThingSpeak.setField(7,String(maxAverangeWind)+separator+String(maxWind));
    ThingSpeak.setField(8,String(maxRain)+separator+String(minOutsideHum));
    displayConnectionError(ThingSpeak.writeFields(tempRangeChannelId,tempRangeApiKey)); 
  }
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


