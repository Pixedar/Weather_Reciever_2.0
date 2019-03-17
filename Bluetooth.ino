void readBluetoothCommands() {
  if (Serial.available()) {
    unsigned long t = millis();
    byte a,b,c,d;
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
          display.display();
          break;
        case 4:
          getMaxima();
          setColor(false);
          clearDisplay();
          display.print(F("Max/Min has been set"));
          display.display();
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
          display.display(); 
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
          display.display();
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
          display.display();
          saveByte(c ,22);
          }
          break;
        case 8:
          startDimHour = Serial.read();
          stopDimHour = Serial.read();
          startDimMinute = Serial.read();
          stopDimMinute = Serial.read();
          clearDisplay();
          display.print(F("Turn off time has been set"));
          display.display();
          saveByte((byte)startDimHour ,23);
          saveByte((byte)stopDimHour ,24);
          saveByte((byte)startDimMinute ,25);
          saveByte((byte)stopDimMinute ,26);
          break;
        case 9:
          recieveDataFormInternet = boolean(Serial.read());
          clearDisplay();
          display.println(F("RecieveDataFormInternet: "));
          display.print(String(recieveDataFormInternet));
          display.display();
          saveByte((byte)recieveDataFormInternet  ,27);
          break;
        case 10:
          if(boolean(Serial.read())){
            display.testDim(1);
          }else{
            display.testDim(0);
          }
          break;
        case 12:   
          homeDhtInterval = 89000;    /////DEBUG ONLY !!!!!!!!!!!!!!!!        
          getHearBeatParams(Serial.read());
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
            display.display();
        case 15: /////DEBUG ONLY !!!!!!!!!!!!!!!!
          homeDhtInterval = 89000;
          break;
        case 16:
          autoRangeEnabled = boolean(Serial.read());
          break;
        case 17:
           executeOTA();
           break;
        case 18:
           ESP.restart();  
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
        displayWeatherData();
      }
      mySerial.flush();  
    } 
  }
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
void getMaxima() {
  byte a,b;
  switch(Serial.read()) {
  case 0: 
    maxInsideTemp[1] =Serial.read();
    minInsideTemp[1] = Serial.read();
    saveByte((byte)maxInsideTemp[1],9);
    saveByte((byte)minInsideTemp[1],10);
    break;
  case 1:
    maxInsideHum[1] = Serial.read();
    minInsideHum[1] = Serial.read();
    saveByte((byte)maxInsideHum[1] ,11);
    saveByte((byte)minInsideHum[1] ,12);
    break;
  case 2:
    a = Serial.read();
    b =Serial.read();
    maxPressure[1] = a+900;
    minPressure[1] = b+900;
    saveByte((byte)a ,13);
    saveByte((byte)b ,14);
    break;
  case 3:
   // monthMax[_month] = Serial.read()*10;
 //   monthMin[_month] =Serial.read()*10;
    _max[1] =Serial.read()*10;
    _min[1] = Serial.read()*10;
    break;
  case 4:
    maxOutsideHum[1] = Serial.read();
    minOutsideHum[1] = Serial.read();
    saveByte((byte)maxOutsideHum[1] ,15);
    saveByte((byte)minOutsideHum[1] ,16);
    break;
  case 5:
    maxAverangeWind[1] = Serial.read();
    saveByte((byte)maxAverangeWind[1] ,17);
    break;
  case 6:
    maxWind[1] =Serial.read();
    saveByte((byte)maxWind[1] ,18);
    break;
  case 7:
    maxRain[1] = Serial.read();
    saveByte((byte)maxRain[1] ,19);
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
 if(autoTempRangeMode){
  bytes[8] = byte(_max[1]*10);
  bytes[9] = byte(_min[1]*10);
 }else{
  bytes[8] = byte(_max[1]/10);
  bytes[9] = byte(_min[1]/10);
 }
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
  bytes[28] =byte(recieveDataFormInternet);
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
      if(millis() > t2+ 90){
        display.print(F("."));
        display.display();
        t2 = millis();
      }     
    }
    delay(1);
  }
  if(!show){
    clearDisplay();
    display.println("Trasmitter error");
    display.display();
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
  display.println("Trasmitter error");
  display.display();
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

