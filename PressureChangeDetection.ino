#define SHORT_TIME_CHANGE_INTERVAL 300
#define LONG_TIME_CHANGE_INTERVAL 10000
#define INTERPOLATION_INTERVAL 10
const float wpIncrement = 1.0f/(((SHORT_TIME_CHANGE_INTERVAL-50)/INTERPOLATION_INTERVAL));
const long delayThreshold = SHORT_TIME_CHANGE_INTERVAL/3;
unsigned long interpolationTime;
unsigned long shortChnageTime; 
unsigned long longChnageTime; 
float shortChangeArr[120];
const short shortChnageArrLeght = sizeof(shortChangeArr)/sizeof(float);
float lastShortChange = 0.0f;
int cf = 0;
int cf2 = 0;
float lastQ =0;
boolean enableMinPressureChangeDetection =false;
float lastTestSecondIIRFilterChange = 0;
int startCtn =0;

void detectPressureChange(float currentPressure){

    if(millis() > shortChnageTime + SHORT_TIME_CHANGE_INTERVAL&&!isnan(currentPressure)){
      if(millis() - (shortChnageTime+ SHORT_TIME_CHANGE_INTERVAL) > delayThreshold){
        updateShortTimeArr((0.5f*currentPressure)+(0.5f*lastPressure));
      }else{
        updateShortTimeArr(currentPressure);
      }
      lastPressure = currentPressure;
      
     if(enableMinPressureChangeDetection){
       double shortChange;
       int cfInfex = cf-1;
       if(cfInfex+1 > shortChnageArrLeght-1){
       shortChange = shortChangeArr[cfInfex] - shortChangeArr[0];
       }else{
        shortChange = shortChangeArr[cfInfex] - shortChangeArr[cfInfex+1];
       }
       shortChange*=100.0f;
        double testChange = lastShortChange*0.99995d+ shortChange*0.00005d;
         
         lastShortChange = testChange;
         double secIIRTestFilter = lastTestSecondIIRFilterChange*0.5d + testChange*0.5d;
         lastTestSecondIIRFilterChange = secIIRTestFilter;
         if(secIIRTestFilter < 0.0d){
            secIIRTestFilter*=(-1.0d);
         }
         pressureChange =pow(secIIRTestFilter,1.0d/2.5d) *6.895f;


       if(pressureChange > maxPressureChange[0]){
         maxPressureChange[0] = pressureChange;
       }else if(pressureChange <minPressureChange[0]){
         minPressureChange[0] = pressureChange;
       }


               if(debugPressureChange){
          Serial.println("PPPPP");
          Serial.println(String(enableMinPressureChangeDetection));
          Serial.println(String(pressureChange));
          Serial.println(String(maxPressureChange[0]));
          Serial.println(String(minPressureChange[0]));
          Serial.println(String(shortChange));
         
         if(cfInfex+1 < shortChnageArrLeght-1){
             Serial.println(String(shortChangeArr[cfInfex]));
              Serial.println(String(shortChangeArr[cfInfex+1]));
          }
          Serial.println(String(cf));
     
//          Serial.println(maxPressureChange[1]);
//          Serial.println(String(250 -map(pressureChange*100,minPressureChange[0]*100,maxPressureChange[0]*100,0,(250+hsv_h_ext)*10)/10.0f));
//          Serial.println(String(250+hsv_h_ext -map(pressureChange*100,minPressureChange[0]*100,maxPressureChange[0]*100,0,(250+hsv_h_ext)*10)/10.0f));
                Serial.println(String(250+hsv_h_ext -map(pressureChange*10,minPressureChange[0]*10,maxPressureChange[0]*10,0,(250+hsv_h_ext)*10)/10.0f));
          Serial.println(String(250+hsv_h_ext -map(pressureChange,minPressureChange[0],maxPressureChange[0],0,(250+hsv_h_ext))));
//            Serial.println(String(map(pressureChange,minPressureChange[0],maxPressureChange[0],0,250+hsv_h_ext)));
          Serial.println("PPPPP");
       }

       
      }
       
       if(pressureChangeEnabled){
         lastR = r;
         lastB = b;
         lastG = g;
         setLedColor(colorMode);
         wp = 0;
       }

      
       shortChnageTime = millis();
    }
    if(millis() > interpolationTime +INTERPOLATION_INTERVAL){
      if(wp < 1.0f){
        wp+=wpIncrement;
      }
      ledAnalogWrite();
      interpolationTime = millis();
    }
}



void updateShortTimeArr(float change){
  if(cf >= shortChnageArrLeght){
          cf = 0;
          if(!enableMinPressureChangeDetection&&startCtn < 7){
            startCtn++;
          }else{
            enableMinPressureChangeDetection =true;
          }
   }
   shortChangeArr[cf] = change;
   cf++;       
}


