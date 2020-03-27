
void loadSettings(){
    byte a,b;
    EEPROM.begin(37);
    delay(12);
    a = EEPROM.read(0);
    if(a!=255){
    colorMode = a;
    }
    a = EEPROM.read(1);
    if(a!=255){
      colorModesRGB[0][0] = a;
    }
    a = EEPROM.read(2);
    if(a!=255){
    colorModesRGB[0][1] = a;
    }
    a = EEPROM.read(3);
    if(a!=255){
      colorModesRGB[0][2] = a;
    }
    a = EEPROM.read(4);
    if(a!=255){
    colorModesRGB[1][0] = a;
    }
    a = EEPROM.read(5);
    if(a!=255){
      colorModesRGB[1][1] = a;
    }
    a = EEPROM.read(6);
    if(a!=255){
      colorModesRGB[1][2] = a;
    }
    a = EEPROM.read(7);
    if(a!=255){
    colorModesHSV[0] =a;
    }
    a = EEPROM.read(8);
    if(a!=255){
    colorModesHSV[1] = a;
    }
//    a = EEPROM.read(9);
//    b = EEPROM.read(10);
//    if(a!= 255&&b!=255){
//      maxInsideTemp =a;
//      minInsideTemp = b;
//    }
//    a = EEPROM.read(11);
//    b = EEPROM.read(12);
//    if(a!= 255&&b!=255){
//      maxInsideTemp =a;
//      minInsideTemp = b;
//    }
//    a = EEPROM.read(13);
//    b = EEPROM.read(14);
//    if(a!= 255&&b!=255){
//      maxPressure = a+900;
//      minPressure = b+900;
//    }
//    a = EEPROM.read(15);
//    b = EEPROM.read(16);
//    if(a!= 255&&b!=255){
//        maxOutsideHum = a;
//        minOutsideHum = b;
//    }
//    a = EEPROM.read(17);
//    if(a!=255){
//       maxAverangeWind = a;
//    }
//    a = EEPROM.read(18);
//    if(a!=255){
//       maxWind = a;
//    }
//    a = EEPROM.read(19);
//    if(a!=255){
//       maxRain = a;
//    }
    a = EEPROM.read(20);
    if(a!=255){
      sendInterval = a*10000; 
    }
//    a = EEPROM.read(21);
//    if(a!=255){
//      dhtInterval =a*100; 
//    }
//    a = EEPROM.read(22);
//    if(a!=255){
//      windInterval =a*50; 
//    }
    a = EEPROM.read(23);
    if(a!=255){
      startDimHour =a; 
    }
     a = EEPROM.read(24);
    if(a!=255){
      stopDimHour =a; 
    }
     a = EEPROM.read(25);
    if(a!=255){
      startDimMinute =a; 
    }
    a = EEPROM.read(26);
    if(a!=255){
      stopDimMinute =a; 
    }
    a = EEPROM.read(28);
    networkOnlyMode =(boolean)a;
//    a = EEPROM.read(27);
//     if(a!=255){
//      recieveDataFormInternet = a;
//    } 
  EEPROM.end();
    setLedColor(colorMode);
    checkRealTimeWind();
    pressureChangeEnabled = checkPressureChange(colorMode);
}
