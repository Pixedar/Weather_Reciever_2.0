#define iconXmargin  60
#define iconYmargin  3
#define textXmarginB  80
#define textXmarginA 18
#define textYmargin  8
#define iconSize 16
#define SEG_UPDATE_INERVAL 60000
uint8_t segmentsIndex = 1;
unsigned long segmentUpdateTime = 0;
const int barWidth = 128;

const unsigned char therm [] = {
0x03, 0xC0, 0x02, 0x40, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60,
0x07, 0xE0, 0x07, 0xE0, 0x0D, 0xB0, 0x0B, 0xD0, 0x0B, 0xD0, 0x0D, 0xB0, 0x06, 0x60, 0x03, 0xC0
};

const unsigned char wind [] = {
0x00, 0x60, 0x00, 0x10, 0x00, 0x12, 0xFF, 0xE1, 0x00, 0x01, 0x00, 0x02, 0xFF, 0xFC, 0x00, 0x00,
0x00, 0x00, 0xFF, 0xFC, 0x00, 0x02, 0x00, 0x01, 0xFF, 0xE1, 0x00, 0x12, 0x00, 0x10, 0x00, 0x60
};

const unsigned char barometer [] = {
0x07, 0xE0, 0x1C, 0x38, 0x30, 0x0C, 0x60, 0x06, 0x48, 0x02, 0xC4, 0x03, 0x83, 0x81, 0x82, 0x41,
0x82, 0x41, 0x81, 0xC1, 0xC0, 0x03, 0x40, 0x02, 0x60, 0x06, 0x30, 0x0C, 0x1C, 0x38, 0x07, 0xE0
};


const unsigned char drop2 [] = {
0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x06, 0x60, 0x0C, 0x30, 0x08, 0x10, 0x18, 0x18, 0x30, 0x0C,
0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x30, 0x0C, 0x38, 0x1C, 0x1E, 0x78, 0x0F, 0xF0, 0x03, 0xC0
};

const unsigned char rainIcon [] = {
0x07, 0x80, 0x0C, 0xC0, 0x18, 0x60, 0x10, 0x3C, 0x30, 0x06, 0x70, 0x03, 0xC0, 0x01, 0x80, 0x01,
0x80, 0x03, 0xC0, 0x06, 0x7F, 0xFC, 0x08, 0xC0, 0x08, 0xC0, 0x02, 0x10, 0x02, 0x10, 0x02, 0x10
};

const unsigned char therm2 [] = {
0x03, 0xC0, 0x02, 0x40, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60,
0x06, 0x60, 0x06, 0x60, 0x0C, 0x30, 0x08, 0x10, 0x08, 0x10, 0x0C, 0x30, 0x06, 0x60, 0x03, 0xC0
};
int yMargins[2];
//yMargintext = textYmargin
//yMarginIcon = 0
int xQ = 7;
int yQ = 22;
int sizeQ = 0;
int sizeW = 0;
void drawTempSegment(int yMargintext, int yMarginIcon){
   display.setCursor(textXmarginA,yMargintext);
   display.println(String(shtCurrentTemp,1)+" C");
   display.setCursor(textXmarginB,yMargintext);
   display.println(String(tempO,1) +" C");
   display.drawBitmap(0,yMarginIcon, therm2, iconSize , iconSize , 1);
   display.drawBitmap(iconXmargin,yMarginIcon, therm2, iconSize , iconSize , 1);
   display.fillCircle(iconXmargin,yMarginIcon+2,1,WHITE);
//   if((char)Serial.read() == 'a'){
//      xQ--;
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//     Serial.println(sizeW);
//   }
//   if((char)Serial.read() == 'd'){
//      xQ++;
//      Serial.println(textYmargin+5+2*iconSize);
//      Serial.println(2*iconSize +iconYmargin*3);
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//     Serial.println(sizeW);
//   }
//    if((char)Serial.read() == 'w'){
//      yQ++;
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//     Serial.println(sizeW);
//   }
//    if((char)Serial.read() == 's'){
//      yQ--;
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//     Serial.println(sizeW);
//   }
//   if((char)Serial.read() == 'q'){
//      sizeQ--;
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//     Serial.println(sizeW);
//   }
//    if((char)Serial.read() == 'e'){
//      sizeW++;
//         Serial.println(xQ);
//   Serial.println(yQ);
//   Serial.println(sizeQ);
//   Serial.println(sizeW);
//   }


if(shtCurrentTemp !=0){
float sizeA = round(map(shtCurrentTemp,minInsideTemp[0],maxInsideTemp[0],0,11));
if(sizeA > 2){
   display.fillRect(6,30- (iconSize+iconYmargin) + yMarginIcon,4,2,WHITE);
}
    display.drawLine(7,22+11 - (iconSize+iconYmargin) + yMarginIcon,7,22+11-sizeA- (iconSize+iconYmargin) + yMarginIcon,WHITE);
    display.drawLine(7+1,22+11- (iconSize+iconYmargin) + yMarginIcon,7+1,22+11-sizeA- (iconSize+iconYmargin) + yMarginIcon,WHITE);   
}
float sizeB = round(map(tempO,_min[0],_max[0],0,11));
if(sizeB >2){
      display.fillRect(6+iconXmargin,30- (iconSize+iconYmargin) + yMarginIcon,4,2,WHITE);
}
    display.drawLine(7+iconXmargin,22+11 - (iconSize+iconYmargin) + yMarginIcon,7+iconXmargin,22+11-sizeB- (iconSize+iconYmargin) + yMarginIcon,WHITE);
    display.drawLine(7+1+iconXmargin,22+11- (iconSize+iconYmargin) + yMarginIcon,7+1+iconXmargin,22+11-sizeB- (iconSize+iconYmargin) + yMarginIcon,WHITE);
}
//yMargintext = textYmargin+iconSize+textYmargin*0.5
//yMarginIcon = iconSize+iconYmargin;
void drawHumSegment(int yMargintext, int yMarginIcon){
   display.setCursor(textXmarginA,yMargintext);
   display.println(String(shtCurrentHum,1)+"%");
  display.drawBitmap(0,yMarginIcon, drop2, iconSize , iconSize, 1); 
  display.setCursor(textXmarginB,yMargintext);
   if(humO ==100){
           display.println(String(rainSum,1)+" mm/h");
          display.drawBitmap(iconXmargin,yMarginIcon, rainIcon, iconSize , iconSize, 1);
   }else{
       display.println(String(humO)+"%");
       display.drawBitmap(iconXmargin,yMarginIcon, drop2, iconSize , iconSize, 1);  
       display.fillCircle(iconXmargin,yMarginIcon,1,WHITE); 
   }

     int8_t sizeQ = round(map(shtCurrentHum,minInsideHum[0],maxInsideHum[0],-1,-9));
      int8_t sizeW = round(map(humO,minOutsideHum[0],maxOutsideHum[0],-1,-9));

      //54
   for(int8_t k =0; k < 8;k++){
    if(sizeQ !=-1&& shtCurrentHum != 0.0){
       display.drawLine(4+k,13+yMarginIcon,4+k,13+yMarginIcon+sizeQ,WHITE);
    }
    if(sizeW !=-1&& humO!=100){
        display.drawLine(4+k+iconXmargin,13+yMarginIcon,4+k+iconXmargin,13+yMarginIcon+sizeW,WHITE);
    }
   }
    
}
//yMargintext = textYmargin+5+2*iconSize
//yMarginIcon = 2*iconSize +iconYmargin*3
void drawWindSegment(int yMargintext, int yMarginIcon){
      display.setCursor(textXmarginA,yMargintext);
      display.println(String(maxCurrentWind)+" kph");      
       display.drawBitmap(0,yMarginIcon , wind, iconSize , iconSize, 1);
       
     display.drawBitmap(iconXmargin,yMarginIcon, barometer, iconSize , iconSize, 1);

       display.setCursor(textXmarginB,yMargintext);
       display.println(String(currentPressure/100.0f,1) + "hPa");
}
void updateMargins(int index){
    switch(index){
      case 1:
      yMargins[0] = textYmargin;
      yMargins[1] = 0;
      break;
      case 2:
      yMargins[0] = textYmargin+iconSize+textYmargin*0.5;
      yMargins[1] = iconSize+iconYmargin;
      break;
      case 3:
      yMargins[0] = textYmargin+5+2*iconSize;
      yMargins[1] = 2*iconSize +iconYmargin*3;
      break;
    }
 }
 void displaySegment(int index){
    switch(index){
      case 0:drawTempSegment(yMargins[0],yMargins[1]);break;
      case 1:drawHumSegment(yMargins[0],yMargins[1]);break;
      case 2:drawWindSegment(yMargins[0],yMargins[1]);break;
    }
 }
uint8_t incSegIndex(uint8_t index){
  index++;
  if(index>3){
    index = 1;
  }
  return index;
}
void updateWeatherSegments(){
  if(millis() > segmentUpdateTime + SEG_UPDATE_INERVAL){
    segmentsIndex = incSegIndex(segmentsIndex);
    segmentUpdateTime =millis();
  }
}

 void drawLoadingBar(float progress,boolean showCircle){
  const int barMargin = 60 - 1;
  const int radius = 2;
  display.drawLine(0,barMargin,progress,barMargin,WHITE);
  if(showCircle){
  display.fillCircle(progress,barMargin,radius,WHITE);
  }
  displayA();
}
void clearLoadingBar(){
    display.drawLine(0,60-1,128,60-1,BLACK);
   display.fillRect(0,61,128,4,BLACK);
   displayA();
}
void displayWeatherData(boolean flag){
  if(!clockColorMode&&!displayHeartBeatEnabled){  
 //   while(true){
    clearDisplay();  
      uint8_t tmp = segmentsIndex;
      for(uint8_t j =0; j < 3; j++){
        updateMargins(incSegIndex(tmp));
        tmp = incSegIndex(tmp);
        displaySegment(j);
      }
      if(flag){
          displayA(); 
      }
   //      delay(2);
    // }
  
  }
  }

      
        