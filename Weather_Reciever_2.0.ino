#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
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
#include <amt1001_ino.h>

#include <Arduino.h>
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define OLED_RESET 4
#define BUTTON_PIN 12
#define RED_PIN 13
#define GREEN_PIN 16
#define BLUE_PIN 15
#define KNOB_PIN A0
#define HC_CTRL_PIN 14
#define SOFT_SERIAL_RX 5
#define SOFT_SERIAL_TX 4

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
Adafruit_BMP280 bmp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"tempus1.gum.gov.pl");
WiFiClient client;


const float dht22TempMin = -40.0f; // dokumentacja -40C
const float dht22TempMax = 50.0f;// dokumentacja 80C
const float humMin = 2.0f; // dokumentacja sht31 i dht22 0%
const float humMax = 100.0f;// dokumentacja sht31 i dht22 100%

const float presMin = 60000.0f; //bmp085 300hpa
const float presMax = 110000.0f;//bmp085 1100hpa

const float sht31TempMin = -40.0f; // dokumentacja -40C
const float sht31TempMax = 50.0f;// dokumentacja 125C
const int analogMin = 0;
const int analogMax = 1023;
const unsigned long windMin= 0;

float shtCurrentHum =0;
float shtCurrentTemp =0;
float lastShtCurrentHum =0;
float lastShtCurrentTemp =0;   
float shtIIRFilterCoef = 0.91f;

boolean enableKnob = false; //////

boolean turnOffLed = false;

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

//long sendInterval = 180000;
long sendInterval = 120000;
//long sendInterval = 30000;
unsigned long dayTimeInterval = 60000;
int analogReadInterval = 300;

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

int r_freq;
int g_freq;
int b_freq;

int max_r_freq =0;
int max_g_freq =0;
int max_b_freq =0;

int min_r_freq = 1000000;
int min_g_freq= 1000000;
int min_b_freq= 1000000;


float shtTempO =0;
float shtHumO = 0;
float analogLightO =0;
float analogRainO =0;
float analogHumO =0;
float presO = 0;
float dhtHumO = 0;

float currentShtTempO =0;
float currentShtHumO = 0;
float currentPresO = 0;
float currentDhtHumO = 0;
int currentAnalogLightO = 0;
int currentAnalogHumO =0;
int currentAnalogRainO =0;

float tempOsum =0;
float humOsum = 0;
float dhtHumOsum =0;
long analogLightOsum =0;
long analogRainOsum =0;
long analogHumOsum =0;
float presOsum = 0.0f;

int shtOsamples =0;
int dhtOsamples = 0;
int bmpOsamples = 0;
int analogLightOsamples =0;
int analogRainOsamples =0;
int analogHumOsamples =0;

unsigned long rf_pocketsLostCounter = 0;
unsigned long rf_pocketsReceivedCounter = 1;

float averangeWind =0;
float currentWind = 0;
float maxCurrentWind =0;
int windCounter = 0;
float windSum = 0;
float windMax = 300;
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

int8_t _hour;
int8_t _minute;
int8_t _second;

boolean buttonState = false;
boolean startReading = false;
int8_t menuCounter = 0;
boolean lock = false;
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
float wp = 0;
int lastR = 0;
int lastB = 0;
int lastG =0;

int hsv_h_ext = 21;

int analogValue = 0;
int lastAnalogValue = 0;
boolean displayHeartBeatEnabled = false;
boolean transmitCurrentPressure = false;
boolean bmpEnabled = true;
boolean shtEnabled = true;
//Flash real id:   001640EF
//Flash real size: 4194304 bytes
//Flash ide  size: 4194304 bytes
//Flash ide speed: 40000000 Hz
//Flash ide mode:  QIO
boolean displayEnabled = true;
   
void setup() 
{
 initialise();
}
unsigned long rf_communicationTimer = 0;
unsigned long rf_communicationTimer2 = 0;
int maxRfTime = 0;
int minRFTime = 1000*3600;
unsigned long rqSamples =0;
unsigned long rqSum =0;
int minRFTime2 = 1000*3600;
unsigned long rqSum2 =0;

int rqInterval = 0;
void loop() 
{
//  handle_module_B();
//rqInterval =rf_communicationTimer2 -millis();
//
//rqSum2+=rqInterval;
//if(rqInterval < minRFTime2){
//  minRFTime2 = rqInterval;
//}

//rf_communicationTimer =micros();
rfCommunication();
//unsigned long rQ = micros() - rf_communicationTimer;
//rf_communicationTimer2 = millis();
//if(rQ < minRFTime){
//  minRFTime = rQ;
//}
//
//if(rQ > maxRfTime){
//  maxRfTime = rQ;
//}
//
//rqSamples++;
//rqSum+=rQ;

if(millis() > sendTime +sendInterval){
  if(networkOnlyMode){
      networkVersion();
  }
  caclulateAvgMeasurements();
  sendData(0,false);
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
    updateWeatherSegments(false);
    serialFlush();
  sendTime =millis();
}
 
if(millis() > displayBrightnesTime + DISPLAY_BRIGHTNESS_INTERVAL){
 if(displayHeartBeatEnabled){  
     displayHeartBeat();
 }else if(clockColorMode){  
  displayClock(); 
 }else if(millis() > displayTime +sendInterval/128){
    displayWeatherData(false);
    drawLoadingBar(map(millis() - sendTime,0,sendInterval,0,128),true);
    displayTime = millis();
 }
}
  //knobController();
  // buttonControler();
  //dhtRead();  
  shtRead();
  sendSavedData();
  readPressure();
  detectPressureChange(currentPressure);
 // updateFluidDimming();

#ifdef DEBUG
 readBluetoothCommands();
#endif

}
void readPressure(){
  if(millis() > pressureReadTime + PRESSURE_READ_INTERVAL&&bmpEnabled){
    currentPressure = bmp.readPressure();
    if(currentPressure > 94500.0f && currentPressure < 106500.0f&&!isnan(currentPressure)){
       presSum+=currentPressure;
       pressureSamples++;
    }
    pressureReadTime = millis();
    if(transmitCurrentPressure){
      mySerial.println(currentPressure);
    }
  }
}

void shtRead(){
  if(millis() > sthTime+ SHT_READ_INTERVAL&&shtEnabled){
        float t = sht31.readTemperature();
        float h = sht31.readHumidity(); 
        if(t >13&&t < 50&&h>=0&&h<=100&&!isnan(t)&&!isnan(h)){  
            tempSum+=t;
            humSum+=h;
            samples++;  
            shtColorRealTimeUpdate(t,h);
        }
     sthTime = millis();  
  }   
}

void shtColorRealTimeUpdate(float t, float h){
   shtCurrentTemp = lastShtCurrentTemp*shtIIRFilterCoef + t*(1.0f -shtIIRFilterCoef);
   shtCurrentHum =lastShtCurrentHum*shtIIRFilterCoef + h*(1.0f -shtIIRFilterCoef);
    lastShtCurrentHum = shtCurrentHum;
    lastShtCurrentTemp =shtCurrentTemp; 
    if(!pressureChangeEnabled){
    setLedColor(colorMode);
    ledAnalogWrite();
    }
}

void knobController(){
   if(millis() > analogReadTime + analogReadInterval&&enableKnob){
    analogValue = map(analogRead(KNOB_PIN),4,879,0,200);     
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
void calcRainSum(){
  rainSum +=rain*RAIN_COEF;
  if(_hour != lastHour){
    rainSum = 0.0f;
  }
  lastHour = _hour;
}
