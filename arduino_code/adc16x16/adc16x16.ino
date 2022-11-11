/*16x16测试程序*/

#define XA0 4
#define XA1 2
#define XA2 6
#define XA3 7

#define YA0 9
#define YA1 8
#define YA2 18
#define YA3 19

#define CS 10
#define Y0 37
#define LED_CE 5  
/*
LEC_CE 控制绿色led，同时是开关电源的控制腿, 高电平时，开启开关电源，电源输出功率比较大，具体看PW2051的文档
开关电源开启后，静态功耗也比较大,如果开关电源不开启，供电由线性稳压U15提供，输出只有100ma，但静态功耗很低 
当需要开启wifi时， 才开启开关电源, 不开启wifi时， U15供电足够了。
*/

void sel_x(uint8_t x){
  digitalWrite(XA0, x & 1);
  digitalWrite(XA1, (x >> 1) & 1);
  digitalWrite(XA2, (x >> 2) & 1);
  digitalWrite(XA3, (x >> 3) & 1);
}
void sel_y(uint8_t y){
  digitalWrite(YA0, y & 1);
  digitalWrite(YA1, (y >> 1) & 1);
  digitalWrite(YA2, (y >> 2) & 1);
  digitalWrite(YA3, (y >> 3) & 1);
}
uint16_t get_adc() {
  digitalWrite(CS,LOW);
  delayMicroseconds(50);
  uint16_t ret = analogRead(A1);
  digitalWrite(CS,HIGH);
  return ret;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_CE,OUTPUT);
  pinMode(XA0,OUTPUT);
  pinMode(XA1,OUTPUT);
  pinMode(XA2,OUTPUT);
  pinMode(XA3,OUTPUT);
  pinMode(YA0,OUTPUT);
  pinMode(YA1,OUTPUT);
  pinMode(YA2,OUTPUT);
  pinMode(YA3,OUTPUT);
  pinMode(CS,OUTPUT);
  pinMode(Y0,OUTPUT);
  digitalWrite(CS,HIGH); //disable out
  digitalWrite(Y0,LOW);
}
void loop() {
uint32_t addc=0;
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(Y0,HIGH);
  for(uint8_t i = 0; i < 16; i++) {
  analogWrite(LED_CE, i*4);
    sel_y(i);
    for(uint8_t i0 = 0; i0 < 16; i0++) {
      sel_x(i0);
      Serial.print(get_adc());
      Serial.write(' ');
    }
    Serial.println();
  }

  digitalWrite(Y0,LOW);
  Serial.printf("=======%ld\r\n",addc);
}
