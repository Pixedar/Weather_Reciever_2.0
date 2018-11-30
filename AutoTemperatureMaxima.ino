
int size = 0;
int dataSize =440;
float data[440];
float abslouteMaxTemp =-100;
float abslouteMinTemp =-100;
float lastAvg = 0;

float maxTable[] ={
0.011335028f,
0.022070985f,
0,
0.009418906f,
0.034140173f,
0.124199318f,
0.356163907f,
0.594231765f,
0.780979154f,
0.936013968f,
0.989288051f,
0.981075812f,
1.0f,
0.984945983f,
0.915449663f,
0.797914673f,
0.608242649f,
0.362355032f,
0.26364263f,
0.195025128f,
0.149101559f,
0.131409133f,
0.122212128f,
0.058693905f,
};

void initialiseAutoTempRange(){
    EEPROM.begin(40);
    float maxG = 0;
    float minG = 0;
    byte a = EEPROM.read(30);
    byte b = EEPROM.read(31);
    if(a >=50){
      maxG = ((float)(a- 50.0f)) + ((float)b/100.0f);
      maxG*=-1.0f;
    }else{
      maxG = (float)(a) + ((float)b/100.0f);
    }
    a = EEPROM.read(32);
    b = EEPROM.read(33);
    if(a >=50){
      minG = ((float)(a- 50.0f)) + ((float)b/100.0f);
      minG*=-1.0f;
    }else{
       minG = (float)(a) + ((float)b/100.0f);
    }
    _min = minG;
    _max = maxG;
    if(maxG  < -25&&maxG  > 40){
      _max = -10;
    }
    if(minG  < -25&&minG  > 40){
      _min = 40;
    }
    
   EEPROM.end();
}

void calculateTempRange(float t, int8_t h,float scale){
   if(size < dataSize){
      size++;
      data[size] = t;
   }else{
      addToArr(t);
   }
    float sum = 0;
  for(int i =0; i < size;i++){
    sum+=data[i];
  }
  float avg = sum/size;
  calcMax(t,h,scale,avg);
  calcMin(t,h,scale,avg);
  lastAvg = avg;
}

void calcMax(float f, int8_t h,float scale,float avg){
    if(abslouteMaxTemp > f){
      abslouteMaxTemp = f;
   }
  //float p = (-pow(h-12.0f,4)+2800.0f)/2800.0f;
   float p = maxTable[h];
  if(p <0){
     p =0;
  }
 
  if(avg-lastAvg >0.0f){
    if(_max < abslouteMaxTemp+10){
      _max+=(((avg))-lastAvg)*scale*p*(1-pow((_max/(abslouteMaxTemp+10)),4) );
    }
  }else{
      _max+=(((avg))-lastAvg)*scale*p;
  }


  if(f > _max){
    _max = f;
  }
}


void calcMin(float f, int8_t h,float scale,float avg){
    if(abslouteMinTemp < f){
      abslouteMinTemp = f;
   }
 // float p = (pow(h-12.0f,4))/16000.0f;
  float p = 1.0f - maxTable[h];
  if(p >1){
     p =1;
  }
  
  if(avg-lastAvg <0.0f){
    if(_min < abslouteMinTemp-10){
      _min-=(((avg))-lastAvg)*scale*p*(1-pow((_min/(abslouteMinTemp-10)),4) );
    }
  }else{
      _min-=(((avg))-lastAvg)*scale*p;
  }


  if(f < _min){
    _min = f;
  }
}


void addToArr(float x){
  float f;
   for(int i =dataSize-1; i <=0;i--){
      f = data[i];
      if(i == dataSize-1){
        data[i] = x;
      }
      if(i-1 >= 0){
        data[i-1] = f;
      }
   }
}

