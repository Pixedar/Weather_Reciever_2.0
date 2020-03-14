const char * apiKey = "12RUI1FA6G5C5CWM"; 
const char * debugApikey = "V9BDERAUY8VGWJU7";
const char * autoRangeApikey = "T44K8QM9YHVD73WG";
#define TALK_BACK_ID_A 28333
#define TALK_BACK_ID_B 37669
#define TALK_BACK_DEBUG_ID 37719
#define TALK_BACK_KEY_A "GKUDL9YXUNXZG425"
#define TALK_BACK_KEY_B "D60O4AD1A7RLMUI7"
#define TALK_BACK_DEBUG_KEY "QFJRR6QS7OBFFPAK"
float lastTempO =0;
float lastAvgWind =0;
float lastWind =0;
int lastRain =0;
int lastHumO =0;
boolean firstTime = true;
#define NUM_OF_ATTEMPTS 4
#define MEM_SIZE 1000 // uwaga bylo 2000 ale ide pokazywalo ze za malo apmieci i moga byc probelmy ze stabuilnoscia mimo to esp dzialalo stabilnie
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
String setFields(boolean saved){
  String error = "";
    if (signalReceived) {
      if (humO ==0&&tempO==0) {
        error+="dht error ";
      } else {
        if (tempO <-39||tempO >39.9||((abs(lastTempO-tempO)>3.85f)&&!firstTime)) {
          error+="tempO= "+String(tempO);
        } else if(!networkOnlyMode){
          ThingSpeak.setField(1, String(tempO,2));
          if(!saved){
             lastTempO = tempO; 
          }
        }
        if (humO >100||humO < 0||((abs(lastHumO - humO)>35)&&!firstTime)) {
          error+="humO= "+String(humO);
        } else if(!networkOnlyMode) {
            ThingSpeak.setField(3, humO);
            if(!saved){   
                lastHumO = humO;
            }
        }
      }  

      if(averangeWind <0|| averangeWind >49||((abs(lastAvgWind - averangeWind) > 10)&&!firstTime)){
        error+="averangeWind= "+String(averangeWind);
      }else if(!networkOnlyMode){ 
        ThingSpeak.setField(6,String(averangeWind,2));
        if(!saved){
          lastAvgWind =  averangeWind;
        }
      }
      
      if (maxCurrentWind <0||maxCurrentWind >70||(abs(lastWind - maxCurrentWind)>40)&&!firstTime) {
        error+="maxCurrentWind= "+String(maxCurrentWind);
      } else if(!networkOnlyMode){
        ThingSpeak.setField(7,maxCurrentWind);
         if(!saved){
          lastWind = maxCurrentWind;
         }
      }
      
      if(rain <0|| rain >99||(abs(lastRain - rain) > 30&&!firstTime)){
        error+="rain= "+String(rain);
      }else{
          ThingSpeak.setField(8,rain);  
         if(!saved){
            lastRain = rain;
         }
      }
   
      if(firstTime&&!networkOnlyMode){
 
        
        firstTime = false;
      } 
    }
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
    saveAsByteArr(fieldsCounter,0,tempO);
    saveAsByteArr(fieldsCounter,1,temp);
    saveAsByteArr(fieldsCounter,2,humO);
    saveAsByteArr(fieldsCounter,3,hum);
    saveAsByteArr(fieldsCounter,4,pres);
    saveAsByteArr(fieldsCounter,5,averangeWind);
    saveAsByteArr(fieldsCounter,6,maxCurrentWind);
    saveAsByteArr(fieldsCounter,7,rain);             
    fieldsCounter+=4;
    }else{
    fields[fieldsCounter][0] = (byte)(30+tempO)+(byte)floor((tempO -(int)(tempO))/0.5f)*70;
    fields[fieldsCounter][1] = (byte)(temp-16)+(byte)floor((temp -(int)(temp))/0.0625f)*15;
    fields[fieldsCounter][2] = (byte)(humO);
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
   tempO = readFromByteArr(fieldsCounter,0);
   temp=  readFromByteArr(fieldsCounter,1); 
   humO=  readFromByteArr(fieldsCounter,2);
   hum=  readFromByteArr(fieldsCounter,3);
   pres=  readFromByteArr(fieldsCounter,4); 
   averangeWind= readFromByteArr(fieldsCounter,5);
   maxCurrentWind= readFromByteArr(fieldsCounter,6);
   rain=  readFromByteArr(fieldsCounter,7);      
   fieldsCounter-=4;
   }else{
    tempO = (float)fields[fieldsCounter][0] - floor(fields[fieldsCounter][0]/70)*70.0f +floor(fields[fieldsCounter][0]/70)*0.5f;
    temp=  (float)fields[fieldsCounter][1] - floor(fields[fieldsCounter][1]/15)*15.0f +floor(fields[fieldsCounter][1]/15)*0.0625f;
    humO=  (float)fields[fieldsCounter][2];
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
  String error = setFields(saved);
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
    success = true;
    attempts =0; 
    savedDataSendTime =millis();    
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

boolean displayConnectionError(int httpCode ){
   if(httpCode != 200){
     clearDisplay();
     display.print(F("HTTP ERROR: "));
     display.println(String(httpCode));
     display.println(F("ATTEMPT: "));
     display.print(String(attempts));
     displayA();
     delay(1500);
     return true;
  }else{
    return false;
  }
}
boolean displayConnectionError(int httpCode,int del){
   if(httpCode != 200){
     clearDisplay();
     display.print(F("HTTP ERROR: "));
     display.println(String(httpCode));
     displayA();
     delay(del);
     return true;
  }else{
    return false;
  }
}
void displayNotification(){
        if(clockColorMode){
        display.setCursor(120,52);
        display.print(F("s"));
      }else{
         display.fillRect(0,60,128,3,BLACK);
        display.setCursor(83,52);
        display.print(F("sending"));
      }
      displayA(); 
}




