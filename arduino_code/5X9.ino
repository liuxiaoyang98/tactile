/*16x16测试程序*/

#define A0 1
#define A1 2
#define A2 4
#define A3 5
#define A4 6

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
uint16_t adc[6];
uint16_t gpio[10][6];
/*
LEC_CE 控制绿色led，同时是开关电源的控制腿, 高电平时，开启开关电源，电源输出功率比较大，具体看PW2051的文档
开关电源开启后，静态功耗也比较大,如果开关电源不开启，供电由线性稳压U15提供，输出只有100ma，但静态功耗很低 
当需要开启wifi时， 才开启开关电源, 不开启wifi时， U15供电足够了。
*/

//void sel_x(uint8_t x){
//  digitalWrite(XA0, x & 1);
//  digitalWrite(XA1, (x >> 1) & 1);
//  digitalWrite(XA2, (x >> 2) & 1);
//  digitalWrite(XA3, (x >> 3) & 1);
//}
//void sel_y(uint8_t y){
//  digitalWrite(YA0, y & 1);
//  digitalWrite(YA1, (y >> 1) & 1);
//  digitalWrite(YA2, (y >> 2) & 1);
//  digitalWrite(YA3, (y >> 3) & 1);
//}
void get_adc(uint8_t pin) {
  digitalWrite(CS,LOW);
  delayMicroseconds(50);
  uint16_t ret[6];
  
  uint16_t Max=0,Min=0xffff,sum=0;
  
  for (uint8_t i=1;i<6;i++){
    for(uint8_t i0=0;i0<6;i++){//测5次取平均
      uint16_t temp=analogRead(i);
      sum=sum+temp;
      if(Max<temp)Max=temp;
      if(Min>temp)Min=temp;
    }
    gpio[pin][i]=(sum-Max-Min)/3;
  }
  digitalWrite(CS,HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_CE,OUTPUT);
//  pinMode(A0,OUTPUT);
//  pinMode(A1,OUTPUT);
//  pinMode(A2,OUTPUT);
//  pinMode(A3,OUTPUT);
//  pinMode(A4,OUTPUT);
  
  pinMode(Y1,INPUT);pinMode(Y2,INPUT);
  pinMode(P19,INPUT);pinMode(P18,INPUT);  
  pinMode(P10,INPUT);pinMode(P9,INPUT);
  pinMode(P8,INPUT);pinMode(P7,INPUT);
  pinMode(P6,INPUT);

  
  pinMode(CS,OUTPUT);
  pinMode(Y0,OUTPUT);
  digitalWrite(CS,HIGH); //disable out
  digitalWrite(Y0,LOW);
}
void loop() {
uint32_t addc=0;
  // put your main code here, to run repeatedly:
  delay(1000);
  //digitalWrite(Y0,HIGH);
  uint8_t io[10]={22,21,20,19,18,17,16,13,12};
  for(uint8_t i=0;i<9;i++){
    pinMode(io[i],OUTPUT);
    digitalWrite(io[i],HIGH);
    get_adc(i);
  }
  for(uint8_t i=0;i<6;i++)
      for(uint8_t i0=0;i<10;i++)
        Serial.print(gpio[i][i0]);
  
  
  digitalWrite(Y0,LOW);
  Serial.printf("=======%ld\r\n",addc);
}
