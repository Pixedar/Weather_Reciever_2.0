int8_t numberOfSamples = 4;
int8_t hearBeatLedMode = 0;
int averangeHeartRate = 0;
byte maxHeartRate = 180;
boolean dynamicColor = true;
boolean changingColor = false;
int heartRateInterval =0;
int lastHeartRateInterval =0;
unsigned long hearBeatTime;
int interpolatedHeartRateInterval = 0;
int lastInterpolatedHeartRateInterval;
boolean flag  = true;
float interpolate = 0;
float heartBeatBrightness = 0;
#define LED_SAMPLES 40
#define MIN_HEART_RATE 40
#define HEART_RATE_SENDING_INTEVAL 7000

void displayHeartBeat(){
  //lastInterpolatedHeartRateInterval = interpolatedHeartRateInterval;
  interpolatedHeartRateInterval = interpolate*heartRateInterval+(1-interpolate)*lastHeartRateInterval;
  if(millis() > hearBeatTime + interpolatedHeartRateInterval/LED_SAMPLES){
                 clearDisplay();
    if(flag&&heartBeatBrightness <1.0f){
      heartBeatBrightness+=(2*brightness)/LED_SAMPLES;
    }else if(flag){
      flag = false;
    }
    if(!flag){
      if(heartBeatBrightness >0){
         heartBeatBrightness-=(2*brightness)/LED_SAMPLES;
      }else{
        flag =true;
      }
    }
    if(changingColor){
         HSV_to_RGB(map(heartBeatBrightness*1000,0,1000,135,30),100,100);
    }
    if(interpolate < 1){
         interpolate+= (float)(heartRateInterval/LED_SAMPLES)/HEART_RATE_SENDING_INTEVAL;
    }
    if(dynamicColor){
      analogWrite(BLUE_PIN,1023 -(interpolate*b+(1-interpolate)*lastB)*heartBeatBrightness);
      analogWrite(GREEN_PIN,1023 -(interpolate*g+(1-interpolate)*lastG)*heartBeatBrightness);
      analogWrite(RED_PIN,1023 -(interpolate*r+(1-interpolate)*lastR)*heartBeatBrightness);
    }else{
      analogWrite(BLUE_PIN,1023 -b*heartBeatBrightness);
      analogWrite(GREEN_PIN,1023 -g*heartBeatBrightness);
      analogWrite(RED_PIN,1023 -r*heartBeatBrightness);
    }
//    if(60000/interpolatedHeartRateInterval != 60000/lastInterpolatedHeartRateInterval){
//       displayHeartBeatValue(60000/interpolatedHeartRateInterval);
//    }
    hearBeatTime = millis();
  }
}

void getHearBeatParams(byte val){
   switch(val){
    case 0: readHeartRate(); break;
    case 1: readDynamicColor(); break;
    case 2: readStaticColor(); break;
    case 3: readNumberOfSamples(); break;
    case 4: readStopCommand(); break;
    default: Serial.flush(); break;
   }
}
 void readNumberOfSamples(){
  numberOfSamples = Serial.read();
}
 void readDynamicColor(){
  switch(Serial.read()){
    case 0: dynamicColor = true; changingColor = false; break;
    case 1: changingColor = true; dynamicColor = false; break;
  }
  maxHeartRate = Serial.read();
  if(dynamicColor){
     getHSVColor(); 
  }
  clearDisplay();
  display.println(F("Heart Beat Settings:"));
  display.println(F("Color mode: dynamic"));
  display.print(F("Max heart rate: "));
  display.println(String(maxHeartRate));
  displayA();
}
 void readHeartRate(){
  averangeHeartRate = Serial.read();
  lastHeartRateInterval = heartRateInterval;
  heartRateInterval = round(60000.0f/averangeHeartRate);   
  if(dynamicColor){
     lastR = r;
     lastG = g;
     lastB = b;
    getHSVColor();
  }
  interpolate = 0.0f;  
  if(!displayHeartBeatEnabled){
    displayHeartBeatEnabled = true;
   // interpolatedHeartRateInterval = heartRateInterval;
    display.setFont(&FreeSansBold12pt7b);
    lastHeartRateInterval = heartRateInterval;
    heartBeatBrightness = 0;
    lastR = r;
    lastG = g;
    lastB = b;
    hearBeatTime = millis();
  }
  displayHeartBeatValue(averangeHeartRate);
}
void displayHeartBeatValue(int value){
  display.clearDisplay();
  display.setCursor(52,22);
  display.print(value);
  displayA();
}
 void readStaticColor(){
   dynamicColor = false;
   changingColor = false;
   r = map(Serial.read(),0,255,0,1023);
   g = map(Serial.read(),0,255,0,1023);
   b = map(Serial.read(),0,255,0,1023);
  clearDisplay();
  display.println(F("Heart Beat Settings:"));
  display.println(F("Color: Static"));
  displayA();
}
void readStopCommand(){
  displayHeartBeatEnabled = false;
  display.setFont();
  clearDisplay();
  display.println(F("HeartBeat DISABLED"));
  displayA();
  setColor(false);
  delay(700);
  displayWeatherData(true);
  ledAnalogWrite();
}
void getHSVColor(){
   HSV_to_RGB(map(averangeHeartRate*averangeHeartRate,MIN_HEART_RATE*MIN_HEART_RATE,maxHeartRate*maxHeartRate,200,0),100,100);
}

