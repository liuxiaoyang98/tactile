#include <TimerOne.h>
#include <stdint.h>
int adc[8][2];// 8个gpio 2个adc管理16个传感器
int stat;//中断后的拿去修改标记，方便loop检测到了数据更新
void setup() {
  
  Serial.begin(115200);
  uint16_t freq=50;//采样率
  stat = 0;
  Timer1.initialize(1000000/freq);
  Timer1.attachInterrupt(collect_data);

  analogReference(INTERNAL);//3.3v标准电压
  for(int i=2;i<=9;i++){//gpio改为输入
    pinMode(i, INPUT);
  }
  interrupts();
}
float get_adc(int pin){//获取adc读数
  uint16_t a1;
  uint16_t sum=0;
  uint16_t max=0,min=0xffff;
  
  for(uint8_t i=0;i<5;i++){//连续读5次去掉最高值和最低值然后取平均
    a1=analogRead(pin);
    if(max < a1) max = a1;
    if(min > a1) min = a1;
    sum=sum+a1;
  }
  float avg = (sum-max-min)/3;
  return avg;//返回adc的数
}
void collect_data(){//中断后执行的函数。收集数据
  stat = 1;//更改stat给loop循环做判断
  float a1,a0;
  
  uint8_t gpio;
  
  for(gpio=2;gpio<=9;gpio++){//轮询开始 2~9
    pinMode(gpio , OUTPUT);
    digitalWrite(gpio ,HIGH);
    //14->A0   15->A1
    //更新全局变量adc
    adc[gpio-2][0] = get_adc(14);
    adc[gpio-2][1] = get_adc(15);


    digitalWrite(gpio ,LOW);
    pinMode(gpio , INPUT);
  }
}

//float get_press(int avg){
//  float volt;
//  volt = avg * (1.1/1023);
//  float pressure;
//  pressure = (volt*10000000)/(15000*(5-volt));
//  return pressure;
//}

void send_data(){//串口发送数据
  stat=0;
  String prin = "";
  for(int i=0;i<8;i++){
//    Serial.print(i);
//    Serial.print("0");
//    
//    Serial.print(adc[i][0]);
//    Serial.print(" ");
//    
//    Serial.print(i);
//    
//    Serial.print("1");
//    Serial.print(adc[i][1]);
//    Serial.print(" ");

    
    prin = "";
    if(adc[i][0]%1000  ==adc[i][0])prin+="0";
    if(adc[i][0]%100   ==adc[i][0])prin+="0";
    if(adc[i][0]%10    ==adc[i][0])prin+="0";
    prin+=adc[i][0];
    Serial.print(prin);
        
    prin = "";
    if(adc[i][1]%1000  ==adc[i][1])prin+="0";
    if(adc[i][1]%100   ==adc[i][1])prin+="0";
    if(adc[i][1]%10    ==adc[i][1])prin+="0";
    prin+=adc[i][1];
    Serial.print(prin);
    
  }
  Serial.println("");
}
void loop() {
  if(stat==1){//stat由中断函数collect_data来改变
    stat=0;
    send_data();
  }
}
