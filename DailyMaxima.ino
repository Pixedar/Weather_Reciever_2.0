
const char * dailyMaximaApiKey = "VJFQUX9HYOWZAQT4"; 
float dailyPressureSum;;
float dailyInsideTemperatureSum;
float dailyInsideHumiditySum;
float dailyOutsideHumiditySum;
float dailyOutsideTemperatureSum;
float dailyWindSum;
float dailyAverangeWindSum;
int   dailyRain;

float dailyPressureMaxima[] = {2000,0};
float dailyInsideTemperatureMaxima[] = {100,0};
float dailyInsideHumidityMaxima[] = {100,0};
float dailyOutsideHumidityMaxima[] = {100,0};
float dailyOutsideTemperatureMaxima[] = {100,0};
float dailyWindMaxima[] = {300,0};
float dailyAverangeWindMaxima[] = {300,0};
float dailySamples = 0;
boolean dailyMaximaSent = false; 
void calculateDailyMaxima(){
 dailySamples++;  
 dailyPressureSum+=pres;
 dailyInsideTemperatureSum+=temp;
 dailyInsideHumiditySum+=hum;
 dailyOutsideHumiditySum+=shtHumO;
 dailyOutsideTemperatureSum+=shtTempO;
 dailyWindSum+=maxCurrentWind;
 dailyAverangeWindSum+=averangeWind;
 dailyRain+=rain;
 if(pres < dailyPressureMaxima[0]){
  dailyPressureMaxima[0] = pres;
 }else if(pres > dailyPressureMaxima[1]){
  dailyPressureMaxima[1] = pres;
 }
 if(temp < dailyInsideTemperatureMaxima[0]){
  dailyInsideTemperatureMaxima[0] = temp;
 }else if(temp > dailyInsideTemperatureMaxima[1]){
   dailyInsideTemperatureMaxima[1] = temp;
 }
 if(hum < dailyInsideHumidityMaxima[0]){
  dailyInsideHumidityMaxima[0] = hum;
 }else if(hum > dailyInsideHumidityMaxima[1]){
  dailyInsideHumidityMaxima[1] = hum;
 }
 if(shtHumO < dailyOutsideHumidityMaxima[0]){
  dailyOutsideHumidityMaxima[0] = shtHumO;
 }else if(shtHumO > dailyOutsideHumidityMaxima[1]){
  dailyOutsideHumidityMaxima[1] = shtHumO;
 }
 if(shtTempO < dailyOutsideTemperatureMaxima[0]){
  dailyOutsideTemperatureMaxima[0] = shtTempO;
 }else if(shtTempO > dailyOutsideTemperatureMaxima[1]){
   dailyOutsideTemperatureMaxima[1] = shtTempO;
 }
 if(maxCurrentWind < dailyWindMaxima[0]){
  dailyWindMaxima[0] = maxCurrentWind;
 }else if(maxCurrentWind > dailyWindMaxima[1]){
   dailyWindMaxima[1] = maxCurrentWind;
 }
 if(averangeWind < dailyAverangeWindMaxima[0]){
   dailyAverangeWindMaxima[0] = averangeWind;
 }else if(averangeWind > dailyAverangeWindMaxima[1]){
   dailyAverangeWindMaxima[1] = averangeWind;
 }
}
void sendDailyMaximaTimeCheck(){
  if(_hour == 0){
    if(!dailyMaximaSent){
      sendDailyMaxima(false);
      dailyMaximaSent = true;
    }
  }else{
    dailyMaximaSent = false;
  }
}

void sendDailyMaxima(boolean flag) {
  if (millis() >= 86400000l-sendInterval||flag) {     
    char separationCharacter = ',';
    String postStr = "";
    postStr += String(dailyOutsideTemperatureSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideTemperatureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideTemperatureMaxima[1]);
    ThingSpeak.setField(1, postStr);
    postStr = "";
    postStr += String(dailyOutsideHumiditySum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideHumidityMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideHumidityMaxima[1]);
    ThingSpeak.setField(3, postStr);
    postStr = "";
    postStr += String(dailyAverangeWindSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyAverangeWindMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyAverangeWindMaxima[1]);
    ThingSpeak.setField(6, postStr);
    postStr = "";
    postStr += String(dailyWindSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyWindMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyWindMaxima[1]);
    ThingSpeak.setField(7, postStr);
    postStr = "";
    postStr += String(dailyInsideTemperatureSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideTemperatureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideTemperatureMaxima[1]);
    ThingSpeak.setField(2, postStr);
    postStr = "";
    postStr += String(dailyInsideHumiditySum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideHumidityMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideHumidityMaxima[1]);
    ThingSpeak.setField(4, postStr);
    postStr = "";
    postStr += String(dailyPressureSum/dailySamples, 2);
    postStr +=separationCharacter;
    postStr +=String(dailyPressureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyPressureMaxima[1]);
    ThingSpeak.setField(5, postStr);
    postStr = "";
    postStr +=String(dailyRain);
    ThingSpeak.setField(8, postStr);
    postStr = "";
    
    const unsigned long id =  544874;
    displayConnectionError(ThingSpeak.writeFields(id,dailyMaximaApiKey)); 
  
    dailyRain = 0;
    dailySamples = 0;
    dailyPressureSum=0;
    dailyInsideTemperatureSum =0;
    dailyInsideHumiditySum =0;
    dailyOutsideHumiditySum =0;
    dailyOutsideTemperatureSum =0;
    dailyWindSum =0;
    dailyAverangeWindSum =0;
    dailyPressureMaxima[0] = 2000;
    dailyPressureMaxima[1] = 0;
    dailyInsideTemperatureMaxima[0] = 100;
    dailyInsideTemperatureMaxima[1] = 0;
    dailyInsideHumidityMaxima[0] = 100;
    dailyInsideHumidityMaxima[1] = 0;
    dailyOutsideHumidityMaxima[0] =100;
    dailyOutsideHumidityMaxima[1] =0;
    dailyOutsideTemperatureMaxima[0] = 100;
    dailyOutsideTemperatureMaxima[1] = 0;
    dailyWindMaxima[0] = 3000;
    dailyWindMaxima[1] = 0;
    dailyAverangeWindMaxima[0] = 3000;
    dailyAverangeWindMaxima[1] = 0;
  }
  delay(100);
}
