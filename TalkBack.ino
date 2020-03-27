


//const unsigned long myTalkBackID_A =  28333;
//const char * myTalkbackKey = "GKUDL9YXUNXZG425";
unsigned long checkIntervalTime =0;



char DELIMITER_A = 'Q';
#define DELIMITER_B 'Z'
int CHECK_INTERVAL  = 15000;


// General function to POST to ThingSpeak
int httpPOST(String uri, String postMessage, String &response){

  bool connectSuccess = false;
  connectSuccess = client.connect("api.thingspeak.com",80);

  if(!connectSuccess){
      return -301;   
  }
  
  postMessage += "&headers=false";
  
  String Headers =  String("POST ") + uri +
                    String(" HTTP/1.1\r\nHost: api.thingspeak.com\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\nContent-Length: ") +
            //        String("Content-Type: application/x-www-form-urlencoded\r\n") +
             //       String("Connection: close\r\n") +
                     String(postMessage.length()) +
                    String("\r\n\r\n");

  client.print(Headers);
  client.print(postMessage);

  long startWaitForResponseAt = millis();
  while(client.available() == 0 && millis() - startWaitForResponseAt < 5000){
      delay(100);
  }

  if(client.available() == 0){       
    return -304; // Didn't get server response in time
  }

  if(!client.find(const_cast<char *>("HTTP/1.1"))){
      return -303; // Couldn't parse response (didn't find HTTP/1.1)
  }
  
  int status = client.parseInt();
  if(status != 200){
    return status;
  }

  if(!client.find(const_cast<char *>("\n\r\n"))){
    return -303;
  }

  String tempString = String(client.readString());
  response = tempString;
  
  return status;
    
}
int sendCommand(String command, long id, String key){

   String tbURI = String("/talkbacks/") + String(id) + String("/commands/");
   String postMessage =  String("api_key=") + key+"&&command_string="+command; 
   String newCommand = ""; 
   int x = httpPOST(tbURI, postMessage, newCommand);
   client.stop();
   return x;
}

 void readTalkBackCommands(long id, String key){
 //if(millis() > checkIntervalTime + CHECK_INTERVAL){

  String tbURI = String("/talkbacks/") + String(id) + String("/commands/execute");
  String postMessage =  String("api_key=") + key;  

  String newCommand = "";

  int x = httpPOST(tbURI, postMessage, newCommand);
  client.stop();

  if(x == 200){
    handleCommand(newCommand,id);
  }
//     checkIntervalTime =millis(); 
 // }
 }
 void handleCommand(String newCommand,long id){
    switch(id){
      case TALK_BACK_ID_B:
        if(newCommand.charAt(0) == DELIMITER_B){
         readOutsideData(newCommand);
      }
      break;
    }

 }
char getDelimiter(){
  return DELIMITER_A;
}
void readOutsideData(String str){
 String srA= getValue(str,DELIMITER_B,1);
 shtTempO = srA.toFloat();
 srA= getValue(str,DELIMITER_B,2);
 shtHumO = srA.toInt();
 srA= getValue(str,DELIMITER_B,3);
 averangeWind = srA.toFloat();
 srA= getValue(str,DELIMITER_B,4);
 maxCurrentWind= srA.toFloat();
 srA= getValue(str,DELIMITER_B,5);
 rain= srA.toInt();
 }
