#define FLUID_DIMING_INTERVAL 100
boolean dimmingFlag = false;
boolean isDST;
unsigned long fluidDimmingTime =0;
boolean way = false;
void updateFluidDimming(){
  if(dimmingFlag&&millis() > fluidDimmingTime +FLUID_DIMING_INTERVAL){
    if(way){
      if(brightness <lastBrightness){
        brightness+=(double)lastBrightness/((fluidDimming*60000.0f)/(float)(FLUID_DIMING_INTERVAL));
      }else{
        brightness = lastBrightness;
        turnOffLed = false;
        dimmingFlag = false;
      }
    }else{
      if(brightness>0.0f){
        brightness-=(double)lastBrightness/((fluidDimming*60000.0f)/(float)(FLUID_DIMING_INTERVAL));
      }else{
        brightness =0.0f;
             clearDisplay();
             display.display();
            displayEnabled = false;
        turnOffLed = true;
        dimmingFlag = false;
      }
    }
    ledAnalogWrite();
    fluidDimmingTime = millis();
  }
}

void turnOffLedAtSpecyficTime(){
  int startTime = startDimHour*60+startDimMinute;
  int stopTime = stopDimHour*60+stopDimMinute;

   if(_hour*60+_minute>=startTime&&!turnOffLed){
      if(fluidDimming <1){
       clearDisplay();
       display.display();
      displayEnabled = false;
      lastBrightness = brightness;
      brightness =0;
      turnOffLed = true;
      }else{
     //   display.testDim(50);
        if(!dimmingFlag){
          lastBrightness = brightness;
        }
        dimmingFlag = true;
        way = false;
      }
         
   }
   if(_hour*60+_minute>=stopTime&&turnOffLed){
    if((stopTime<startTime&&_hour*60+_minute<startTime)||stopTime>startTime){
          
        if(fluidDimming <1){
             displayEnabled = true;
          brightness = lastBrightness;
          turnOffLed = false;
        }else{
         displayEnabled = true;
           dimmingFlag = true;
           way = true;
        }
    }      
   }
}
unsigned long getEpochTime(boolean includeDST){
  unsigned long result = epochTime +((millis() - timeOffset)/1000);
  if(checkDST()){
    return result+3600;
  }
  return result;
}
unsigned long updateTime(){
    unsigned long t  =getEpochTime(true);
   _minute = minute(t);
   _hour = hour(t);
   _second = second(t);
   return t;
}
void getDateTime(){

  ntpTest();  
  epochTime = timeClient.getEpochTime();
  timeOffset = millis();

                 
  _year = year(timeClient.getEpochTime());
  _month = month(timeClient.getEpochTime());
  _day = day(timeClient.getEpochTime());
  
  _minute = minute(timeClient.getEpochTime());
  _hour = hour(timeClient.getEpochTime()); 
  _second = second(timeClient.getEpochTime());


                 
//  isDST = checkDST();
//  if(!isDST){
//   _hour-=1; 
//  }
}


boolean checkDST(){
  return false; //////////////// to do
  int previousSunday = _day - (((epochTime / 86400) + 4) % 7);
  if(_month < 3 || _month > 11){
    return false;
  }else if(_month > 3 && _month < 11){
    return true;
  }else if(_month == 3){
    if(previousSunday >=8){
      return true;
    }else{
      return false;
    }
  }
  if(previousSunday <=0){
     return true;
  }else{
      return false;
 }
}

 void ntpTest(){
  int y = 2020; //todo
//   EEPROM.begin(512);
//   EEPROM.get(500,y);
//   EEPROM.end();
  timeClient.update();
  if(year(timeClient.getEpochTime()) == y&&month(timeClient.getEpochTime()) !=0) return;


   if(ntpTestHelper("tempus2.gum.gov.pl",y)) return;
   if(ntpTestHelper("ntp1.tp.pl",y)) return;
   if(ntpTestHelper("tempus2.gum.gov.pl",y)) return;
   if(ntpTestHelper("ntp.itl.waw.pl",y)) return;

   if(year(timeClient.getEpochTime()) == y&&month(timeClient.getEpochTime()) !=0) return;
   
   clearDisplay();
   display.println("NTP error");
   displayA();
   delay(3500);
    
}
boolean ntpTestHelper(char* v,int y){
  if(year(timeClient.getEpochTime()) == y&&month(timeClient.getEpochTime()) !=0) return true;
  NTPClient timeClient(ntpUDP,v);
  timeClient.update();
  return false;
}
