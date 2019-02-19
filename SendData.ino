const char * apiKey = "12RUI1FA6G5C5CWM"; 
const char * dailyMaximaApiKey = "VJFQUX9HYOWZAQT4"; 
const char * debugApikey = "V9BDERAUY8VGWJU7";
const char * tempRangeApikey = "T44K8QM9YHVD73WG";
float lastTempO =0;
float lastAvgWind =0;
float lastWind =0;
int lastRain =0;
int lastHumO =0;
boolean firstTime = true;
boolean repeat =true;
void sendData() { 
   const unsigned long id = 333150;
   String error = "";
    if (!dataNotReceived) {
      if (humO ==0&&tempO==0) {
        error+="dht error ";
      } else {
        if (tempO <-39||tempO >40||((abs(lastTempO-tempO)>3.75f)&&!firstTime)) {
          error+="tempO= "+String(tempO);
        } else {
          ThingSpeak.setField(1, String(tempO,2));
        }
        if (humO >100||((abs(lastHumO - humO)>37)&&!firstTime)) {
          error+="humO= "+String(humO);
        } else {     
         ThingSpeak.setField(3, humO);
        }
      }  

      if(averangeWind <0|| averangeWind >49||((abs(lastAvgWind - averangeWind) > 10)&&!firstTime)){
        error+="averangeWind= "+String(averangeWind);
      }else{ 
        ThingSpeak.setField(6,String(averangeWind,2));
      }
      
      if (maxCurrentWind <0||maxCurrentWind >70||(abs(lastWind - maxCurrentWind)>40)&&!firstTime) {
        error+="maxCurrentWind= "+String(maxCurrentWind);
      } else {
        ThingSpeak.setField(7,maxCurrentWind);
      }
      
      if(rain <0|| rain >99||(abs(lastRain - rain) > 33&&!firstTime)){
        error+="rain= "+String(rain);
      }else{
        ThingSpeak.setField(8,rain);
      }
      lastHumO = humO;
      lastTempO = tempO;
      lastRain = rain;
      lastAvgWind =  averangeWind;
      lastWind = maxCurrentWind;
      if(firstTime){
        firstTime = false;
      } 
    }
    if (temp != 0.0f) {
      ThingSpeak.setField(2,String(temp,2));
    } else {
      error+="temp=0 ";
    }
    if (hum != 0.0f) {
      ThingSpeak.setField(4,String(hum,2));
    } else {
      error+="hum=0 ";
    }
    if (pres ==0.0f||pres > 1150.0f) {
      error+="pres= "+ String(pres);
    } else {
      ThingSpeak.setField(5,String(pres,2));
    }
  dataError = false;
  displayConnectionError(ThingSpeak.writeFields(id, apiKey));
  if (!error.equals("")) {
    sendError(error);
    dataError = true;
  }
}

void sendError(String error) {
  const unsigned long id = 556803;
  displayConnectionError(ThingSpeak.writeField(id, 1, error, debugApikey));  
}

void displayConnectionError(int httpCode ){
    if(httpCode != 200){
     clearDisplay();
     display.print(F("HTTP ERROR"));
     display.println(String(httpCode));
     display.display();
     delay(2000);
     if(repeat&&!firstTime){
      repeat = false;
      sendData();
    }
  }else{
    repeat = true; 
  }
}

void sendDailyMaxima() {
  if (millis() >= 86400000l-sendInterval) {     
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


