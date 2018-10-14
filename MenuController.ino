void buttonControler(){
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if(!buttonState){
        setLedColor(colorMode); 
        checkRealTimeWind();      
          colorMode++;
            if(colorMode > 3){
              colorMode = 0;
            }
        delay(100);
        buttonTime = millis();
        displayTime+=3600;
       lock = true;
    }
    buttonState = true;  
  } else {
    buttonState = false;  
  }
  if(millis() > buttonTime + 3600&& lock){
     clearDisplay();
     display.println("Home:    " + String(temp,1) +"C  " +String(hum,1)+"%");
     display.println("Outside: " + String(tempO,1) +"C  "+String(humO)+"%");
     display.println(String(pres,1) + "hPa "+String(rain)+"mm    "+String(maxCurrentWind)+"kmh");      
     display.display(); 
     lock = false;
  }
}
void checkRealTimeWind(){
  boolean enable = false;
  switch(colorMode){
    case 0:
     for(int k =0; k < 3;k++){
      if(colorModesRGB[0][k] == 7){
         enable = true;
         break;
      }
    }
    break;
   case 1:
     for(int k =0; k < 3;k++){
      if(colorModesRGB[1][k] == 7){
         enable = true;
         break;
      }
    }
    break;
    case 2: 
    if(colorModesHSV[0] ==7){
      enable = true;
    }
    break;
    case 3:
    if(colorModesHSV[1] ==7){
      enable = true;
    }
    break;
  }
  if(enable&&!realTimeWind){
    mySerial.write((byte)6);
    mySerial.write((byte)1);
    if(checkResponse(false)){
      realTimeWind =true;
    }
  }else if(!enable&&realTimeWind){
    mySerial.write((byte)6);
    mySerial.write((byte)0);
    if(checkResponse(false)){
      realTimeWind =false;
    }
  }
}

void displayColorMode(){
          clearDisplay(); 
          if(colorMode <2){
          display.print(getColorName(colorModesRGB[colorMode][0]));
          display.println(String(map(r,0,1023,0,100))+"%");
          display.print(getColorName(colorModesRGB[colorMode][1]));
          display.println(String(map(g,0,1023,0,100))+"%");
          display.print(getColorName(colorModesRGB[colorMode][2]));
          display.println(String(map(b,0,1023,0,100))+"%");
          }else{
            display.print(F("hsv: "));
            display.println(getColorName(colorModesHSV[colorMode -2]));
            display.println(String(map(r,0,1023,0,100)) +"% " +String(map(g,0,1023,0,100)) +"% " +String(map(b,0,1023,0,100))+"%");
          }
          display.display();

}
void displayBrightness(){
  clearDisplay();
  display.println("       Brightness ");
  display.fillRect(0,10,map(brightness*100,0,100,0,128),3,WHITE);
  display.println();
  display.println("          " +String((int)(brightness*100))+"%");
   display.display();
  //delay(700);
}

