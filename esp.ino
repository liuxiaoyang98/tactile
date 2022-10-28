
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
ESP8266WiFiMulti wifiMulti;           // 建立ESP8266WiFiMulti对象
WiFiClient client;//tcp连接
const char* host = "192.168.43.29"; // 网络服务器地址
 
const char*  WiFi_SSID = "Pixel_8008";
const char*  WiFi_Password = "lkjh123123";
const int httpPort=8802;//端口号
void setup(){
  Serial.println(" ");
  Serial.begin(115200);
  
  wifiMulti.addAP(WiFi_SSID, WiFi_Password); 
  Serial.print("\r\n连接到AP.."); 


 
  // 尝试进行wifi连接。
  int i = 0;  
  while (wifiMulti.run() != WL_CONNECTED) { 
    delay(1000);
    Serial.print('.');
  }
  
  Serial.println("");
  Serial.print("已连接到 ");
  Serial.println(WiFi.SSID());
  Serial.print("IP地址:\t");
  Serial.println(WiFi.localIP());



  if(!client.connect(host,httpPort)){//连接失败
    Serial.println("connection failed");
    Serial.println(WiFi.localIP());
    delay(5000);
    return;
  }
  client.print("9999");
  Serial.print("Connecting to");
  Serial.println(host);
}
 void loop() {
  String data ="";
  while (Serial.available()>0)

  {

    data +=char(Serial.read());

  }
  Serial.print(data);
  client.print(data);
}
