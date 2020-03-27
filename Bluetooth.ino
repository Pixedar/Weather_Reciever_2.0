void readBluetoothCommands() {
  if (Serial.available()) {
    unsigned long t = millis();
    byte a,b,c,d;
    float mc[2] ={0,0};
    while (millis() < t + 3000&&!displayHeartBeatEnabled) {
      delay(2);
      if (Serial.available()) {
        switch(Serial.read()) {
        case 0:
          sendSettings(false);
          break;
        case 1:
          brightness = float(Serial.read())/100.0f;
          ledAnalogWrite();
          analogValue = brightness*100;
          displayBrightness();
          lastBrightness = brightness;
          digitalWrite(LED_BUILTIN, LOW);
          break;
        case 2:
          colorMode = Serial.read();
          setColor(true);
          saveByte((byte)colorMode,0);
          break;
        case 3:
          getColorMode();
          setColor(false);
          clearDisplay();
          display.print(F("ColorMode has been set"));
          displayA();
          break;
        case 4:
          getMaxima(mc[0],mc[1]);
          setColor(false);
          clearDisplay();
          display.println(F("Maxima have been set"));
          display.print(F("Max: "));
          display.println(String(mc[0]));
          display.print(F("Min: "));
          display.print(String(mc[1]));
          displayA();
          break;
        case 5:        
          break;
        case 6:
          break;
        case 7:
          
          break;
        case 8:
          startDimHour = Serial.read();
          stopDimHour = Serial.read();
          startDimMinute = Serial.read();
          stopDimMinute = Serial.read();
          fluidDimming = (int)Serial.read();
          if(isnan(fluidDimming)||fluidDimming<0){
            fluidDimming = 0;
          }
          clearDisplay();
          display.print(F("Turn off time has been set"));
          displayA();
          saveByte((byte)startDimHour ,23);
          saveByte((byte)stopDimHour ,24);
          saveByte((byte)startDimMinute ,25);
          saveByte((byte)stopDimMinute ,26);
          break;
        case 9:
          networkOnlyMode = boolean(Serial.read());
          clearDisplay();
          display.println(F("NetworkOnlyMode: "));
          display.print(String(networkOnlyMode));
          displayA();
          saveByte((byte)networkOnlyMode  ,28);
          break;
        case 10:
          if(boolean(Serial.read())){
        //    display.testDim(1);
          }else{
        //    display.testDim(0);
          }
          break;
        case 13:
          if(enableKnob){
            enableKnob = false;
          }else{
            enableKnob = true;
          }
          break;
        case 14:
          d = Serial.read();
          if(d >byte(0)&& d <=byte(125)){
            maximaScale = ((float)d/100000000.0f);
            display.println(F("AutoTemp scale:"));
            display.print(String(d));
            display.print("%");
          }else{
            display.println(F("wrong value"));
          }
            displayA();
          break;
        case 16:
          d = Serial.read();
          if(d>=0&&d < sizeof(rangeMode)/sizeof(boolean)){
            rangeMode[d] = true;
          }
          break;
        case 17: // hex 11
           executeOTA();
           break;
        case 18: // hex 12
           ESP.restart();  
           break;
       case 19:
           d = Serial.read();
           if(d < 9){
            rangeMode[d] = false;
            clearDisplay();
            display.print(F("Auto range "));
            display.print(String(d));
           }else{
            display.println(F("wrong value"));
           }
           displayA();
           break;
        case 20:
           hsv_h_ext = (int)(Serial.read());        
          break;
        case 21:
            setBmpFilter();
            break;
        case 22:
           sendAutoRangeToServer(2,true);
           break;
        case 23: //hex 17
          if(!debugPressureChange){
            debugPressureChange = true;
          }else{
            debugPressureChange = false;
          }
          break;
        case 24: // hex 18
          loadAutoRangeFromEEPROM();
          break;
        case 25: // hex 19
          if(!transmitCurrentPressure){
            transmitCurrentPressure = true;
          }else{
            transmitCurrentPressure = false;
          }
          break;
        case 26: // hex 0x1A kalibracja diody     
          analogWrite(BLUE_PIN,(Serial.read()/100.0f)*1023);
          analogWrite(GREEN_PIN,(Serial.read()/100.0f)*1023);
          analogWrite(RED_PIN,(Serial.read()/100.0f)*1023);
          while(millis() < t + 30000){
          }  
          break;
        case 27: //0x1B
          Serial.println(_month);
          Serial.println(_day);
          Serial.println(absoluteMaxTemp);
          Serial.println(absoluteMinTemp);    
          break;    
        case 28:
          saveAutoRangeToEEPROM();
          break;
        case 29:
          loadAutoRangeFromServer();
          break; 
        case 30:
          sendDailyMaxima(true);
          break;
        case 31:
           EEPROM.begin(512);
          delay(5);
          EEPROM.put(170,(byte)1);
           EEPROM.put(180,(byte)0);
          EEPROM.end();
          break;
        case 32:
          analogWriteFreq(((int)(Serial.read()))*500);
          break;
        case 33:
          setWIFi();
          break;
        case 34:
          btSendSavedFields();
          break;
        case 35:
          absoluteMinTemp = 5.8f;
          absoluteMaxTemp = 30.25f;
          break;
        case 36:
          Serial.println(WiFi.status());
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          resetWiFiConnection = true;
          break;
        case 40:
            ThingSpeak.begin(client);
            break;
        case 42:
         lightLock = Serial.read();
          Serial.println(lightLock);
          saveByte((byte)lightLock,27);
        break;  
        case 43:
            EEPROM.begin(37);
          delay(12);
          Serial.println(EEPROM.read(27));
            EEPROM.end();
        break; 
        case 46:
          shtIIRFilterCoef  = (float)(Serial.read()/100.0f);
          break;
        case 47:
          getRfPocketRatio();
        break;
        case 48:
          rf_pocketsReceivedCounter =1;
          rf_pocketsLostCounter = 0;
          break;
        case 49:
        Serial.println(maxRfTime);
        Serial.println(minRFTime);
        Serial.println((double)rqSum/rqSamples);
        Serial.println(minRFTime2);
        Serial.println((double)rqSum2/rqSamples);
        break;
        case 50:
        enableProgrammingMode();
        break;
        }
        Serial.flush();
        mySerial.flush();
        
        t = millis();
      }
    }
    if(displayHeartBeatEnabled){
      getHearBeatParams(Serial.read());
    }else{
      if(!clockColorMode){
        displayWeatherData(true);
      }
      mySerial.flush();  
    } 
  }
}
void enableProgrammingMode(){
  byte b[2] = {(byte)202,(byte)202};
  mySerial.write(b,2);
  mySerial.println(F("Programming mode"));
  delay(60*1000);
  mySerial.flush();
}
void getRfPocketRatio(){
  Serial.println(rf_pocketsLostCounter);
  Serial.println(rf_pocketsReceivedCounter);
  Serial.println(((float)rf_pocketsLostCounter/(float)rf_pocketsReceivedCounter));
}

void setWIFi(){
   char charBuf1[10];
   char charBuf2[10];
   Serial.readStringUntil('\n').toCharArray(charBuf1, 10);
    Serial.readStringUntil('\n').toCharArray(charBuf2, 10);
   WiFi.begin(charBuf1, charBuf2);
   delay(7000);
  clearDisplay();
   if(WiFi.status() == WL_CONNECTED){
    display.println("success");
    }else{
     display.println("fail");
   }
   displayA();

}
void setBmpFilter(){
  switch(Serial.read()){
    case 0:
       bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
       Adafruit_BMP280::SAMPLING_X2,   
       Adafruit_BMP280::SAMPLING_X16,  
       Adafruit_BMP280::FILTER_OFF,   
       Adafruit_BMP280::STANDBY_MS_1);
       break;
    case 1: 
       bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
       Adafruit_BMP280::SAMPLING_X2,   
       Adafruit_BMP280::SAMPLING_X16,  
       Adafruit_BMP280::FILTER_X8,   
       Adafruit_BMP280::STANDBY_MS_1);
       break;
   case 2: 
       bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
       Adafruit_BMP280::SAMPLING_X2,   
       Adafruit_BMP280::SAMPLING_X16,  
       Adafruit_BMP280::FILTER_X16,   
       Adafruit_BMP280::STANDBY_MS_1);
        break;
  }
  delay(200);
}
void getColorMode() {
  switch(Serial.read()) {
  case 0:
    colorModesRGB[0][0] = Serial.read();
    colorModesRGB[0][1] = Serial.read();
    colorModesRGB[0][2] = Serial.read();
    saveByte((byte) colorModesRGB[0][0],1);
    saveByte((byte)colorModesRGB[0][1],2);
    saveByte((byte)colorModesRGB[0][2],3);
    break;
  case 1:
    colorModesRGB[1][0] = Serial.read();
    colorModesRGB[1][1] = Serial.read();
    colorModesRGB[1][2] = Serial.read();
    saveByte((byte)colorModesRGB[1][0],4);
    saveByte((byte)colorModesRGB[1][1],5);
    saveByte((byte)colorModesRGB[1][2],6);
    break;
  case 2:
    colorModesHSV[0] = Serial.read();
    saveByte((byte)colorModesHSV[0],7);
    break;
  case 3:
    colorModesHSV[1] = Serial.read();
    saveByte((byte)colorModesHSV[1],8);
    break;
  }
}
void getMaxima(float &q, float &z) {
  byte a,b;

  byte val = Serial.read();
  if(val < 9){
    rangeMode[val] = true;
  }
  switch(val) {
  case 0: 
    maxInsideTemp[1] =Serial.read();
    minInsideTemp[1] = Serial.read();
    q = maxInsideTemp[1];
    z = minInsideTemp[1];
    saveByte((byte)maxInsideTemp[1],9);
    saveByte((byte)minInsideTemp[1],10);
    break;
  case 1:
    maxInsideHum[1] = Serial.read();
    minInsideHum[1] = Serial.read();
    q = maxInsideHum[1];
    z = minInsideHum[1];
    saveByte((byte)maxInsideHum[1] ,11);
    saveByte((byte)minInsideHum[1] ,12);
    break;
  case 2:
    a = Serial.read();
    b =Serial.read();
    maxPressure[1] = a+900;
    minPressure[1] = b+900;
    q = maxPressure[1];
    z = minPressure[1];
    saveByte((byte)a ,13);
    saveByte((byte)b ,14);
    break;
  case 3:
   // monthMax[_month] = Serial.read()*10;
 //   monthMin[_month] =Serial.read()*10;
    a = Serial.read();
    if(a >=50){
      a*=-1;
    }
    _max[1] = (float)(a) + (Serial.read()/100.0f);
     a = Serial.read();
    if(a >=50){
      a*=-1;
    }
    _min[1] = (float)(a) + (Serial.read()/100.0f);
    q = _max[1];
    z = _min[1];
    break;
  case 4:
    maxOutsideHum[1] = Serial.read();
    minOutsideHum[1] = Serial.read();
    saveByte((byte)maxOutsideHum[1] ,15);
    saveByte((byte)minOutsideHum[1] ,16);
    q = maxOutsideHum[1];
    z = minOutsideHum[1];
    break;
  case 5:
    maxAverangeWind[1] = Serial.read();
    saveByte((byte)maxAverangeWind[1] ,17);
    q = maxAverangeWind[1];
    z = 0;
    break;
  case 6:
    maxWind[1] =Serial.read();
    saveByte((byte)maxWind[1] ,18);
    q = maxWind[1];
    z = 0;
    break;
  case 7:
    maxRain[1] = Serial.read();
    saveByte((byte)maxRain[1] ,19);
    q = maxRain[1];
    z = 0;
    break;
  case 8:
    a = Serial.read();
    maxPressureChange[1] = maxPressureChange[0]*((float)a/100.0f);
    q = maxPressureChange[1];
    z = minPressureChange[0];
    break;
  }
}
void setColor(boolean q) {
  wp=1;
  setLedColor(colorMode);
  ledAnalogWrite();
  if (q&&!clockColorMode) {
    displayColorMode();
  }
  checkRealTimeWind(); 
   pressureChangeEnabled = checkPressureChange(colorMode);
}

boolean checkPressureChange(const int _mode){
  switch(_mode){
   case 0:if(compare2(colorModesRGB[0][0])||compare2(colorModesRGB[0][1])||compare2(colorModesRGB[0][2])) return true; break;
   case 1:if(compare2(colorModesRGB[1][0])||compare2(colorModesRGB[1][1])||compare2(colorModesRGB[1][2])) return true; break;
   case 2:if(compare(colorModesHSV[0]))return true; break;
   case 3:if(compare(colorModesHSV[1])) return true; break;
   default: return false;
  }
  return false;
}
boolean compare(const int8_t val){
  const int8_t th = 9;
  const int8_t c = 10;
  const int8_t c2 = 20;
  return (val >th||val >th+c||val > th+c2)?true:false;
}

boolean compare2(const int8_t val){
  const int8_t th = 9;
  return (val ==th)?true:false;
}

void sendSettings(boolean state) {
  byte bytes[42];
  bytes[0] = byte(brightness*100);
  bytes[1] = byte(colorMode);
  bytes[2] = maxInsideTemp[1];
  bytes[3] = minInsideTemp[1];
  bytes[4] = maxInsideHum[1];
  bytes[5] = minInsideHum[1];
  bytes[6] = byte(maxPressure[1] - 900);
  bytes[7] = byte(minPressure[1] - 900);
   bytes[8] = byte((maxPressureChange[1]/maxPressureChange[0])*100);
   bytes[9] = byte((maxPressureChange[1]/minPressureChange[0])*100);
  bytes[10] = maxOutsideHum[1];
  bytes[11] = minOutsideHum[1];
  bytes[12] = maxAverangeWind[1];
  bytes[13] = maxWind[1];
  bytes[14] = maxRain[1];
  bytes[15] = byte(sendInterval/10000);
  bytes[16] = byte(0); /// todo
  bytes[17] = byte(0); //todo
  bytes[18] = byte(startDimHour);
  bytes[19] = byte(stopDimHour);
  bytes[20] = colorModesRGB[0][0];
  bytes[21] = colorModesRGB[0][1];
  bytes[22] = colorModesRGB[0][2];
  bytes[23] = colorModesRGB[1][0];
  bytes[24] = colorModesRGB[1][1];
  bytes[25] = colorModesRGB[1][2];
  bytes[26] = colorModesHSV[0];
  bytes[27] = colorModesHSV[1];
  bytes[28] =byte(networkOnlyMode);
  if (state) {
    bytes[49] = byte(autoRangeEnabled);
    Serial.write(bytes, 29);
  } else {
    bytes[29] = floor(temp);
    bytes[30] = ((temp - floor(temp))*100);
    bytes[31] = floor(hum);
    bytes[32] = ((hum - floor(hum))*100);
    bytes[33] = floor(pres - 900.0f);
    bytes[34] = ((pres - floor(pres))*100);
    bytes[35] = floor(shtTempO);
    bytes[36] = ((shtTempO- floor(shtTempO))*100);
    bytes[37] = shtHumO;
    bytes[38] = currentWind;
    bytes[39] = floor(averangeWind);
    bytes[40] = ((averangeWind - floor(averangeWind))*100);
    bytes[41] = rain;
    bytes[42] = byte(autoRangeEnabled);
    Serial.write(bytes, 42);
  }
  Serial.flush();
}

boolean checkResponse(boolean show){
  unsigned long t = millis();
  unsigned long t2 = millis();
  while(millis() < t+ SERIAL_TIMEOUT){
    if(mySerial.available()){
      if(mySerial.read() == 1){
        return true;
      }else{
        return false;
      }        
    }
    if(show){
      if(millis() > t2+ 5){
           drawLoadingBar(map(millis() - t,0,SERIAL_TIMEOUT,0,128),true);
        t2 = millis();
      }     
    }
    delay(1);
  }
  if(!show){
    clearDisplay();
    display.println(F("Trasmitter error"));
    displayA();
    delay(3000);
  }
  return false;
}

int getResponse(){
  unsigned long t = millis();
  while(millis() < t+ SERIAL_TIMEOUT){
    if(mySerial.available()){
        return mySerial.parseInt();
    }
    delay(1);
  }
  clearDisplay();
  display.println(F("Trasmitter error"));
  displayA();
  delay(300);
  return -1;
}

void saveByte(byte val,int address){
   EEPROM.begin(address+15);
   delay(5);
   EEPROM.put(address, val);
   EEPROM.end();
}
