
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
const int httpPort=8801;//端口号
void setup(){
  Serial.begin(115200);

}
 void loop() {
  String data ="";
  while (Serial.available()>0)

  {

    data +=char(Serial.read());

  }
  Serial.print(data);
}
