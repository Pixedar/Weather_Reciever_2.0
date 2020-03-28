const char *ssid =  "WLAN-7F8H56"; 
const char *pass =  "ifDii1BFF3Dfh0d";
const char *ssid2 =  "FunBox-72D6"; 
const char *pass2 =  "FF6EAD542DCE77FD357F2F6AC9";
const char *ssid3 =  "OnePlus 7Pro"; 
const char *pass3 =  "W112Qzcx98";
const char *ssid4 ="AP-Media_6860";
const char *pass4 ="cekincekin";
void initialise(){
  pinMode(HC_CTRL_PIN,INPUT);
  digitalWrite(HC_CTRL_PIN,LOW);
  
  Serial.begin(9600);
  mySerial.begin(57600);

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

  display.print(F("WiFi connecting"));
 
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  connectToWiFi(); 

  if(WiFi.status() == WL_CONNECTED){
    display.print(F(" OK"));
    display.println();
    display.print(F("Time Synh"));
    displayA();
    timeClient.begin();
    timeClient.setTimeOffset(7200);   
    getDateTime();
    initOTA();
    display.print(F(" "));
    printDate();
    display.println();
    displayA();
    
    display.print(F("Sending status"));
    displayA();
    sendError("RECEIVER RESET");
    display.print(F(" OK"));
    display.println();
    displayA();
    
    display.println(F("Loading Auto Range"));
    displayA();
    
    initAutoRange();
  
  }else{
    display.print(F(" ERR"));
     display.println(F("Offline Mode"));
    displayA();
  }

  turnOffLedAtSpecyficTime(); 
    
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
  sendTime = millis()+sendInterval;
  sendTime = millis();
  displayTime = millis();
  rgbTime = millis();
  
  serialFlush();
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
boolean handleWiFiConnect(){
  WiFi.begin();
  displayWiFiConnecting();
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  return true;
}
void printDate(){
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
}
