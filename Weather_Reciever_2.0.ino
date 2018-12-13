#include <DHT.h> 
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Wire.h"
#include "Time.h"
#include <TimeLib.h>
#include <EEPROM.h>
#include <Fonts/FreeSansBold12pt7b.h>

#define OLED_RESET 4
#define BUTTON_PIN 12
#define RED_PIN 13
#define GREEN_PIN 16
#define BLUE_PIN 15
#define DHTPIN 14

#define DEBUG false
#define SERIAL_TIMEOUT 3000
#define SENDING_TIMEOUT 1100
#define SYNC_TIME_INTERVAL 86400000
#define DISPLAY_BRIGHTNESS_INTERVAL 800


Adafruit_SSD1306 display(OLED_RESET);
SoftwareSerial mySerial(5, 4);
SoftwareSerial btSerial(10, 9);
Adafruit_BMP280 bmp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"tempus1.gum.gov.pl");
WiFiClient client;
DHT dht(DHTPIN, DHT22,11);


String apiKey = "12RUI1FA6G5C5CWM"; 
String dailyMaximaApiKey = "VJFQUX9HYOWZAQT4";     
const char *ssid =  "WLAN-7F8H56"; 
const char *pass =  "ifDii1BFF3Dfh0d";
const char* server = "api.thingspeak.com";
//const char *ssid =  "AndroidAP";
//const char *pass =  "jyok8169";
boolean enableKnob = false; //////
boolean realTimeWind = false;
boolean turnOffLed = false;
boolean recieveDataFormInternet = false;
boolean autoTempRangeMode = true;

long sendInterval = 180000;
int dhtInterval = 3000;
int homeDhtInterval = 3000;
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
int8_t stopDimHour = 6;
int8_t stopDimMinute = 7;

boolean clockColorMode = false;
int8_t colorMode = 1;
int8_t colorModesRGB[2][3] = {{1,4,3},{2,3,1}};
int8_t colorModesHSV[2] = {0,6};
int monthMax[] = {73,97,157,233,270,300,337,340,293,203,133,87};
int monthMin[] = {-216,-213,-179,-60,6,68,94,90,38,-44,-181,-225}; 

int8_t maxAverangeWind= 8;
int8_t maxWind = 20; 
int8_t maxInsideTemp = 28;
int8_t minInsideTemp = 19;
int8_t maxInsideHum = 80;
int8_t minInsideHum = 34;
int8_t maxOutsideHum = 100;
int8_t minOutsideHum = 0;
int8_t maxRain = 8;
int maxPressure = 990;
int minPressure = 969;

int r;
int g;
int b;

float tempO =0;
byte humO = 0;

float averangeWind =0;
byte maxCurrentWind = 0;
byte currentWind = 0;
int windCounter = 0;
float windSum = 0;
float lastWindA;

float temp = 0;
float hum = 0;
float pres = 0;
int samples = 0;

byte rain =0;

float _max;
float _min;
uint8_t _day;
int8_t _month;

int8_t _hour;
int8_t _minute;
int8_t _second;

int8_t lastSecond = 0;

boolean buttonState = false;
boolean startReading = false;
int8_t menuCounter = 0;
boolean lock = false;
boolean dataNotReceived = false;
float presSum =0;
float tempSum =0;
float humSum =0;

float dailyPressureSum;;
float dailyInsideTemperatureSum;
float dailyInsideHumiditySum;
float dailyOutsideHumiditySum;
float dailyOutsideTemperatureSum;
float dailyWindSum;
float dailyAverangeWindSum;
int   dailyRain;

float dailyPressureMaxima[] = {2000,0};
float dailyInsideTemperatureMaxima[] = {100,0};
float dailyInsideHumidityMaxima[] = {100,0};
float dailyOutsideHumidityMaxima[] = {100,0};
float dailyOutsideTemperatureMaxima[] = {100,0};
float dailyWindMaxima[] = {300,0};
float dailyAverangeWindMaxima[] = {300,0};
float dailySamples = 0;

unsigned long dhtTime;
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
float wp = 0;
int lastR = 0;
int lastB = 0;
int lastG =0;
float windColor = 0;
boolean backupSendLock = false;
boolean dailyMaximaSent = false; 
int analogValue = 0;
int lastAnalogValue = 0;
boolean displayHeartBeatEnabled = false;
void setup() 
{
  Serial.begin(115200);
  mySerial.begin(38400);

  pinMode(RED_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  analogWrite(RED_PIN,1023); 
  analogWrite(GREEN_PIN,1023); 
  analogWrite(BLUE_PIN,1023); 
  
  dht.begin();
  Wire.begin(2,0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.display(); 
  clearDisplay();
  display.display();     
  display.setTextSize(1);
  display.setTextColor(WHITE);
   
  while(!bmp.begin()){
    clearDisplay();
    display.println(F("Pressure sensor error"));
    display.display();
    delay(1);
  }

 loadSettings();

 display.println(F("Trasmitter connecting"));
 display.println(F("WiFi connecting"));
 display.display();
 mySerial.flush();
 mySerial.write((byte)9);
 mySerial.write(lowByte(sendInterval/200));
 mySerial.write(highByte(sendInterval/200));
 mySerial.write(lowByte(dhtInterval/2));
 mySerial.write(highByte(dhtInterval/2));
 mySerial.write(lowByte(windInterval/2));
 mySerial.write(highByte(windInterval/2));
 mySerial.write((byte)realTimeWind);
 boolean response = checkResponse(true);
  clearDisplay();
  if(response){
    display.println(F("Trasmitter connected"));
  }else{
    display.println(F("Trasmitter error"));
    delay(2500);
  }
  display.println(F("WiFi connecting"));
  display.display();
  mySerial.flush();
  mySerial.write((byte)1);

 // display.println(F("WiFi connecting"));
 // display.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
    display.print(F("."));
    display.display();
  }
  clearDisplay();
  if(response){
    display.println(F("Trasmitter connected"));
  }else{
    display.println(F("Trasmitter error"));
  }
  display.println(F("WiFi connected"));
  display.display();
  delay(700);

  timeClient.begin();
  timeClient.setTimeOffset(7200);
  
  timeClient.update();
  delay(300);
  if(autoTempRangeMode){
      initialiseAutoTempRange();
  }else{
      setMaxMin();
  }
  epochTime = timeClient.getEpochTime();
  timeOffset = millis();

  clearDisplay();
 // display.println(F("       Today is "));
  display.print(year(timeClient.getEpochTime()));
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
  display.print(hour(epochTime));
  display.print(F(":"));
  display.print(minute(epochTime));
  display.print(F(":"));
  display.print(second(epochTime));
  display.println();
  display.print(F("Interval: "));
  display.println(String(sendInterval/1000)+" s");
  display.print(F("Max temp: "));
  if(autoTempRangeMode){
      display.print(_max,1);
  }else{
      display.print(_max/10.0f,1);
  }
  display.println(F(" C"));
  display.print(F("Min temp: "));
  if(autoTempRangeMode){
      display.print(_min,1);
  }else{
      display.print(_min/10.0f,1);
  }
  display.print(F(" C"));
  display.display();
  delay(2700);
  
  turnOffLedAtSpecyficTime(); 
  
  analogValue = map(analogRead(A0),3,879,0,200); 
  lastAnalogValue = analogValue;
  brightness = 1 -(analogValue/200.0f);
  
  tempSum= dht.readTemperature();
  humSum= dht.readHumidity();
  presSum = bmp.readPressure()/100.0f;
  samples++;
    
  displayBrightnesTime =millis();
  analogReadTime =millis();
  syncTime = millis();
  clockUpdateTime = millis();
  dhtTime = millis();
  sendTime = millis()+sendInterval-500;
  displayTime = millis();
  rgbTime = millis();
}

void loop() 
{
  if (mySerial.available()||dataNotReceived) {
    if (getData()||dataNotReceived) {
      if(clockColorMode){
        display.setCursor(120,25);
        display.print("s");
        display.display();
      }
     if(samples == 0){
      samples = 1;
     }
     
     temp = tempSum/samples;
     hum = humSum/samples;
     pres = presSum/samples;
     sendData();
       
     if(!dataNotReceived){
      displayWeatherData();
     }else if(!clockColorMode&&!displayWeatherData){
      clearDisplay();
       display.println("Home:    " + String(temp,1) +"C  " +String(hum,1)+"%");
       display.println(F("Outside: ---C   ---%"));
       display.println(String(pres,1) + "hPa --mm   --kmh");   
       display.display();
     }
    
    
    if(millis() > syncTime + SYNC_TIME_INTERVAL){
      timeClient.update();
      epochTime = timeClient.getEpochTime();
      timeOffset = millis();
      delay(300);
      if(!autoTempRangeMode){
        setMaxMin();
      }else{
        ntpTest();  
      }
      
      syncTime = millis();
    }
    updateTime();
    turnOffLedAtSpecyficTime();
    calculateDailyMaxima();
    sendDailyMaximaTimeCheck();
    calculateTempRange(tempO, _hour,0.46f);
    tempSum = 0;
    humSum =0;
    presSum = 0;
    samples =0;
    maxCurrentWind= 0;
    colorChangeInterval = 980;
    colorChangeIntervalResolution = 35;
    backupSendLock = false;
    dataNotReceived = false; 
    sendTime = millis();  
    } else {
     display.fillRect(87,16,40,20,BLACK);   
     display.setCursor(97,16);
     display.println(String(currentWind)+"kmh");
     display.fillRect(0,30,map(millis() - sendTime,0,sendInterval,0,128),3,WHITE);   
     display.display(); 

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
  
  }else if(!backupSendLock&&millis() > sendTime +sendInterval + SENDING_TIMEOUT){
   mySerial.write((byte)1);
    //mySerial.read();
    backupSendLock = true;  
  }else if(backupSendLock&&millis() > sendTime +sendInterval + SENDING_TIMEOUT*2){
    dataNotReceived = true;
  }


if(millis() > displayBrightnesTime + DISPLAY_BRIGHTNESS_INTERVAL){
 if(displayHeartBeatEnabled){  
     displayHeartBeat();
 }else if(clockColorMode){  
  displayClock(); 
 }else if(!realTimeWind&&millis() > displayTime +sendInterval/128){
    display.fillRect(0,30,map(millis() - sendTime,0,sendInterval,0,128),3,WHITE);
    display.display();
    displayTime = millis();
 }
 if(analogReadInterval == 1){
   analogReadInterval =300;
    displayWeatherData();
 }
}


  knobController();
  // buttonControler();
  homeSensorsRead();  
  
  if(millis() > rgbTime + colorChangeInterval/colorChangeIntervalResolution&&wp <=1){   
    ledAnalogWrite();  
     wp+=0.1f;
    rgbTime =millis();
  }
#ifdef DEBUG
 readBluetoothCommands();
#endif

}
void homeSensorsRead(){
    if(millis() > dhtTime+ homeDhtInterval&&!startReading){  
      dht.resetData();
      digitalWrite(DHTPIN, HIGH);
      startReading = true;
      dhtTime = millis();
      pinTime = millis();
  }
    if(startReading){
      if(millis() - pinTime > 250){
        pinMode(DHTPIN, OUTPUT);
        digitalWrite(DHTPIN, LOW);
        delay(20);
        if(dht.fastRead()){
        float p =bmp.readPressure()/100.0f;
        if(p>0){              
        float t = dht.getTemperature();
        float h = dht.getHumidity(); 
        if(t >13){               
            presSum+= p;
            tempSum+=t;
            humSum+=h;
            samples++;  
        }   
          }
        }
        startReading = false;      
      }
    }
}
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
void updateTime(){
    unsigned long offset = (millis() - timeOffset)/1000;
   _minute = minute(epochTime+offset);
   _hour = hour(epochTime+offset);
   _second = second(epochTime+offset);
}
void turnOffLedAtSpecyficTime(){
   if(_hour >= startDimHour){
    if(_minute >=startDimMinute&&!turnOffLed){
      display.testDim(0);
      lastBrightness = brightness;
      brightness =0;
      turnOffLed = true;
    }     
   }else if(_hour >= stopDimHour){
      if(_minute >=stopDimMinute&&turnOffLed){
         display.testDim(255);
          brightness = lastBrightness;
          turnOffLed = false;
     }   
   }
}

void sendDailyMaximaTimeCheck(){
  if(_hour == 0){
    if(!dailyMaximaSent){
      sendDailyMaxima();
      dailyMaximaSent = true;
    }
  }else{
    dailyMaximaSent = false;
  }
}
void calculateDailyMaxima(){
 dailySamples++;  
 dailyPressureSum+=pres;
 dailyInsideTemperatureSum+=temp;
 dailyInsideHumiditySum+=hum;
 dailyOutsideHumiditySum+=humO;
 dailyOutsideTemperatureSum+=tempO;
 dailyWindSum+=maxCurrentWind;
 dailyAverangeWindSum+=averangeWind;
 dailyRain+=rain;
 if(pres < dailyPressureMaxima[0]){
  dailyPressureMaxima[0] = pres;
 }else if(pres > dailyPressureMaxima[1]){
  dailyPressureMaxima[1] = pres;
 }
 if(temp < dailyInsideTemperatureMaxima[0]){
  dailyInsideTemperatureMaxima[0] = temp;
 }else if(temp > dailyInsideTemperatureMaxima[1]){
   dailyInsideTemperatureMaxima[1] = temp;
 }
 if(hum < dailyInsideHumidityMaxima[0]){
  dailyInsideHumidityMaxima[0] = hum;
 }else if(hum > dailyInsideHumidityMaxima[1]){
  dailyInsideHumidityMaxima[1] = hum;
 }
 if(humO < dailyOutsideHumidityMaxima[0]){
  dailyOutsideHumidityMaxima[0] = humO;
 }else if(humO > dailyOutsideHumidityMaxima[1]){
  dailyOutsideHumidityMaxima[1] = humO;
 }
 if(tempO < dailyOutsideTemperatureMaxima[0]){
  dailyOutsideTemperatureMaxima[0] = tempO;
 }else if(tempO > dailyOutsideTemperatureMaxima[1]){
   dailyOutsideTemperatureMaxima[1] = tempO;
 }
 if(maxCurrentWind < dailyWindMaxima[0]){
  dailyWindMaxima[0] = maxCurrentWind;
 }else if(maxCurrentWind > dailyWindMaxima[1]){
   dailyWindMaxima[1] = maxCurrentWind;
 }
 if(averangeWind < dailyAverangeWindMaxima[0]){
   dailyAverangeWindMaxima[0] = averangeWind;
 }else if(averangeWind > dailyAverangeWindMaxima[1]){
   dailyAverangeWindMaxima[1] = averangeWind;
 }
}

 void ntpTest(){
  if(year(timeClient.getEpochTime()) < 2018){
   timeClient.update();
   delay(1500);
   }else{
    return;
   }
   if(year(timeClient.getEpochTime()) < 2018){
     NTPClient timeClient(ntpUDP,"tempus2.gum.gov.pl");
     timeClient.update();
     delay(700);
   }
   if(year(timeClient.getEpochTime()) < 2018){
     NTPClient timeClient(ntpUDP,"ntp1.tp.pl");
     timeClient.update();
     delay(700);
   }
   if(year(timeClient.getEpochTime()) < 2018){
     NTPClient timeClient(ntpUDP,"ntp1.tp.pl");
     timeClient.update();
     delay(700);
   }
   if(year(timeClient.getEpochTime()) < 2018){
      clearDisplay();
      display.println("NTP error");
      display.display();
      delay(3500);
    }
}

void clearDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
}
void setMaxMin(){
  ntpTest();  
  _month = month(timeClient.getEpochTime());
  _day = day(timeClient.getEpochTime());

  _max = (monthMax[_month-1]*map(_day,1,_month%2?31:30,0,100)+monthMax[_month==1?12:_month-2]*map(_day,1,_month%2?31:30,100,0))/100;
  _min = (monthMin[_month-1]*map(_day,1,_month%2?31:30,100,0)+monthMin[_month==12?0:_month]*map(_day,1,_month%2?31:30,0,100))/100; 
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
     display.display();
     display.setFont();

     lastR = r;
     lastB = b;
     lastG = g;
     setLedColor(colorMode);
     wp = 0; 
  }
}
void ledAnalogWrite(){
    analogWrite(BLUE_PIN,1023 -((1-wp)*lastB + wp*b)*brightness);
    analogWrite(GREEN_PIN,1023 -((1-wp)*lastG + wp*g)*brightness);
    analogWrite(RED_PIN,1023 -((1-wp)*lastR + wp*r)*brightness);
}
void displayWeatherData(){
  if(!clockColorMode&&!displayHeartBeatEnabled){
    clearDisplay();
     display.println("Home:    " + String(temp,1) +"C  " +String(hum,1)+"%");
     display.println("Outside: " + String(tempO,1) +"C  "+String(humO)+"%");
     display.println(String(pres,1) + "hPa "+String(rain)+"mm    "+String(maxCurrentWind)+"kmh");  
     display.display(); 
  }
}
