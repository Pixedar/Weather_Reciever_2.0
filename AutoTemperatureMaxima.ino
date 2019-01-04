float scale = 0.46f;
int size = 0;
int dataSize =440;
float data[440];
float abslouteMaxTemp =-100;
float abslouteMinTemp =-100;
float lastAvg = 0;
float sc = 1.0f;
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
    EEPROM.end();

    if(maxG  < -25||maxG  > 40){
      _max = -10;
    }else{
      _max = maxG;
      abslouteMaxTemp = _max +0.5;
    }
   
    if(minG  < -25||minG  > 40){
      _min = 40;
    }else{
      _min = minG;
      abslouteMinTemp = _min -0.5;
   }

}

void calculateTempRange(float t, int8_t h){
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
  sc =(float)(sendInterval/180000.0f);
  float avg = sum/size;
  calcMax(t,h,scale*sc,avg);
  calcMin(t,h,scale*sc,avg);
  lastAvg = avg;
}

void calcMax(float f, int8_t h,float scale,float avg){
    if(f >abslouteMaxTemp){
      abslouteMaxTemp = f;
   }
   abslouteMaxTemp-=0.0000288f*sc;
  //float p = (-pow(h-12.0f,4)+2800.0f)/2800.0f;
   float p = maxTable[h];

//      Serial.println("f" + String(f));
//    Serial.println("h" + String(h));
//  Serial.println("scale" + String(scale));
//   Serial.println("avg" + String(avg));
//  Serial.println("s" + String(p));
  if(avg-lastAvg >0.0f){
    if(_max < abslouteMaxTemp+10){
      _max+=(((avg))-lastAvg)*scale*p*(1-pow((_max/(abslouteMaxTemp+10)),4) );
 //     Serial.println("p1" + String((((avg))-lastAvg)*scale*p*(1-pow((_max/(abslouteMaxTemp+10)),4) )));
    }
  }else{
      _max+=(((avg))-lastAvg)*scale*p;
  //     Serial.println("p2" + String((((avg))-lastAvg)*scale*p));
  }


  if(f > _max){
    _max = f;
  }
}


void calcMin(float f, int8_t h,float scale,float avg){
    if(f < abslouteMinTemp){
      abslouteMinTemp = f;
   }
    abslouteMinTemp+=0.0000288f*sc;
 // float p = (pow(h-12.0f,4))/16000.0f;
  float p = 1.0f - maxTable[h];
  
  if(avg-lastAvg <0.0f){
    if(_min > abslouteMinTemp-10){
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
  float f = data[dataSize-1];
    data[dataSize-1] = x;
   for(int i =0; i< dataSize-2;i++){
        data[i] = data[i+1];
   }
   data[dataSize-2] = f;
}

