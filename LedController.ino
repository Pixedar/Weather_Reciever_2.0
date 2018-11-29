


void setLedColor(int8_t _mode){
  clockColorMode = false;
  switch(_mode){
   case 0:
    r = getColor(colorModesRGB[0][0],false);
    g = getColor(colorModesRGB[0][1],false);
    b = getColor(colorModesRGB[0][2],false);
   break;
   case 1:
    r = getColor(colorModesRGB[1][0],false);
    g = getColor(colorModesRGB[1][1],false);
    b = getColor(colorModesRGB[1][2],false);
   break;
   case 2:
   if(colorModesHSV[0] !=0){
   HSV_to_RGB(265 -getColor(colorModesHSV[0],true),100,100);
   }else{
    r = map(_hour,0,23,0,1023);
    g = map(_minute,0,59,0,1023);
    b = map(_second,0,59,0,1023);
    clockColorMode = true;
   }
   break;
   case 3:
   if(colorModesHSV[1] !=0){
   HSV_to_RGB(265 -getColor(colorModesHSV[1],true),100,100);
   }else{
    r = map(_hour,0,23,0,1023);
    g = map(_minute,0,59,0,1023);
    b = map(_second,0,59,0,1023);
    clockColorMode = true;
   }
   break;
  }
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
    default: return "";
  }
}
int getColor(int8_t value,boolean hsv){
  int m;
  int8_t k = 10;
  if(hsv){
    m = 265;
  }else{
    m =1023;
  }
  switch(value){
    case 3: return map(pres*k,maxPressure*k,minPressure*k,m,0);
    case 1: return map(temp*k,maxInsideTemp*k,minInsideTemp*k,m,0);
    case 2: return map(hum*k,maxInsideHum*k,minInsideHum*k,m,0);
    case 4: 
     if(autoTempRangeMode){
       return map(tempO,_max,_min,m,0);
     }else{
       return map(tempO*k,_max,_min,m,0);
     }
    case 5: return map(humO,minOutsideHum,maxOutsideHum,0,m);
  //  case 6: return map(((currentWind + 0.5*lastCurrentWind)/1.5f)*k,0,maxWind*k,m,0);
    case 7: return map(windColor*k,0,maxWind*k,0,m);
    case 6: return map(averangeWind*k,0,maxAverangeWind*k,0,m);
    case 8: return map(rain,0,maxRain,0,m);
    default: return m;
  }
}


void HSV_to_RGB(float h, float s, float v)
{
  float f,p,q,t;
  int i;
  int range = 1022;
  h = _max(0.0, _min(360.0, h));
  s = _max(0.0, _min(100.0, s));
  v = _max(0.0, _min(100.0, v));
  
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


