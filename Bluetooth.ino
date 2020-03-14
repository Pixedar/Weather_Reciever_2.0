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
          a =Serial.read();
          sendInterval = a*10000; 
          mySerial.write((byte)2);
          mySerial.write(lowByte(sendInterval/200));
          mySerial.write(highByte(sendInterval/200));
         // mySerial.print(sendInterval/2);
          if(checkResponse(false)){         
          clearDisplay();
          display.print(F("Sending interval: "));
          display.print(String(sendInterval));
          displayA(); 
          saveByte(a ,20);
         }       
          break;
        case 6:
          b = Serial.read();
          dhtInterval =b*100;
          //mySerial.print("2;"+String(b*1000/2));
          mySerial.write((byte)3);
          mySerial.write(lowByte(dhtInterval/200));
          mySerial.write(highByte(dhtInterval/200));
          if(checkResponse(false)){
          clearDisplay();
          display.print(F("Oversampling: "));
          display.print(dhtInterval);
          displayA();
          saveByte(b ,21);
          }
          break;
        case 7:
          c = Serial.read();
          windInterval = c*50;
          mySerial.write((byte)4);
          mySerial.write(lowByte(windInterval/200));
          mySerial.write(highByte(windInterval/200));
          if(checkResponse(false)){
          clearDisplay();
          display.print(F("Oversampling: "));
          display.print(b);
          displayA();
          saveByte(c ,22);
          }
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
          delay(20);
          initAutoRange();  
          Serial.println(_month);
          Serial.println(_day);
          Serial.println(absoluteMaxTemp);
          Serial.println(absoluteMinTemp);
          Serial.println(maxInsideTemp[0]);
          Serial.println(minInsideTemp[0]);
          Serial.println(maxInsideHum[0]);
          Serial.println(minInsideHum[0]);
          Serial.println(maxPressure[0]);
          Serial.println(minPressure[0]);
          Serial.println(maxPressureChange[0]);
          Serial.println(minPressureChange[0]); 
          Serial.println(maxPressureChange[1]);
          Serial.println(minPressureChange[1]);
          Serial.println(F("end"));    
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
          Serial.println(WiFi.disconnect());
          break;
        case 38:
          WiFi.mode(WIFI_STA);
          connectToWiFi();
     //      WiFi.connect();
          break;
        case 39:
          resetWiFiConnection = true;
          break;
        case 40:
            ThingSpeak.begin(client);
            break;
        case 41:
          testClient();
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

        }
      
        Serial.flush();
        if (realTimeWind) {
          mySerial.flush();
        }
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

void testClient(){
  int conuterA = 0;
  while(true){
       if(WiFi.status() == WL_CONNECTED){
        Serial.println(F("connected"));
       }else{
        Serial.println(WiFi.status());
       }
     int x = ThingSpeak.writeField(556803, 1, conuterA, "V9BDERAUY8VGWJU7");
     Serial.println(x);
      delay(30000);
     long l = ThingSpeak.readLongField(556803, 1, "V9BDERAUY8VGWJU7");  
     Serial.println(l);
     x = ThingSpeak.getLastReadStatus();
     Serial.println(x);
     delay(30000);
    conuterA++;
   if(conuterA > 1){
      WiFi.disconnect();
      delay(2000);
       if(WiFi.status() == WL_CONNECTED){
        Serial.println(F("connected"));
       }else{
        Serial.println(WiFi.status());
       }
      WiFi.begin(ssid, pass);
      displayWiFiConnecting();  
    }
    if(conuterA > 2){
      ThingSpeak.flushClient();
    }
    if(conuterA > 3){
      client.flush();
    }
  }
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
//  bytes[8] = byte(monthMax[_month]/10);
 // bytes[9] = byte(monthMin[_month]/10);
   bytes[8] = byte((maxPressureChange[1]/maxPressureChange[0])*100);
   bytes[9] = byte((maxPressureChange[1]/minPressureChange[0])*100);
// if(autoTempRangeMode){
//  bytes[8] = byte(_max[1]*10);
//  bytes[9] = byte(_min[1]*10);
// }else{
//  bytes[8] = byte(_max[1]/10);
//  bytes[9] = byte(_min[1]/10);
// }
  bytes[10] = maxOutsideHum[1];
  bytes[11] = minOutsideHum[1];
  bytes[12] = maxAverangeWind[1];
  bytes[13] = maxWind[1];
  bytes[14] = maxRain[1];
  bytes[15] = byte(sendInterval/10000);
  bytes[16] = byte(dhtInterval/1000);
  bytes[17] = byte(windInterval/50);
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
    bytes[35] = floor(tempO);
    bytes[36] = ((tempO - floor(tempO))*100);
    bytes[37] = humO;
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
       // display.print(F("."));
        //display.fillRect(0,61,map(millis() - t,0,SERIAL_TIMEOUT,0,128),3,WHITE);
       // displayA();
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
//void btSendSettings(){
//  int8_t d = 4;
//  int8_t h = 0;
//  byte bytes[38];
//  bytes[0] = byte(brightness*100);
//  bytes[1] = byte(colorMode);
//  setLedColor(0);
//  bytes[2] = byte(r/d);
//  bytes[3] = byte(g/d);
//  bytes[4] = byte(b/d);
//  setLedColor(1);
//  bytes[5] = byte(r/d);
//  bytes[6] =byte(g/d);
//  bytes[7] = byte(b/d);
//  setLedColor(2);
//  bytes[8] = byte(r/d);
//  bytes[9] = byte(g/d);
//  bytes[10] = byte(b/d);
//  setLedColor(3);
//  bytes[11] = byte(r/d);
//  bytes[12] = byte(g/d);
//  bytes[13] = byte(b/d);
//  setLedColor(colorMode);
//  bytes[14] = maxInsideTemp;
//  bytes[15] = minInsideTemp;
//  bytes[16] = maxInsideHum;
//  bytes[17] = minInsideHum;
//  bytes[18] = byte(maxPressure - 900);
//  bytes[19] = byte(minPressure - 900);
//  bytes[20] = byte(monthMax[_month]/10);
//  bytes[21] = byte(monthMin[_month]/10);
//  bytes[22] = maxOutsideHum;
//  bytes[23] = minOutsideHum;
//  bytes[24] = maxAverangeWind;
//  bytes[25] = maxWind;
//  bytes[26] = maxRain;
//  bytes[27] = byte(sendInterval/8000);
//  bytes[28] = byte(dhtInterval/1000);
//  bytes[29] = byte(windInterval/50);
//  bytes[30] = byte(startDimHour);
//  bytes[31] = byte(stopDimHour);
//  bytes[32] = colorModesRGB[0][0];
//  bytes[33] = colorModesRGB[0][1];
//  bytes[34] = colorModesRGB[0][2];
//  bytes[35] = colorModesRGB[1][0];
//  bytes[36] = colorModesRGB[1][1];
//  bytes[37] = colorModesRGB[1][2];
//  bytes[38] = colorModesHSV[0];
//  bytes[39] = colorModesHSV[1];
//  bytes[40] =byte(recieveDataFormInternet);
//  Serial.write(bytes,41);
////Serial.write(bytes[0]);
////Serial.write(bytes[1]);
////Serial.write(bytes[2]);
////Serial.write(bytes[3]);
////Serial.write(bytes[4]);
// // Serial.write(byte(colorMode));
//  Serial.flush();
//}

