WiFiClient moduleB_client;
WiFiServer server(100); 
byte tmpBuf2[4];

void initServer(){
  server.begin();
  

}

void handle_module_B(){
    moduleB_client = server.available();
//  if(!moduleB_client.connected()){
//    moduleB_client.flush();
//    moduleB_client.stop();
//    moduleB_client = server.available();
//  }
if(!moduleB_client){
 // Serial.println("NO CLIENT !!");
  return;
}
if(!moduleB_client.available()){
 // Serial.println("NOT AVAILABLE !!");
  return;
}

//  if(!moduleB_client.available()||!moduleB_client){
//    return;
//  }
  Serial.println("read init");
//  while(moduleB_client.available()){
//    Serial.write(moduleB_client.read());
//  }
boolean f = false;
  float test = readFloat2(f);
  r_freq = (int)readInt2(f);
  g_freq = (int)readInt2(f);
  b_freq = (int)readInt2(f);
  int cd = (int)readInt2(f);
    if(r_freq > max_r_freq){
    max_r_freq = r_freq;
  }
  if(r_freq < min_r_freq){
    min_r_freq = r_freq;
  }
  if(g_freq > max_g_freq){
    max_g_freq = g_freq;
  }
  if(g_freq < min_g_freq){
    min_g_freq = g_freq;
  }

      if(b_freq > max_b_freq){
    max_b_freq = b_freq;
  }
  if(b_freq < min_b_freq){
    min_b_freq = b_freq;
  }
  
//  Serial.println(r_freq);
 // Serial.println(g_freq);
 // Serial.println(b_freq);
///  String request = client.readStringUntil('\r');    // receives the message from the client
//      Serial.print("From client: "); Serial.println(request);

     df();
   //   moduleB_client.flush();
  //    df();
       Serial.println("flush end");
    //  moduleB_client.stop();
   //   df();
      ///  Serial.println("client stopped end");
}
unsigned long tstCtn =0 ;
unsigned long qw2 = 0;
double skj = 0;
void df(){
  tstCtn++;
   Serial.print(tstCtn);
   Serial.print(" ");
  Serial.print("FreeHeap ");
Serial.print(ESP.getFreeHeap(),DEC);
Serial.print(" getHeapFragmentation ");
Serial.print(ESP.getHeapFragmentation(),DEC);
Serial.print(" getMaxFreeBlockSize ");
Serial.print(ESP.getMaxFreeBlockSize(),DEC);
Serial.print(" ");
int cv = millis() - qw2;
skj+=cv;
Serial.print(cv);
Serial.print(" ms ");
Serial.println(skj/tstCtn);

qw2 = millis();
}
uint16_t readInt2(boolean &flag){
   if(!moduleB_client.available()){ flag = true; return 0;}
   tmpBuf2[0] = moduleB_client.read();
   if(!moduleB_client.available()){ flag = true; return 0;}
   tmpBuf2[1] = moduleB_client.read();
  return *((uint16_t*)(tmpBuf2));
}

float readFloat2(boolean &flag){
      if(!moduleB_client.available()){ flag = true; return 0;}
      tmpBuf2[0] = moduleB_client.read();
      if(!moduleB_client.available()){ flag = true; return 0;}
      tmpBuf2[1] = moduleB_client.read();
      if(!moduleB_client.available()){ flag = true; return 0;}
      tmpBuf2[2] = moduleB_client.read();
      if(!moduleB_client.available()){ flag = true; return 0;}
      tmpBuf2[3] = moduleB_client.read();
      float result = *((float*)(tmpBuf2));
      return result;
}
