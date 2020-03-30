#define DATA_ITERATION 2
#define END_OF_TRASMISSION_CODE 154
#define STRING_TRASMISSION_CODE 202
#define ERROR_STRING_TRASMISSION_CODE 203
#define REAL_TIME_DATA_SIZE 15
#define TIMEOUT 5

#define SHT_ID 170
#define DHT_ID 171
#define ANAOG_LIGHT_ID 172
#define ANALOG_HUM_ID 173
#define ANALOG_RAIN_ID 174
#define RAIN_ID 175
#define BMP_ID 176

byte rf_id;
byte j;
byte tmpBuf[4];
float tmpValA =0;
int tmpValB =0;
float rangeCheckA;
int rangeCheckB;
unsigned long timeOutCtn;
boolean successFlag = false;

void rfCommunication(){
  while(Serial.available()){
    networkOnlyMode = false;
    successFlag = false;
    getID();
    if(rf_id == 0){
 //     Serial.flush();
         serialFlush();
         rf_pocketsLostCounter++;
      continue;
    }
    switch((int)rf_id){
      case SHT_ID: readSht(); break;
      case ANALOG_HUM_ID: readAnalogHum(); break;
      case ANALOG_RAIN_ID: readAnalogRain(); break;
      case ANAOG_LIGHT_ID: readAnalogLight(); break;
      case BMP_ID: readBmp(); break;
      case DHT_ID: readDht(); break;
      case RAIN_ID: rain++; break;
      case STRING_TRASMISSION_CODE: readText(false); break;
      case ERROR_STRING_TRASMISSION_CODE: readText(true); break;
      default: serialFlush();
    }
    if(!successFlag){
      rf_pocketsLostCounter++;
    }else{
      rf_pocketsReceivedCounter++;
    }
    yield();
  }
}

void readText(boolean sendAsError){
  if(!checkSerial()) return;
  String result = Serial.readStringUntil('\n');
  if(sendAsError){
    sendError(result);
  }
}

void readDht(){
  if(!read(rangeCheckA)) return;
  if(rangeCheckA >=humMin&& rangeCheckA <=humMax) return;
  currentDhtHumO = rangeCheckA;
  dhtHumOsum+=currentDhtHumO;
  dhtOsamples++;
  successFlag = true;
}

void readBmp(){
  if(!read(rangeCheckA)) return;
  if(isnan(rangeCheckA)&&rangeCheckA >=presMin&& rangeCheckA <=presMax) return;
  currentPresO = rangeCheckA/100.0f;
  presOsum+=currentPresO;
  bmpOsamples++;
  successFlag = true;
}

void readSht(){
   if(!read(rangeCheckA)) return;
   if(isnan(rangeCheckA)||rangeCheckA <sht31TempMin||rangeCheckA >sht31TempMax) return;
   currentShtTempO = rangeCheckA;
   if(!read(rangeCheckA)) return;
   if(isnan(rangeCheckA)||rangeCheckA <humMin||rangeCheckA >humMax) return;
   currentShtHumO = rangeCheckA;
   tempOsum+=currentShtTempO;
   humOsum+=currentShtHumO;
   shtOsamples++;
   successFlag = true;
}

void readAnalogHum(){
  if(!read(rangeCheckB)) return;
  if(isnan(rangeCheckB)||rangeCheckB <analogMin||rangeCheckB >analogMax) return;
  currentAnalogHumO = rangeCheckB;
  analogHumOsum+=currentAnalogHumO;
  analogHumOsamples++;
  successFlag = true;
}

void readAnalogRain(){
   if(!read(rangeCheckB)) return;
   if(isnan(rangeCheckB)||rangeCheckB <analogMin|| rangeCheckB >analogMax) return;
   currentAnalogRainO = rangeCheckB;
   analogRainOsum+=currentAnalogRainO;
   analogRainOsamples++;
   successFlag = true;
}
void readAnalogLight(){
   if(!read(rangeCheckB)) return;
   if(isnan(rangeCheckB)||rangeCheckB <analogMin||rangeCheckB >analogMax) return;
   currentAnalogLightO = rangeCheckB;
   analogLightOsum+=currentAnalogLightO;
   analogLightOsamples++;
   successFlag = true;
}
boolean read(float &val){
  boolean flag = false;
  for(j =0; j < DATA_ITERATION/2; j++){
   tmpValA = readFloat(flag);
   if(flag) break;
   if(tmpValA != readFloat(flag)) flag = true;
  }
  if(flag) return false;
  val = tmpValA;
  return true;
}
boolean read(int &val){
  boolean flag = false;
  for(j =0; j < DATA_ITERATION/2; j++){
   tmpValB = readInt(flag);
   if(flag) break;
   if(tmpValB != readInt(flag)) flag = true;
  }
  if(flag) return false;
  val = tmpValB;
  return true;
}
int readInt(boolean &flag){
   if(!checkSerial()){ flag = true; return 0;}
   tmpBuf[0] = Serial.read();
   if(!checkSerial()){ flag = true; return 0;}
   tmpBuf[1] = Serial.read();
  return tmpBuf[0]|tmpBuf[1]<<8;
}
void getID(){
  boolean flag = false;
    for(j =0; j < DATA_ITERATION/2; j++){
        if(!checkSerial()) flag = true;
        rf_id = Serial.read();
        if(!checkSerial()) flag = true;
        if(rf_id != Serial.read()) flag = true;
    }
    if(flag) rf_id = 0;
}
float readFloat(boolean &flag){
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[0] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[1] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[2] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[3] = Serial.read();
      float result = *((float*)(tmpBuf));
      return result;
}
boolean checkSerial(){
    if(!Serial.available()){
      timeOutCtn = millis();
      while(millis() < timeOutCtn + TIMEOUT){
        if(Serial.available()){
          return true;
        }
        yield();
      }
      return false;
    }
    return true;
}
void serialFlush(){
  while(Serial.available()){
    Serial.read();
    yield();
  }
}
void setHcSettings(){
  while(Serial.available()){
    byte b = Serial.read();
    if(b == 'w'){
        Serial.println("W_");
        pinMode(HC_CTRL_PIN,INPUT);
       digitalWrite(HC_CTRL_PIN,LOW);
       break;
    }else if(b == 'q'){
      Serial.println("Q_");
        pinMode(HC_CTRL_PIN,OUTPUT);
            digitalWrite(HC_CTRL_PIN,LOW);
            delay(40);
            Serial.flush();
          break;
    }
    mySerial.write(b);
    yield();
  }
  while(mySerial.available()){
      Serial.write(mySerial.read());
     yield();
  }
} 
boolean hcSetBoundRate(int bound){
  hcEnableCommandMode();
  Serial.print("AT+B" + String(bound));
  String result = Serial.readString();
  hcDisableCommandMode();
  if(result.equals("OK")) return true;
  return false;
}
void hcEnableCommandMode(){
  pinMode(HC_CTRL_PIN,OUTPUT);
  digitalWrite(HC_CTRL_PIN,LOW);
  delay(40);
  serialFlush();
}
void hcDisableCommandMode(){
  pinMode(HC_CTRL_PIN,OUTPUT);
  digitalWrite(HC_CTRL_PIN,LOW);
  delay(40);
  serialFlush();
}
//czas wykonania funckji interwal 30ms 115200bps rozmiar 15*2
//max 19273 ~ 19ms
//min 10 ~ 0.01ms
//typ 16.44 ~ 0.016ms
//co ile sie funckja wykonuje
//typ 11.40ms
//pakiety interwal 30ms 115200bps
//zgubione: 64045
//odebrane: 1595682
//% zgubionych 0.04%
//czas wykonania funckji interwal 10ms 96000bps rozmiar 15*2
//max 38889 ~ 39ms
//min 10 ~ 0.01ms
//typ 377.44 ~ 0.37 ms
//co ile sie funckja wykonuje
//max 46905ms
//typ 1997.62ms
//pakiety interwal 30ms 115200bps rozmiar 15*2
//zgubione: 5801
//odebrane: 182123
//% zgubionych 0.03%
