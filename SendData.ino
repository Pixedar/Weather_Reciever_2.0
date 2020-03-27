const char * apiKey = "12RUI1FA6G5C5CWM"; 
const char * debugApikey = "V9BDERAUY8VGWJU7";
const char * autoRangeApikey = "T44K8QM9YHVD73WG";
#define TALK_BACK_ID_A 28333
#define TALK_BACK_ID_B 37669
#define TALK_BACK_DEBUG_ID 37719
#define TALK_BACK_KEY_A "GKUDL9YXUNXZG425"
#define TALK_BACK_KEY_B "D60O4AD1A7RLMUI7"
#define TALK_BACK_DEBUG_KEY "QFJRR6QS7OBFFPAK"

#define NUM_OF_ATTEMPTS 4
#define MEM_SIZE 1000 // uwaga bylo 2000 ale ide pokazywalo ze za malo apmieci i moga byc probelmy ze stabuilnoscia mimo to esp dzialalo stabilnie

#define BAD_VALUE_ERROR_CODE -125
#define POCKET_LOST_RATIO_THRESHOLD 0.8

int8_t attempts = 0;
byte fields[MEM_SIZE][8];
unsigned long timestamps[MEM_SIZE];
int fieldsCounter = 0;
unsigned long savedDataSendTime;
boolean success = false;
boolean arrayCompressed = false;
int timestampsCounter = 0;
const unsigned long debugId = 556803;
String talkBackCommand = "";
int shtOsamplesCpy;


const long id = 333150;
const char * key = "12RUI1FA6G5C5CWM"; 

void loadValue(int8_t index,float &v,boolean &errorFlag){
  const float tmp =ThingSpeak.readFloatField(id,index, key);
  if(ThingSpeak.getLastReadStatus()!=200){
    errorFlag = true;
    return;
  } 
  v = tmp;
}
void loadValue(int8_t index,byte &v,boolean &errorFlag){
  const float tmp =ThingSpeak.readFloatField(id,index, key);
  if(ThingSpeak.getLastReadStatus()!=200){
    errorFlag = true;
    return;
  } 
  v = tmp;
}

boolean networkVersion(){
//  boolean errorFlag = false;
//  loadValue(1,tempO,errorFlag);
//  loadValue(3,humO,errorFlag);
//  loadValue(6,averangeWind,errorFlag);
//  loadValue(7,maxCurrentWind,errorFlag);
//  if(errorFlag){
//    displayConnectionError(0);
//  }   
//readTalkBackCommands();         
 // tempO = ThingSpeak.readFloatField(id,1, key);
 // humO = ThingSpeak.readFloatField(id,3, key);
 // averangeWind = ThingSpeak.readFloatField(id,6, key);
//  maxCurrentWind = ThingSpeak.readFloatField(id,7, key);
//  displayConnectionError(ThingSpeak.getLastReadStatus());
  
  if(!clockColorMode){
    displayWeatherData(true);
    display.setCursor(74,52);
    display.print(F("receiving"));
  }else{
    display.setCursor(120,52);
    display.print(F("r"));
  }
  displayA();  
String stat = "";
const char delC = 'Q';
 stat = ThingSpeak.readStatus(id, key);
 if(stat != ""){
  shtTempO = getValue(stat,delC,0).toFloat();
  shtHumO= getValue(stat,delC,1).toInt();
  averangeWind = getValue(stat,delC,2).toFloat();
  maxCurrentWind = getValue(stat,delC,3).toInt();
  rain = getValue(stat,delC,4).toInt();
 }
  displayWeatherData(true);
  return true;
}


void caclulateAvgMeasurements(){
  const float q = 100.0f;
     if(samples == 0){
      samples = 1;
     }
     if(pressureSamples ==0){
      pressureSamples =1;
     }
     temp = round((tempSum*q)/samples)/q;
     hum = round((humSum*q)/samples)/q;
     pres = round(presSum/pressureSamples)/q;

    calcRainSum();
    if(shtOsamples ==0){
      shtTempO = BAD_VALUE_ERROR_CODE;
      shtHumO = BAD_VALUE_ERROR_CODE;
    }else{
    shtTempO = round((tempOsum*q)/shtOsamples)/q;
    shtHumO = round((humOsum*q)/shtOsamples)/q;
    }
    if(dhtOsamples ==0){
      dhtHumO = BAD_VALUE_ERROR_CODE;
    }else{
      dhtHumO = round((dhtHumOsum*q)/dhtOsamples)/q;
    }
    if(dhtHumO!=BAD_VALUE_ERROR_CODE&&shtHumO!=BAD_VALUE_ERROR_CODE){
      shtHumO = (dhtHumO+shtHumO)/2.0f;
    }
    if(analogLightOsamples == 0){
      analogLightO = BAD_VALUE_ERROR_CODE;
    }else{
      analogLightO = round((analogLightOsum*q)/analogLightOsamples)/q;
    }
    if(analogRainOsamples ==0){
      analogRainO =BAD_VALUE_ERROR_CODE;
    }else{
      analogRainO = round((analogRainOsum*q)/analogRainOsamples)/q;
    }
    if(analogHumOsamples ==0){
      analogHumO = BAD_VALUE_ERROR_CODE;
    }else{
      analogHumO = getAnalogHum(round((analogHumOsum*q)/analogHumOsamples)/q);
    }
    if(bmpOsamples ==0){
      presO =  BAD_VALUE_ERROR_CODE;
    }else{
       presO = round((presOsum*q)/bmpOsamples)/q;
    }
    shtOsamplesCpy = shtOsamples;
    shtOsamples =0;
    dhtOsamples =0;
    analogLightOsamples =0;
    analogRainOsamples =0;
    analogHumOsamples =0;
    bmpOsamples =0;

    tempOsum =0;
    dhtHumOsum =0;
    analogLightOsum =0;
    analogRainOsum =0;
    analogHumOsum =0;
    presOsum = 0;
    humOsum =0;
    
    tempSum = 0;
    humSum =0;
    presSum = 0;
    samples =0;
    pressureSamples = 0;
}
int getAnalogHum(float stepV){
  double volt = (double)stepV * (5.0 / 1023.0);
  return (int)amt1001_gethumidity(volt);
}
String setFieldsForChannelB(){
  String error = "";
  if(analogLightO!=BAD_VALUE_ERROR_CODE&&analogLightO>=analogMin&& analogLightO <=analogMax){
    ThingSpeak.setField(1, String(analogLightO,2));
  }else{
    error+="analogLightO= "+String(analogLightO);
  }
  if(analogRainO!=BAD_VALUE_ERROR_CODE&&analogRainO>=analogMin&& analogRainO <=analogMax){
    ThingSpeak.setField(3, String(analogRainO,2));
  }else{
    error+="analogRainO= "+String(analogRainO);
  }
   if(analogHumO!=BAD_VALUE_ERROR_CODE&&analogHumO>=analogMin&& analogHumO <=analogMax){
    ThingSpeak.setField(6, String(analogHumO,2));
  }else{
    error+="analogHumO= "+String(analogHumO);
  }
  if(presO!=BAD_VALUE_ERROR_CODE&&presO>=presMin/100.0f&& presO <=presMax/100.0f){
    ThingSpeak.setField(7, String(presO,2));
  }else{
    error+="presO= "+String(presO);
  }
  ThingSpeak.setField(8, String(shtOsamplesCpy));
  return error;
}
String setFieldsForChannelA(){
  String error = "";
    if(shtTempO!=BAD_VALUE_ERROR_CODE&&shtTempO >=sht31TempMin&& shtTempO <=sht31TempMax&&!networkOnlyMode){
      ThingSpeak.setField(1, String(shtTempO,2));
    }else{
      error+="shtTempO= "+String(shtTempO);
    }
    if(shtHumO!=BAD_VALUE_ERROR_CODE&&shtHumO >=humMin&& shtHumO <=humMax&&!networkOnlyMode){
      ThingSpeak.setField(3, String(shtHumO,2));
    }else{
      error+="shtHumO= "+String(shtHumO);
    }
    if(rain >=0&& rain <=200&&!networkOnlyMode){  ///// todo zalezne od sending interval
      ThingSpeak.setField(8,rain); 
    }else{
      error+="rain= "+String(rain);
    }
    rain =0;
//      if(averangeWind <0|| averangeWind >49||((abs(lastAvgWind - averangeWind) > 10)&&!firstTime)){
//        error+="averangeWind= "+String(averangeWind);
//      }else if(!networkOnlyMode){ 
//        ThingSpeak.setField(6,String(averangeWind,2));
//        if(!saved){
//          lastAvgWind =  averangeWind;
//        }
//      }
      
//      if (maxCurrentWind <0||maxCurrentWind >70||(abs(lastWind - maxCurrentWind)>40)&&!firstTime) {
//        error+="maxCurrentWind= "+String(maxCurrentWind);
//      } else if(!networkOnlyMode){
//        ThingSpeak.setField(7,maxCurrentWind);
//         if(!saved){
//          lastWind = maxCurrentWind;
//         }
//      }
         
    if (temp != 0.0f) {
  //    if(networkOnlyMode){
   //     talkBackCommand+=getDelimiter()+String(temp,2);
  //    }else{
        ThingSpeak.setField(2,String(temp,2));       
   //   }
    } else {
      error+="temp=0 ";
    }
    if (hum != 0.0f) {
  //        if(networkOnlyMode){
  //      talkBackCommand+=getDelimiter()+String(hum,2);
  //    }else{
      ThingSpeak.setField(4,String(hum,2));
//      }
    } else {
      error+="hum=0 ";
    }
    if (pres ==0.0f||pres > 1150.0f) {
      error+="pres= "+ String(pres);
    } else {
//          if(networkOnlyMode){
//        talkBackCommand+=getDelimiter()+String(pres,2);
//      }else{
      ThingSpeak.setField(5,String(pres,2));
//      }
    }
  dataError = false;
  return error;
}
String getTimestamp(unsigned long t){
  return String(String(_year)+"-"+addLedaingZero(month(t))+"-"+addLedaingZero(day(t))+" "+addLedaingZero(hour(t)-2)+":"+addLedaingZero(minute(t))+":"+addLedaingZero(second(t)));
}
String addLedaingZero(int val){
 if( val< 10){
    return "0"+String(val);
  }
  return String(val);
}
void btSendSavedFields(){
  int h = 4;
  if(arrayCompressed){
    h=1;
  }
  for(int k =0; k < MEM_SIZE;k+=h){
    Serial.println(timestamps[k]);
    Serial.print(F("&&"));
     for(int g =0; g < 8;g++){
         Serial.println(String(readFromByteArr(k,g)));
         Serial.print(F("&&"));
         delay(4);
     }
  }
}
void saveFieldsToMemeory(unsigned long createdAtTime){ 
  if(fieldsCounter < MEM_SIZE){
    timestamps[timestampsCounter] = createdAtTime;
    timestampsCounter++;
    if(!arrayCompressed){
    saveAsByteArr(fieldsCounter,0,shtTempO);
    saveAsByteArr(fieldsCounter,1,temp);
    saveAsByteArr(fieldsCounter,2,shtHumO);
    saveAsByteArr(fieldsCounter,3,hum);
    saveAsByteArr(fieldsCounter,4,pres);
    saveAsByteArr(fieldsCounter,5,averangeWind);
    saveAsByteArr(fieldsCounter,6,maxCurrentWind);
    saveAsByteArr(fieldsCounter,7,rain);             
    fieldsCounter+=4;
    }else{
    fields[fieldsCounter][0] = (byte)(30+shtTempO)+(byte)floor((shtTempO -(int)(shtTempO))/0.5f)*70;
    fields[fieldsCounter][1] = (byte)(temp-16)+(byte)floor((temp -(int)(temp))/0.0625f)*15;
    fields[fieldsCounter][2] = (byte)(shtHumO);
    fields[fieldsCounter][3] = (byte)(hum);
    fields[fieldsCounter][4] = (byte)(pres-959.0f)+(byte)floor((pres -(int)(pres))/0.25f)*50;
    fields[fieldsCounter][5] = (byte)(averangeWind)+(byte)floor((averangeWind -(int)(averangeWind))/0.0625f)*15;
    fields[fieldsCounter][6] = (byte)(maxCurrentWind);
    fields[fieldsCounter][7] = (byte)(rain);
    fieldsCounter++;
    connectToWiFi();
   }
    
  }else{
    compressArray();
    clearDisplay();
    display.println(F("unable to save fileds"));
    displayA();
    connectToWiFi();
  }
}
void compressArray(){
    arrayCompressed = true;
    int c = 0;
    int tmp =0;
    while(fieldsCounter >0){
      unsigned long t = readFieldsFromMemeory();
      tmp = fieldsCounter;
      fieldsCounter = c;
      saveFieldsToMemeory(t);
      fieldsCounter = tmp;
      c++;
    }
     fieldsCounter = c;
}
void saveAsByteArr(int index1,int index2, float val){
  byte *b = (byte *)&val;
  fields[index1][index2] = b[0];
  index1++;
  fields[index1][index2] = b[1];
  index1++;
  fields[index1][index2] = b[2];
  index1++;
  fields[index1][index2] = b[3];
  index1++;
}
float readFromByteArr( int index1,int index2){
  byte vBuffer[4];
  if(index1!=0){
    index1--;
  }
  vBuffer[3] =fields[index1][index2];
  index1--;
 vBuffer[2] = fields[index1][index2];
  index1--;
 vBuffer[1]= fields[index1][index2];
  index1--;
 vBuffer[0] = fields[index1][index2];
  return *(float *)&vBuffer;
}
unsigned long readFieldsFromMemeory(){ 
  unsigned long createdAtTime =timestamps[timestampsCounter];
  if(fieldsCounter > 0){
   timestampsCounter--;
  if(!arrayCompressed){
   shtTempO = readFromByteArr(fieldsCounter,0);
   temp=  readFromByteArr(fieldsCounter,1); 
   shtHumO=  readFromByteArr(fieldsCounter,2);
   hum=  readFromByteArr(fieldsCounter,3);
   pres=  readFromByteArr(fieldsCounter,4); 
   averangeWind= readFromByteArr(fieldsCounter,5);
   maxCurrentWind= readFromByteArr(fieldsCounter,6);
   rain=  readFromByteArr(fieldsCounter,7);      
   fieldsCounter-=4;
   }else{
    shtTempO = (float)fields[fieldsCounter][0] - floor(fields[fieldsCounter][0]/70)*70.0f +floor(fields[fieldsCounter][0]/70)*0.5f;
    temp=  (float)fields[fieldsCounter][1] - floor(fields[fieldsCounter][1]/15)*15.0f +floor(fields[fieldsCounter][1]/15)*0.0625f;
    shtHumO=  (float)fields[fieldsCounter][2];
    hum=  (float)fields[fieldsCounter][3];
    pres=  (float)fields[fieldsCounter][4] - floor(fields[fieldsCounter][4]/50)*50.0f +floor(fields[fieldsCounter][4]/50)*0.25f;
    averangeWind= (float)fields[fieldsCounter][5] - floor(fields[fieldsCounter][5]/15)*15.0f +floor(fields[fieldsCounter][5]/15)*0.0625f;
    maxCurrentWind= (float)fields[fieldsCounter][6];
    rain=  (float)fields[fieldsCounter][7];
    fieldsCounter--;
   }
  }
  return createdAtTime;
}
void sendData(unsigned long createdAtTime,boolean saved) { 
  displayNotification();
  caclulateAvgMeasurements();
  String error = setFieldsForChannelA();
  if(createdAtTime>0){
    ThingSpeak.setCreatedAt(getTimestamp(createdAtTime));
  }else{
    createdAtTime = getEpochTime(false);
  }
  const long id = 333150;
  int httpCode;
  if(!networkOnlyMode){
    httpCode =ThingSpeak.writeFields(id, apiKey);
  }else{
        httpCode =ThingSpeak.writeFields(556803, debugApikey);
  //  Serial.println("sendingTalkBack");
 //   Serial.println(talkBackCommand);
 //   httpCode =sendCommand(talkBackCommand,TALK_BACK_ID_A,TALK_BACK_KEY_A);
   // talkBackCommand = "";
     //   Serial.println(httpCode);
  }
  
  if(httpCode != 200){
     displayConnectionError(httpCode);
     success = false;       
     if(attempts< NUM_OF_ATTEMPTS){
      attempts++;  
      sendData(createdAtTime,saved);
     }else if(!saved){
      saveFieldsToMemeory(createdAtTime);
     }
  }else{
 //   if(networkOnlyMode){
    //  readTalkBackCommands(TALK_BACK_ID_B,TALK_BACK_KEY_B); ////////////////////// zeby od razu sprawdzilo komende zwortna uwaga moze kolidowac z limitem czasowym komend na thikspeak 
    
  //  }
      savedDataSendTime =millis();
   if(!networkOnlyMode){  
  error+= setFieldsForChannelB();
  httpCode =ThingSpeak.writeFields(556803, debugApikey); 
   }
    success = true;
    attempts =0;     
  }
  if((float)(rf_pocketsLostCounter/rf_pocketsReceivedCounter) > POCKET_LOST_RATIO_THRESHOLD){
    error+="pocket lost ratio: " + String((float)(rf_pocketsLostCounter/rf_pocketsReceivedCounter),3);
  }
  if (!error.equals("")) {
    sendError(error);
    dataError = true;
  }
}
void sendSavedData(){
  if(success&&fieldsCounter >0&&sendInterval>10000&&millis()> savedDataSendTime + 16000&&millis()-sendTime > 10000){
      unsigned long createdAtTime = readFieldsFromMemeory();
      attempts = NUM_OF_ATTEMPTS-1;
      sendData(createdAtTime,true);
       attempts = 0;
      if(fieldsCounter==0){
         arrayCompressed = false;
      }
      savedDataSendTime =millis();   
  }
}

void sendError(String error) {
 // const unsigned long id = 556803;
  //displayConnectionError(ThingSpeak.writeField(id, 1, error, debugApikey));  
  if(displayConnectionError( sendCommand(String(_year)+"-" +addLedaingZero(_month)+"-" +addLedaingZero(_day)+ " " + addLedaingZero(_hour)+":"+addLedaingZero(_minute)+":"+addLedaingZero(_second)+ " "+ error,TALK_BACK_DEBUG_ID,TALK_BACK_DEBUG_KEY))){
    return;
  }
  readTalkBackCommands(TALK_BACK_DEBUG_ID,TALK_BACK_DEBUG_KEY);
}
