#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_SH1106.h>
#include "Wire.h"
#include "Time.h"
#include <TimeLib.h>
#include <EEPROM.h>
#include "ThingSpeak.h"
#include <Fonts/FreeSansBold12pt7b.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <math.h> 


#include <Arduino.h>
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define OLED_RESET 4
#define BUTTON_PIN 12
#define RED_PIN 13
#define GREEN_PIN 16
#define BLUE_PIN 15
//#define DHTPIN 14
#define SOFT_SERIAL_RX 5
#define SOFT_SERIAL_TX 4
#define BT_SERIAL_RX 10
#define BT_SERIAL_TX 9

#define DEBUG false
#define SERIAL_TIMEOUT 3500
#define SENDING_TIMEOUT 1100
#define SYNC_TIME_INTERVAL 86400000
#define DISPLAY_BRIGHTNESS_INTERVAL 800
#define PRESSURE_READ_INTERVAL 50
#define MAX_ACCEPTABLE_PRESSURE_CHANGE 10
#define SHT_READ_INTERVAL 30
#define PRESSURE_FIX -8.25f
#define RAIN_COEF 0.329
#define WIFI_CONNECTION_TIMEOUT 30000
//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_SH1106 display(OLED_RESET);
SoftwareSerial mySerial(SOFT_SERIAL_RX, SOFT_SERIAL_TX);
SoftwareSerial btSerial(BT_SERIAL_RX, BT_SERIAL_TX);
Adafruit_BMP280 bmp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"tempus1.gum.gov.pl");
WiFiClient client;


const char *ssid3 =  "OnePlus 7Pro"; 
const char *pass3 =  "W112Qzcx98";

const char *ssid2 =  "FunBox-72D6"; 
const char *pass2 =  "FF6EAD542DCE77FD357F2F6AC9";

const char* server = "api.thingspeak.com";

//const char *ssid3=  "AndroidAP";
//const char *pass3 =  "jyok8169";
const char *ssid =  "WLAN-7F8H56"; 
const char *pass =  "ifDii1BFF3Dfh0d";


float shtCurrentHum =0;
float shtCurrentTemp =0;
float lastShtCurrentHum =0;
float lastShtCurrentTemp =0;   
float shtIIRFilterCoef = 0.91f;

//const char *ssid3 ="AP-Media_6860";
//const char *pass3 ="cekincekin";
boolean enableKnob = false; //////
boolean realTimeWind = false;
boolean turnOffLed = false;
//boolean recieveDataFormInternet = false;
boolean autoTempRangeMode = true;
boolean pressureChangeEnabled = false;
boolean debugPressureChange = false; 
boolean networkOnlyMode = false;
boolean resetWiFiConnection = false;
boolean lightLock = false;

unsigned long pressureReadTime;
float pressureSamples;
float currentPressure =0;
float lastPressure;
float pressureChange =0;
float maxPressureChange[] = {49.21f,49.21f};
float minPressureChange[] = {0,0};
float interpolatedAverangeWind =0;

long sendInterval = 180000;
int dhtInterval = 3000;
int windInterval  = 492;
unsigned long dayTimeInterval = 60000;
int analogReadInterval = 300;
long checkTimeInterval = 3600*1000;
long colorChangeInterval = sendInterval/3;
int colorChangeIntervalResolution = 10;
float brightness = 1;
float lastBrightness = brightness;
int8_t startDimHour = 22;
int8_t startDimMinute = 22;
int8_t stopDimHour = 9;
int8_t stopDimMinute = 40;
int fluidDimming = 30;

boolean clockColorMode = false;
int8_t colorMode = 1;
int8_t colorModesRGB[2][3] = {{1,4,3},{2,3,1}};
int8_t colorModesHSV[2] = {0,6};
int monthMax[] = {73,97,157,233,270,300,337,340,293,203,133,87};
int monthMin[] = {-216,-213,-179,-60,6,68,94,90,38,-44,-181,-225}; 
boolean rangeMode[9];

float maxAverangeWind[]= {8,8};
float maxWind[] = {20,20}; 
float maxInsideTemp[] = {28,28};
float minInsideTemp[] = {19,19};
float maxInsideHum[] = {75,75};
float minInsideHum[] = {34,34};
float maxOutsideHum[] = {100,100};
float minOutsideHum[] = {10,10};
float maxRain[] = {8,9};
float maxPressure[] = {990,990};
float minPressure[] = {969,969};
boolean autoRangeEnabled = true;

int r;
int g;
int b;
int hsv_h_ext = 21;

float tempO =0;
byte humO = 0;

float averangeWind =0;
byte maxCurrentWind = 0;
byte currentWind = 0;
int windCounter = 0;
float windSum = 0;
//float lastWindA;

float temp = 0;
float hum = 0;
float pres = 0;
int samples = 0;

int rain =0;
float rainSum =0;
int lastHour =0;

float _max[] = {35,35};
float _min[] = {-20,-20};
uint8_t _day;
int8_t _month;
int _year;
boolean isDST;

int8_t _hour;
int8_t _minute;
int8_t _second;

int8_t lastSecond = 0;

boolean buttonState = false;
boolean startReading = false;
int8_t menuCounter = 0;
boolean lock = false;
//boolean dataNotReceived = false;
boolean signalReceived = false;
long timeoutInterval = SENDING_TIMEOUT;
unsigned long createdAtTime = 0;


boolean dataError = false;
float presSum =0;
float tempSum =0;
float humSum =0;

unsigned long dhtTime;
unsigned long sthTime;
unsigned long sendTime;
unsigned long displayTime;
unsigned long syncTime;
unsigned long buttonTime;
unsigned long long epochTime;
unsigned long timeOffset;
unsigned long pinTime;
unsigned long rgbTime;
unsigned long clockUpdateTime;
unsigned long analogReadTime;
unsigned long displayBrightnesTime;
unsigned long resetWiFiTime = 7200000;
float wp = 0;
int lastR = 0;
int lastB = 0;
int lastG =0;
float windColor = 0;

int analogValue = 0;
int lastAnalogValue = 0;
boolean displayHeartBeatEnabled = false;
boolean transmitCurrentPressure = false;
//Flash real id:   001640EF
//Flash real size: 4194304 bytes
//Flash ide  size: 4194304 bytes
//Flash ide speed: 40000000 Hz
//Flash ide mode:  QIO
boolean displayEnabled = true;
void displayA(){
  if(displayEnabled){
  display.display();
  }
}
void setup() 
{
  Serial.begin(115200);
  mySerial.begin(38400);

 // Serial.println(String(LED_BUILTIN));


  
  pinMode(RED_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  analogWrite(RED_PIN,1023); 
  analogWrite(GREEN_PIN,1023); 
  analogWrite(BLUE_PIN,1023); 


  Wire.begin(2,0);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  displayA(); 
  clearDisplay();
  displayA();     
  display.setTextSize(1.5);
  display.setTextColor(WHITE);

   
  while(!bmp.begin()){
    display.println(F("Pressure sensor error"));
    displayA();
    delay(100);
  }
 bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
 Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
 Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
 Adafruit_BMP280::FILTER_X16,      /* Filtering. */
 Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */

  if (!sht31.begin(0x44)) {   
    display.println("SHT31 error");
    displayA();
  }

  loadSettings();


// if(lightLock){
 /// display.testDim(0);
// }
lightLock =false;




  display.print(F("RF connecting"));
  if(initTrasmitter()){
    display.println(F(" OK"));
    networkOnlyMode = false;
  }else{
    display.println(F(" ERR"));
 //   display.println(F("WiFi Only Mode"));
       networkOnlyMode = true;
  }
  displayA();
 
  display.print(F("WiFi connecting"));
 
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  connectToWiFi(); 

  if(WiFi.status() == WL_CONNECTED){
    display.print(F(" OK"));
    display.println();
    display.print(F("Time Synh"));
    displayA();
    
    initOTA();
    timeClient.begin();
    timeClient.setTimeOffset(7200);   
    getDateTime();
    display.print(F(" OK"));
    display.println();
    
    display.print(F("Sending status"));
    sendError("RECEIVER RESET");
    displayA();
    display.print(F(" OK"));
    display.println();
     
    display.println(F("Loading Auto Range"));
    displayA();

    if(autoTempRangeMode){
     initAutoRange();
    }else{
      setMaxMin();
      delay(700);
    }
  
  }else{
    display.print(F(" ERR"));
     display.println(F("Offline Mode"));
    displayA();
  }



  clearDisplay();
  display.print(_year);
   display.print(F("-"));
  if(_month < 10){
   display.print(0); 
   display.print(_month); 
  }else{
   display.print(_month);  
  }
  display.print(F("-"));
  if(_day < 10){
    display.print(0);
    display.print(_day);
  }else{
     display.print(_day);
  }
  display.print(F("   "));
  display.print(_hour);
  display.print(F(":"));
  display.print(_minute);
  display.print(F(":"));
  display.print(_second);
  display.println();
  display.print(F("Interval: "));
  display.println(String(sendInterval/1000)+" s");
  display.print(F("Max temp: "));
  if(autoTempRangeMode){
      display.print(_max[0],1);
  }else{
      display.print(_max[0]/10.0f,1);
  }
  display.println(F(" C"));
  display.print(F("Min temp: "));
  if(autoTempRangeMode){
      display.print(_min[0],1);
  }else{
      display.print(_min[0]/10.0f,1);
  }
  display.print(F(" C"));
  displayA();
  delay(2700);
  
  turnOffLedAtSpecyficTime(); 
  
  analogValue = map(analogRead(A0),3,879,0,200); 
  lastAnalogValue = analogValue;
  brightness = 1 -(analogValue/200.0f);
     
//  tempSum= dht.readTemperature();
//  humSum= dht.readHumidity();
  tempSum = sht31.readTemperature();   
   humSum = sht31.readHumidity(); 
  presSum = bmp.readPressure();
  lastPressure = presSum;
  samples++;
  
  int tmp = fluidDimming;
  fluidDimming = 0;
  turnOffLedAtSpecyficTime();
  fluidDimming = tmp;
  
  
  displayBrightnesTime =millis();
  analogReadTime =millis();
  pressureReadTime = millis();
  syncTime = millis();
  clockUpdateTime = millis();
  dhtTime = millis();
  sendTime = millis()+sendInterval-500;
  sendTime = millis();
  displayTime = millis();
  rgbTime = millis();
  
  if(networkOnlyMode){
      sendTime= (-1)*sendInterval;
  }
  
}

boolean initTrasmitter(){
 boolean result = false;
 int attempts = 0;
 unsigned long t2 =millis();
 while(attempts <=4){ 
 mySerial.flush();
 mySerial.write((byte)9);
 mySerial.write(lowByte(sendInterval/200));
 mySerial.write(highByte(sendInterval/200));
 mySerial.write(lowByte(dhtInterval/2));
 mySerial.write(highByte(dhtInterval/2));
 mySerial.write(lowByte(windInterval/2));
 mySerial.write(highByte(windInterval/2));
 mySerial.write((byte)realTimeWind);
/// boolean response = checkResponse(true);
 unsigned long t = millis();
  while(millis() < t+ SERIAL_TIMEOUT/4){
    if(mySerial.available()){
      if(mySerial.read() == 1){
        result =true;
        break;
      }   
    }
    drawLoadingBar(map(millis() - t2,0,SERIAL_TIMEOUT,0,128),false);            
    delay(1);
  }
  if(result){
    break;
  }
  attempts++;
 }
  mySerial.flush();
  mySerial.write((byte)1);
  clearLoadingBar();
  return result;
}

void displayWiFiConnecting(){
  unsigned long ti = millis();
  while (WiFi.status() != WL_CONNECTED&&millis() < ti+WIFI_CONNECTION_TIMEOUT) 
  {
    delay(20);
    drawLoadingBar(map(millis() - ti,0,WIFI_CONNECTION_TIMEOUT,0,128),false);
  }
  clearLoadingBar();
}

void connectToWiFi(){
  if(handleWiFiConnect()) return; 
  int8_t numberOfNetworks = WiFi.scanNetworks();
  for(int8_t i =0; i<numberOfNetworks; i++){
      if(handleWiFiConnect(i,ssid, pass)) return;
      if(handleWiFiConnect(i,ssid2, pass2)) return;
      if(handleWiFiConnect(i,ssid3, pass3)) return;
  }
}
boolean handleWiFiConnect(int8_t index,const char *ssid,const char  *pass){
  if(!WiFi.SSID(index).equals(ssid)) return false;
  WiFi.begin(ssid, pass);
  displayWiFiConnecting();
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  return true;
}

//boolean handleWiFiConnect(const char *ssid,const char  *pass){
//  clearDisplay();
//  display.println(F("WiFi connectiong to:"));
//  display.println(ssid);
//  displayA();
//  WiFi.begin(ssid, pass);
//  displayWiFiConnecting();
//  if(WiFi.status() != WL_CONNECTED){
//    return false;
//  }
//  return true;
//}
//boolean handleWiFiConnect(const char *ssid,const char  *pass){
//  clearDisplay();
//  display.println(F("WiFi connectiong to:"));
//  display.println(ssid);
//  displayA();
//  WiFi.begin(ssid, pass);
//  displayWiFiConnecting();
//  if(WiFi.status() != WL_CONNECTED){
//    return false;
//  }
//  return true;
//}
boolean handleWiFiConnect(){
  WiFi.begin();
  displayWiFiConnecting();
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  return true;
}
void caclulateInsideMeasurements(){
     if(samples == 0){
      samples = 1;
     }
     if(pressureSamples ==0){
      pressureSamples =1;
     }
     temp = round((tempSum*100.0f)/samples)/100.0f;
     hum = round((humSum*100.0f)/samples)/100.0f;
     pres = round(presSum/pressureSamples)/100.0f;

     calcRainSum();
     
    tempSum = 0;
    humSum =0;
    presSum = 0;
    samples =0;
    pressureSamples = 0;
}
void update(unsigned long createdAtTime =0){
   if (getData()) {
      if(realTimeWind){
          caclulateInsideMeasurements();
      }
     sendData(createdAtTime,false);
     createdAtTime=0; 
     if(signalReceived){
      interpolatedAverangeWind = interpolatedAverangeWind*0.9f + averangeWind*0.1f;
      displayWeatherData(true);   
     }else if(!clockColorMode&&!displayWeatherData){
       clearDisplay();
       display.println("Home:    " + String(temp,1) +"C  " +String(hum,1)+"%");
       display.println(F("Outside: ---C   ---%"));
       display.println(String(pres,1) + "hPa --mm   --kmh");   
       displayA();
     }
     
    if(millis() > syncTime + SYNC_TIME_INTERVAL){
      getDateTime();
      syncTime = millis();
    }
    updateTime();
    turnOffLedAtSpecyficTime();
    calculateDailyMaxima();
    sendDailyMaximaTimeCheck();
    calculateAutoRange(_month);
    sendAutoRangeToServer(1,false);
   // maxCurrentWind= 0;
    colorChangeInterval = 980;
    colorChangeIntervalResolution = 35; 
    } else {
     display.fillRect(87,16,40,20,BLACK);   
     display.setCursor(97,16);
     display.println(String(currentWind)+"kmh");
  //   display.fillRect(0,60,map(millis() - sendTime,0,sendInterval,0,128),3,WHITE);   
     displayA(); 

     if(windColor >0){
      windColor-=windColor/19.0f;
     }
     if(currentWind > windColor){
      windColor = currentWind;
     }
              
      windSum+=currentWind;
      windCounter++;
      if(currentWind > maxCurrentWind){
        maxCurrentWind = currentWind;
      }   
      colorChangeInterval = 400;
      colorChangeIntervalResolution = 10;  
    }
    
    if(!clockColorMode&&!displayHeartBeatEnabled){
        lastR = r;
        lastB = b;
        lastG = g;
        setLedColor(colorMode);
        wp = 0;   
    }
}
void rfCommunication(){
    if (mySerial.available()) {
      signalReceived = true;
      if(!realTimeWind){
      caclulateInsideMeasurements();
      }
   //   if(signalReceived){
        update();
   //   }else{
   //      update(createdAtTime);
   //   }
      sendTime = millis(); 
  }

  if(millis() > sendTime +sendInterval + timeoutInterval){
    if(timeoutInterval==SENDING_TIMEOUT){
       createdAtTime =updateTime();
       caclulateInsideMeasurements();
    }else if(timeoutInterval >= sendInterval/2){
       update(createdAtTime);
       timeoutInterval=0;
    }
    mySerial.write((byte)1);
    timeoutInterval+=SENDING_TIMEOUT;
    signalReceived = false;
  }
}

void loop() 
{

  if(networkOnlyMode){
    if(millis() > sendTime +sendInterval){
      signalReceived = true;
      caclulateInsideMeasurements();
      update(); 
      sendTime = millis();
    }
  }else{
    rfCommunication();
  }

 
if(millis() > displayBrightnesTime + DISPLAY_BRIGHTNESS_INTERVAL){
 if(displayHeartBeatEnabled){  
     displayHeartBeat();
 }else if(clockColorMode){  
  displayClock(); 
 }else if(!realTimeWind&&millis() > displayTime +sendInterval/128){
//}else if(!realTimeWind&&millis() > displayTime +50){
    displayWeatherData(false);
    drawLoadingBar(map(millis() - sendTime,0,sendInterval,0,128),true);
    
 //   display.drawLine(0,58,map(millis() - sendTime,0,sendInterval,0,128),58,WHITE);
 //   display.fillCircle(map(millis() - sendTime,58,sendInterval,0,128),1,2,WHITE);
 
    //display.fillRect(0,60,map(millis() - sendTime,0,sendInterval,0,128),3,WHITE);

    displayTime = millis();
 }
   // displayWeatherData(true);
 if(analogReadInterval == 1){
   analogReadInterval =300;
    displayWeatherData(true);
 }
}
  knobController();
  // buttonControler();
  //dhtRead();  
  shtRead();
  sendSavedData();
  readPressure();
  detectPressureChange(currentPressure);
  updateFluidDimming();
  updateWeatherSegments();
  if(millis() > rgbTime + colorChangeInterval/colorChangeIntervalResolution&&wp <1.0f){   

    ledAnalogWrite();  
    
     wp+=0.1f;
    rgbTime =millis();
  }
  
#ifdef DEBUG
 readBluetoothCommands();
#endif

}
void readPressure(){
  if(millis() > pressureReadTime + PRESSURE_READ_INTERVAL ){
    currentPressure = bmp.readPressure();
    if(currentPressure > 94500.0f && currentPressure < 106500.0f&&!isnan(currentPressure)){
       presSum+=currentPressure;
       pressureSamples++;
    }
    pressureReadTime = millis();
    if(transmitCurrentPressure){
      Serial.println(currentPressure);
    }
  }
}

//void dhtRead(){
   //   customDHTRead();
//  if(clockColorMode||displayHeartBeatEnabled||realTimeWind||pressureChangeEnabled||startReading){
//    customDHTRead();
//  }else if(!startReading){
//    float t = dht.readTemperature();
//    float h = dht.readHumidity();
//    if(t >10&&t<50&&!isnan(t)&&!isnan(h)&&h<=100&&h>=1){  
//      tempSum+=t;
//      humSum+=h;
//      samples++;  
//    }
//  }
//}
  ////////////////////////////////////////DEBUG !!!!!!!!!!!/////////////////////////////////////
//float testDiff = 0;
//double testDiffSum =0;
//double testCtn =0;
//float maxTestDiff = 0;
//float minTestDiff = 1200.0f;
  ////////////////////////////////////////DEBUG !!!!!!!!!!!/////////////////////////////////////
  float shtMax = 0.0f;
  float shtMin = 50.0f;
  unsigned long shtTimeTest  =0;
  float lastSht =0.0f;
  int ctnQ =0;
  float ctnQ2 = 0;
  float defSum = 0.0f;
  //maxDif 0.08
  int diff1 = 0;
void shtRead(){
  if(millis() > sthTime+ SHT_READ_INTERVAL){
        float t = sht31.readTemperature();
        float h = sht31.readHumidity(); 
        if(t >13&&t < 50&&h>=0&&h<=100&&!isnan(t)&&!isnan(h)){  
            tempSum+=t;
            humSum+=h;
            samples++;  
     //                    diff1 =abs((t - lastSht)*10000.0f);
            shtColorRealTimeUpdate(t,h);
      //      float a = abs((t - lastSht)*10000.0f)/10.0f;

//            defSum+=a;
//            ctnQ2++;
//            if(ctnQ >2&&a >shtMax){
//              shtMax = a;
//            }
//              if(ctnQ >2&&a <shtMin){
//              shtMin= a;
//            }
            lastSht = t;
        }
     sthTime = millis();  
  }   
//  if(millis()  > shtTimeTest + 1500){
//    Serial.println(shtIIRFilterCoef);
//     Serial.println(diff1);
//     Serial.println(maxInsideTemp[0]);
//     Serial.println(minInsideTemp[0]);
//       Serial.println(maxInsideHum[0]);
//     Serial.println(minInsideHum[0]);
//   // Serial.println(shtMax);
// //     Serial.println(shtMin);
////    Serial.println(defSum/ctnQ2);
////   ctnQ2 =0;
////    defSum = 0;
// //   ctnQ++;
//    shtTimeTest =millis();
//  }
}

void shtColorRealTimeUpdate(float t, float h){

//  if(diff1 <=800){
//   
//  shtIIRFilterCoef = map(diff1,0,800,9999,7800)/10000.0f;
//}
 // if(t <maxInsideTemp[0]&& t > minInsideTemp[0]){

     shtCurrentTemp = lastShtCurrentTemp*shtIIRFilterCoef + t*(1.0f -shtIIRFilterCoef);
//  }
//  if( h < maxInsideHum[0]&& h > minInsideHum[0]){
       shtCurrentHum =lastShtCurrentHum*shtIIRFilterCoef + h*(1.0f -shtIIRFilterCoef);
  // }
   
    lastShtCurrentHum = shtCurrentHum;
    lastShtCurrentTemp =shtCurrentTemp; 
    if(!pressureChangeEnabled){
    setLedColor(colorMode);
    }
      yield();
}
//void customDHTRead(){
//  if(millis() > dhtTime+ 3000&&!startReading){  
//      dht.resetData();
//      digitalWrite(DHTPIN, HIGH);
//      startReading = true;
//      dhtTime = millis();
//      pinTime = millis();
//  }
//    if(startReading){
//      if(millis() - pinTime > 250){
//        pinMode(DHTPIN, OUTPUT);
//        digitalWrite(DHTPIN, LOW);
//        delay(20);
//        if(dht.fastRead()){         
//        float t = dht.getTemperature();
//        float h = dht.getHumidity(); 
//        if(t >13&&t < 50&&h>=0&&h<=100&&!isnan(t)&&!isnan(h)){  
//          tempDiffSum+=tmpTempSth-t;
//          tempDiffSamples++;
//        }
//////          ////////////////////////////////////////DEBUG !!!!!!!!!!!/////////////////////////////////////
//////            testDiff = bmp.readTemperature() - t;    
//////            testDiffSum+=abs(testDiff);
//////            testCtn++;
//////            if(testDiff > maxTestDiff){
//////               maxTestDiff = testDiff;    
//////            }else if(testDiff < minTestDiff){
//////               minTestDiff = testDiff;
//////            }
//////          ////////////////////////////////////////DEBUG !!!!!!!!!!!/////////////////////////////////////
////            tempSum+=t;
////            humSum+=h;
////            samples++;  
////        }   
//        }
//        startReading = false;      
//      }
//    }
//}
void knobController(){
   if(millis() > analogReadTime + analogReadInterval&&enableKnob){
    analogValue = map(analogRead(A0),4,879,0,200);     
    if(analogValue > lastAnalogValue +1||analogValue < lastAnalogValue -1){   
 //if(analogValue != lastAnalogValue){  
      brightness = 1 -(analogValue/200.0f);
      lastBrightness = brightness;
      displayBrightness();
      ledAnalogWrite();
      lastAnalogValue = analogValue;
      analogReadInterval = 1;
      displayBrightnesTime =millis();
    }
    analogReadTime = millis();
  }

}

void clearDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
}

void displayClock(){
  unsigned long offset = (millis() - timeOffset)/1000;
  _second = second(epochTime+offset);
  if(lastSecond != _second){
    lastSecond = _second;
     _minute = minute(epochTime+offset);
     _hour = hour(epochTime+offset);
     display.setFont(&FreeSansBold12pt7b);
     clearDisplay();
     display.setCursor(1,22);
     if(_hour < 10){
      display.print(F("0"));
     }
     display.print(_hour);
     display.print(F(" : "));
     if(_minute < 10){
      display.print(F("0"));
     }
     display.print(_minute);
     display.print(F(" : "));
     if(_second < 10){
      display.print(F("0"));
     }
     display.print(_second);
     displayA();
     display.setFont();

     lastR = r;
     lastB = b;
     lastG = g;
     setLedColor(colorMode);
     wp = 0; 
  }
}
void ledAnalogWrite(){
   if(!lightLock){
    analogWrite(BLUE_PIN,1023 -((1-wp)*lastB + wp*b)*brightness);
    analogWrite(GREEN_PIN,1023 -((1-wp)*lastG + wp*g)*brightness);
    analogWrite(RED_PIN,1023 -((1-wp)*lastR + wp*r)*brightness);
   }
}
void calcRainSum(){
  rainSum +=rain*RAIN_COEF;
  if(_hour != lastHour){
    rainSum = 0.0f;
  }
  lastHour = _hour;
}
//void displayWeatherData(boolean flag){
//  if(!clockColorMode&&!displayHeartBeatEnabled){  
//    clearDisplay();  
//     display.println("Home:    " + String(shtCurrentTemp,1) +"C  " +String(shtCurrentHum,1)+"%");
//     display.println("Outside: " + String(tempO,1) +"C  "+String(humO)+"%");
//     display.println(String(pres,1) + "hPa "+String(rain)+"mm    "+String(maxCurrentWind)+"kmh");  
//       if(flag){
//     displayA(); 
//       }
//  }
//}
