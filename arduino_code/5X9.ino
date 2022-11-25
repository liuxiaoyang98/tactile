/*5x9测试程序*/
#define Y1  22
#define Y2  21
#define P19 20
#define P18 19

#define P9  18
#define P10 17
#define P8  16
#define P7  13
#define P6  12

#define CS 10
#define Y0 37
#define LED_CE 5  
uint8_t adc[6]={A0,A1,A2,A3,A4};
uint8_t io[10]={Y1,Y2,P19,P18,P9,P10,P8,P7,P6};
uint16_t gpio[10][6];

void get_adc() {
  Serial.print("good");
//  delayMicroseconds(50);
//  
//  uint16_t ret[6];
//  
//  for(uint8_t pin=0;pin<9;pin++){//9个gpio
//
//    pinMode(io[pin],OUTPUT);
//    digitalWrite(io[pin],HIGH);
//    
//    uint16_t Max=0,Min=0xffff,sum=0;
//
//    
//      for (uint8_t i=0;i<5;i++){//5个adc
//        for(uint8_t i0=1;i0<6;i++){//测5次取平均
//          uint16_t temp=analogRead(adc[i]);
//          sum=sum+temp;
//          if(Max<temp)Max=temp;
//          if(Min>temp)Min=temp;
//        }
//    gpio[pin][i]=(sum-Max-Min)/3;
//    }
//
//    
//    pinMode(io[pin],INPUT);
//  }

}

void setup() {
  Serial.end();
  // put your setup code here, to run once:
  Serial.begin(115200);
//  pinMode(LED_CE,OUTPUT);
//  pinMode(A0,OUTPUT);
//  pinMode(A1,OUTPUT);
//  pinMode(A2,OUTPUT);
//  pinMode(A3,OUTPUT);
//  pinMode(A4,OUTPUT);
  
  Serial.print("1good");
  pinMode(22,INPUT);//good
//  pinMode(21,INPUT);//TX不管它
  pinMode(P19,INPUT);//good
  pinMode(P18,INPUT); //good

  
//  pinMode(P10,INPUT);//代码通过之后到loop()就断了
  pinMode(P9,INPUT);//good
//  pinMode(P8,INPUT);//代码走到这里就断了
//  pinMode(P7,INPUT);//代码走到这里就断了
//  pinMode(P6,INPUT);//代码通过之后到loop()就断了

  Serial.print("2good"); 
  Serial.println(""); 
//
//  
//  pinMode(CS,OUTPUT);
//  pinMode(Y0,OUTPUT);
//  digitalWrite(CS,HIGH); //disable out
//  digitalWrite(Y0,LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  //digitalWrite(Y0,HIGH);
  
  Serial.print("loop");
  get_adc();

//2 3 5

int lop[]={2,3,5};
for(int pin=0;pin<3;pin++){
    pinMode(io[pin],OUTPUT);
    digitalWrite(io[pin],HIGH);
    
    uint16_t Max=0,Min=0xffff,sum=0;

    
      for (uint8_t i=0;i<5;i++){//5个adc
        for(uint8_t i0=1;i0<6;i++){//测5次取平均
          uint16_t temp=analogRead(adc[i]);
          sum=sum+temp;
          if(Max<temp)Max=temp;
          if(Min>temp)Min=temp;
        }
    gpio[lop][i]=(sum-Max-Min)/3;
    }

    
    pinMode(io[pin],INPUT);
}
  

//  
//  for(uint8_t i=0;i<6;i++){//display
//      for(uint8_t i0=0;i0<10;i0++)
//        Serial.print(gpio[i][i0]);
//      Serial.println();
//  }
  
//  digitalWrite(Y0,LOW);
}
