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

#define COLOR_ID 50
#define WIND_ID 51
#define PHOTORESISTOR_LIGHT_ID 52
#define UV_LIGHT_ID 53
#define BH_LIGHT_ID 54
#define ANALOG_RAIN_ID 55
#define WIND_DIR_ID 56
#define EMF_ID 57

byte rf_id;
byte j;
byte tmpBuf[4];
float tmpValA =0;
int tmpValB =0;
unsigned long tmpValC = 0;
float rangeCheckA;
int rangeCheckB;
unsigned long rangeCheckC;
unsigned long timeOutCtn;
boolean successFlag = false;
byte lastRainId = 255;
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
      case WIND_ID: readWind(); break;
      case COLOR_ID: readColor(); break;
      case SHT_ID: readSht(); break;
    //  case ANALOG_HUM_ID: readAnalogHum(); break;
    //  case ANALOG_RAIN_ID: readAnalogRain(); break;
   //   case ANAOG_LIGHT_ID: readAnalogLight(); break;
      case BMP_ID: readBmp(); break;
      case DHT_ID: readDht(); break;
      case RAIN_ID: readRain(); break;
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
void readRain(){
  byte k = Serial.read();
  if(isnan(k))return;
  if(k!= lastRainId){
    rain++;
  }
  lastRainId = k;
}
unsigned long k1 = 0;
unsigned long k2 = 0;
unsigned long k3 = 0;
float lastK = 0;
double sumK = 0;
double kCtn = 0;

unsigned long n1 = 0;
unsigned long n2 = 0;
unsigned long n3 = 0;
float lastN = 0;
double sumN = 0;
double nCtn = 0;


void readWind(){
    k1++;
    boolean flagC = false;
    float w = readFloat(flag);
    if(flagC){
      k2++;
    }
    if(isnan(w)){
      k3++;
    }
    if(!flagC&&!isnan(w)){
    sumK+=w-lastK;
    lastK=w;
    kCtn++;
    }
    
//  if(!read(rangeCheckC)) return;
//  if(isnan(rangeCheckC)) return;
//  float w = calcWind(rangeCheckC);
//  if(isnan(w)||w < windMin||w>windMax)return;
//  currentWind = w;
}


void readColor(){
  for(int j =0; j < 4; j++){
      n1++;
    boolean flagC = false;
    float w = readUint16(flag);
    if(flagC){
      n2++;
    }
    if(isnan(w)){
      n3++;
    }
    if(w-lastN < 40000&&!flagC&&!isnan(w)){
      sumK+=w-lastN;
      lastN=w;
      nCtn++;
    }
  }
}

//void readColor(){
//  if(!read(rangeCheckB)) return;
//  if(isnan(rangeCheckB)) return;
//  r_freq = rangeCheckB;
//  if(r_freq > max_r_freq){
//    max_r_freq = r_freq;
//  }
//  if(r_freq < min_r_freq){
//    min_r_freq = r_freq;
//  }
//  if(!read(rangeCheckB)) return;
//  if(isnan(rangeCheckB)) return;
//  g_freq = rangeCheckB;
//  if(g_freq > max_g_freq){
//    max_g_freq = g_freq;
//  }
//  if(g_freq < min_g_freq){
//    min_g_freq = g_freq;
//  }
//  if(!read(rangeCheckB)) return;
//  if(isnan(rangeCheckB)) return;
//  b_freq = rangeCheckB;
//    if(b_freq > max_b_freq){
//    max_b_freq = b_freq;
//  }
//  if(b_freq < min_b_freq){
//    min_b_freq = b_freq;
//  }
//}
//wiatromierz 9 cm od srodka do konca 
// ok 7 cm od srodka do środka pułkuli 
//jeden obrót to dwa sygnały
float calcWind(unsigned long val){
  if(val ==0) return -1;
  const double pi = 3.14159265359d;
  const double radius = 0.00009d; // w km, od środka do konca
  return (float)((0.5*2*pi*radius)*((3600*1000000.0d)/((double)val)));
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
boolean read(unsigned long &val){
  boolean flag = false;
  for(j =0; j < DATA_ITERATION/2; j++){
   tmpValC = readLong(flag);
   if(flag) break;
   if(tmpValC != readLong(flag)) flag = true;
  }
  if(flag) return false;
  val = tmpValC;
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
        if(rf_id == COLOR_ID||rf_id == WIND_ID){
          return;
        }
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
float readLong(boolean &flag){
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[0] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[1] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[2] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[3] = Serial.read();
      unsigned long result = *((unsigned long*)(tmpBuf));
      return result;
}
uint16_t readUint16(boolean &flag){
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[0] = Serial.read();
      if(!checkSerial()){ flag = true; return 0;}
      tmpBuf[1] = Serial.read();
      uint16_t result = *((uint16_t*)(tmpBuf));
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
