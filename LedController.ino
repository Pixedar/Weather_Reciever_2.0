#define RGB_MAX 1023
#define HSV_H_MAX 250
#define HSV_V_MAX 100
#define HSV_PLUS_COEFFICIENT 1.3f
#define HEAT_MODE_MAX 60
const int hsvPlus_h_max = round(pow(10*HSV_V_MAX,1.0f/HSV_PLUS_COEFFICIENT));
const int hsvPlus_h_min = round(pow(10,1.0f/HSV_PLUS_COEFFICIENT));
void setLedColor(int8_t _mode){
  clockColorMode = false;
  switch(_mode){
   case 0:
    r = getColor(colorModesRGB[0][0],0,RGB_MAX);
    g = getColor(colorModesRGB[0][1],0,RGB_MAX);
    b = getColor(colorModesRGB[0][2],0,RGB_MAX);
   break;
   case 1:
    r = getColor(colorModesRGB[1][0],0,RGB_MAX);
    g = getColor(colorModesRGB[1][1],0,RGB_MAX);
    b = getColor(colorModesRGB[1][2],0,RGB_MAX);
   break;
   case 2:
   if(colorModesHSV[0] !=0&&colorModesHSV[0] < 10){
      setHSV(0);
   }else if(colorModesHSV[0] ==0){
      setColockColor();
   }else if(colorModesHSV[0] > 9&&colorModesHSV[0]< 20){
     setHSVPlus(0);
   }else{
     setHeatMode(0);
   }
   break;
   case 3:
   if(colorModesHSV[1] !=0&&colorModesHSV[0] < 10){
   HSV_to_RGB(HSV_H_MAX -getColor(colorModesHSV[1],0,HSV_H_MAX+hsv_h_ext),100,100);
   }else if(colorModesHSV[0] ==0){
    setColockColor();
   }else if(colorModesHSV[0] > 9&&colorModesHSV[0]< 20){
    setHSVPlus(1);
   }else{
      setHeatMode(1);
   }
   break;
  }
//  Serial.println("rgb");
//  Serial.println(r);
 //  Serial.println(g);
 //   Serial.println(b);
 //   Serial.println("================================================");
}
void setHSV(const int8_t index){
  HSV_to_RGB(HSV_H_MAX+hsv_h_ext -getColor(colorModesHSV[index],0,HSV_H_MAX+hsv_h_ext),100,100);
}
void setHSVPlus(const int8_t index){
   const int val = getColor(colorModesHSV[index]-10,hsvPlus_h_min,hsvPlus_h_max);  
   HSV_to_RGB(HSV_H_MAX+hsv_h_ext -(getColor(colorModesHSV[index]-10,0,(HSV_H_MAX+hsv_h_ext)*10)/10.0f),100,pow(val,HSV_PLUS_COEFFICIENT)/10.0f); 
}
void setHeatMode(const int8_t index){  
  float norm = getColor(colorModesHSV[index]-20,0,1100)/1100.0f;  
  float h = HEAT_MODE_MAX;
  float b = 100.0f;
 if(norm <=1.0f){
  if(norm <=0.75f){
     float j = mapF(norm,0,0.75f,0,1.0f);
     h= mapF(j*j*j,0,1.0f,0,HEAT_MODE_MAX);
  }
  b = mapF(pow(norm,0.4f),0.0f,1.0f,1.0f,100.0f);
 }
  float s = 100.0f;
  if(norm >0.6f){
    s = 100.0f - mapF(norm*norm*norm,0.216f,1.331f,0,100.0f);
  }
 
  HSV_to_RGB(h,s,b);
}
void setColockColor(){
    r = map(_hour,0,23,0,1023);
    g = map(_minute,0,59,0,1023);
    b = map(_second,0,59,0,1023); 
    clockColorMode = true;
}
String getColorName(int8_t value){
  switch(value){
    case 3: return "cisnienie ";
    case 1: return "temp wewn ";
    case 2: return "wilg wewn ";
    case 4: return "temp zewm ";
    case 5: return "wilg zewm ";
    case 7: return "wiatr ";
    case 6: return "wiatr sr ";
    case 8: return "deszcz ";
    case 9: return "zmina cisn";
    default: return "";
  }
}
int getColor(int8_t value,int n,int m){
    return getVal(rangeMode[value-1],n,m,value);
}
int getVal(int index,int n,int m,int8_t value){
    switch(value){
    case 3: return normalize(currentPressure+PRESSURE_FIX,minPressure[index],maxPressure[index],m,n);
    case 1:
 //   Serial.println(shtCurrentTemp);
 //   Serial.println(maxInsideTemp[index]);
 //    Serial.println(minInsideTemp[index]);
  //    Serial.println(m);
   //      Serial.println(n);
    //          Serial.println(normalize(shtCurrentTemp,maxInsideTemp[index],minInsideTemp[index],m,n));
    return normalize(shtCurrentTemp,maxInsideTemp[index],minInsideTemp[index],m,n);
    case 2: return normalize(shtCurrentHum,maxInsideHum[index],minInsideHum[index],m,n);
    case 4: return normalize(currentShtTempO,_max[index],_min[index],m,n);
    case 5: return normalize(currentShtHumO,minOutsideHum[index],maxOutsideHum[index],n,m);
  //  case 6: return map(((currentWind + 0.5*lastCurrentWind)/1.5f)*k,0,maxWind*k,m,n);
    case 7: return normalize(windColor,0,maxWind[index],n,m);
     case 6: return normalize(interpolatedAverangeWind,0,maxAverangeWind[index],n,m);
  //  case 6: return normalize(averangeWind,0,maxAverangeWind[index],n,m);
    case 8: return normalize(rain,0,maxRain[index],n,m);
    case 9: return normalize2(pressureChange,minPressureChange[index],maxPressureChange[index],n,m);
    default: return m;
    }
}
int normalize(float val, float a, float b, int m, int n){
   // Serial.println("norm");
 if(a>b){
  if(val > a){
    val = a;
  }else if(val < b){
    val = b;
  }
 }else{
  if(val < a){
    val = a;
  }else if(val > b){
    val = b;
  }
 }
 const int k = 100;
  //   Serial.println(val*k);
  //   Serial.println(a*k);
   //   Serial.println(b*k);
  //          Serial.println(map(val*k,a*k,b*k,m,n));
 return map(val*k,a*k,b*k,m,n);
}
int normalize2(float val, float a, float b, int m, int n){
  if(val < a){
    val = a;
  }else if(val > b){
    val = b;
  } 
 return map(val,a,b,m,n);
}
void HSV_to_RGB(float h, float s, float v)
{
  float f,p,q,t;
  int i;
  int range = 1022;
  h = max(0.0f, min(360.0f, h));
  s = max(0.0f, min(100.0f, s));
  v = max(0.0f, min(100.0f, v));
  
  s /= 100;
  v /= 100;
  
  if(s == 0) {
    // Achromatic (grey)
    r = g = b = round(v*range);
    return;
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));
  switch(i) {
    case 0:
      r = round(range*v);
      g = round(range*t);
      b = round(range*p);
      break;
    case 1:
      r = round(range*q);
      g = round(range*v);
      b = round(range*p);
      break;
    case 2:
      r = round(range*p);
      g = round(range*v);
      b = round(range*t);
      break;
    case 3:
      r = round(range*p);
      g = round(range*q);
      b = round(range*v);
      break;
    case 4:
      r = round(range*t);
      g = round(range*p);
      b = round(range*v);
      break;
    default: // case 5:
      r = round(range*v);
      g = round(range*p);
      b = round(range*q);
    }
}
float mapF(float value,float start1, float stop1,float start2, float stop2){
      return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}
void ledAnalogWrite(){
   if(!lightLock){
    analogWrite(BLUE_PIN,1023 -((1-wp)*lastB + wp*b)*brightness);
    analogWrite(GREEN_PIN,1023 -((1-wp)*lastG + wp*g)*brightness);
    analogWrite(RED_PIN,1023 -((1-wp)*lastR + wp*r)*brightness);
   }
}
