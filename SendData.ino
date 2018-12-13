void sendData() {
   String error = "";
  if (client.connect(server, 80))   // 184.106.153.149
  {  
    String postStr = apiKey;
    if (!dataNotReceived) {
      if (humO ==0&&tempO==0) {
        error+="dht error ";
      } else {
        if (tempO <-50||tempO >50) {
          error+="tempO= "+String(tempO);
        } else {
          postStr +="&field1=";
          postStr += String(tempO);
        }
        if (humO >100) {
          error+="humO= "+String(humO);
        } else {
          postStr +="&field3=";
          postStr += String(humO);
        }
      }  

      if(averangeWind <0|| averangeWind >99){
        error+="averangeWind= "+String(averangeWind);
      }else{
        postStr +="&field6=";
        postStr += String(averangeWind);
      }
      
      if (maxCurrentWind <0||maxCurrentWind >99) {
        error+="maxCurrentWind= "+String(maxCurrentWind);
      } else {
        postStr +="&field7=";
        postStr += String(maxCurrentWind);
      }
      
      if(rain <0|| rain >99){
        error+="rain= "+String(rain);
      }else{
        postStr +="&field8=";
        postStr += String(rain);
      }
    }
    if (temp != 0.0f) {
      postStr +="&field2=";
      postStr += String(temp, 2);
    } else {
      error+="temp=0 ";
    }
    if (hum != 0.0f) {
      postStr +="&field4=";
      postStr += String(hum, 2);
    } else {
      error+="hum=0 ";
    }
    if (pres ==0.0f||pres > 1150.0f) {
      error+="pres= "+ String(pres);
    } else {
      postStr +="&field5=";
      postStr += String(pres, 2);
    }
    // postStr += "\r\n\r\n";

    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
    client.print(F("Connection: close\n"));
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(postStr.length());
    client.print(F("\n\n"));
    client.print(postStr);
  }else{
    clearDisplay();
    display.print(F("connection error"));
    display.display();
    delay(2000);
  }
  client.stop();
  if (!error.equals("")) {
    sendError(error);
  }
}

void sendError(String error) {
  if (client.connect(server, 80)) {
    String debugApikey = "V9BDERAUY8VGWJU7";
    String postStr = debugApikey;
    postStr +="&field1=";
    postStr+=error;
    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
    client.print(F("Connection: close\n"));
    client.print("X-THINGSPEAKAPIKEY: "+debugApikey+"\n");
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(postStr.length());
    client.print(F("\n\n"));
    client.print(postStr);
  }
  client.stop();
}

void sendTempRange() {
  if (client.connect(server, 80)) {
    String debugApikey = "T44K8QM9YHVD73WG";
    String postStr = debugApikey;
    postStr +="&field1=";
    postStr+=String(_max);
    postStr +="&field2=";
    postStr+=String(_min);
    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
    client.print(F("Connection: close\n"));
    client.print("X-THINGSPEAKAPIKEY: "+debugApikey+"\n");
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(postStr.length());
    client.print(F("\n\n"));
    client.print(postStr);
  }
  client.stop();
  
  EEPROM.begin(40);
  delay(5);
  if(_max >=-2&& _max < 47){
       EEPROM.put(30,(byte)(floor(_max)));
  }else{
      EEPROM.put(30,(byte)(floor(fabs(_max))+50));
  }  
  EEPROM.put(31,(byte)((fabs(_max) - floor(fabs(_max)))*100));

  if(_min >=-40&& _min < 36){
       EEPROM.put(32,(byte)(floor(_min)));
  }else{
      EEPROM.put(32,(byte)(floor(fabs(_min))+50));
  }  
  EEPROM.put(33,(byte)((fabs(_min) - floor(fabs(_min)))*100)); 
  EEPROM.end();
}

void sendDailyMaxima() {
  if (client.connect(server, 80)&&millis() >= 86400000l-sendInterval) {     
    char separationCharacter = ',';
    String postStr = dailyMaximaApiKey;
    postStr +="&field1=";
    postStr += String(dailyOutsideTemperatureSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideTemperatureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideTemperatureMaxima[1]);
    postStr +="&field3=";
    postStr += String(dailyOutsideHumiditySum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideHumidityMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyOutsideHumidityMaxima[1]);
    postStr +="&field6=";
    postStr += String(dailyAverangeWindSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyAverangeWindMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyAverangeWindMaxima[1]);
    postStr +="&field7=";
    postStr += String(dailyWindSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyWindMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyWindMaxima[1]);
    postStr +="&field2=";
    postStr += String(dailyInsideTemperatureSum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideTemperatureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideTemperatureMaxima[1]);
    postStr +="&field4=";
    postStr += String(dailyInsideHumiditySum/dailySamples, 1);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideHumidityMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyInsideHumidityMaxima[1]);
    postStr +="&field5=";
    postStr += String(dailyPressureSum/dailySamples, 2);
    postStr +=separationCharacter;
    postStr +=String(dailyPressureMaxima[0]);
    postStr +=separationCharacter;
    postStr +=String(dailyPressureMaxima[1]);
    postStr +="&field8=";
    postStr +=String(dailyRain);
    
    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
    client.print(F("Connection: close\n"));
    client.print("X-THINGSPEAKAPIKEY: "+dailyMaximaApiKey+"\n");
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

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
  client.stop();
  delay(100);
  sendTempRange();  ///////////////////////////// DEBUG ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
