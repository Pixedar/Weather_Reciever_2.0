const char *ssid =  "WLAN-7F8H56"; 
const char *pass =  "ifDii1BFF3Dfh0d";
const char *ssid2 =  "FunBox-72D6"; 
const char *pass2 =  "FF6EAD542DCE77FD357F2F6AC9";
const char *ssid3 =  "OnePlus 7Pro"; 
const char *pass3 =  "W112Qzcx98";
const char *ssid4 ="AP-Media_6860";
const char *pass4 ="cekincekin";

IPAddress ip(192,168,8,100);            // IP address of the server
IPAddress gateway(192,168,8,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network


void initialise(){
  pinMode(HC_CTRL_PIN,INPUT);
  digitalWrite(HC_CTRL_PIN,LOW);
//          pinMode(HC_CTRL_PIN,OUTPUT);
//       digitalWrite(HC_CTRL_PIN,LOW);
  Serial.begin(9600);
//Serial.begin(230400);
  mySerial.begin(57600);

//  while(true){
//    while(Serial.available()){
//      mySerial.write(Serial.read());
//    }
//    delay(1);
//  }
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

  if(!bmp.begin()){
    display.println(F("BMP error"));
    displayA();
    delay(1000);
    bmpEnabled = false;
  }else{
      setBmpSampling();
  }
  if (!sht31.begin(0x44)) {   
    display.println(F("SHT31 error"));
    displayA();
    delay(1000);
    shtEnabled = false;
  }
  loadSettings();

  display.print(F("WiFi connecting"));
 
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  connectToWiFi(); 
 // Serial.println("IP");
//  Serial.println(WiFi.subnetMask());
  if(WiFi.status() == WL_CONNECTED){
    initOTA();
    display.print(F(" OK"));
    display.println();
    display.print(F("Time Synh"));
    displayA();
    timeClient.begin();
    timeClient.setTimeOffset(14400);   
    getDateTime();
    int tmp = fluidDimming;
    fluidDimming = 0;
    turnOffLedAtSpecyficTime();
    fluidDimming = tmp;
    display.println(F(" OK"));
    display.println(getTimestamp(timeClient.getEpochTime()));
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
 //   initServer();
  }else{
    display.print(F(" ERR"));
    display.println(F("Offline Mode"));
    displayA();
  } 
        
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
  maxPressureChange[0] = 0;
  minPressureChange[0] = 10000;
  serialFlush();
}

void connectToWiFi(){
  //WiFi.config(ip,gateway,subnet);  
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
void setBmpSampling(){
 bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,    
 Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
 Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
 Adafruit_BMP280::FILTER_X16,      /* Filtering. */
 Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */
}
