/*5x9测试程序*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>


//RX-io20
//TX-io21
Ticker ticker_getadc;
WiFiClient client;
WebServer server(80);
bool conn = false;


uint16_t gpio[10][6];
uint8_t io[] = {19, 18, 9, 10, 6, 7, 8, 20, 21};
uint8_t iocount = sizeof(io);
const char* ssid     = "Pixel_8008";
const char* password = "lkjh123123";
const int httpPort = 8802;
//8801 131 left
//8802 103 right
const char* host = "192.168.43.29"; // 网络服务器地址


int setupserver() {

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //  Serial.println("");

  // Wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //      Serial.println("Start updating " + type);
  })

  ArduinoOTA.begin();

  //  Serial.println("Ready");
  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.localIP());


  if (!client.connect(host, httpPort)) { //连接失败
    //    Serial.println("connection failed");
    //    Serial.println(WiFi.localIP());
    delay(5000);
    return 1;
  }
  client.print("9999");
  //  Serial.print("Connecting to");
  //  Serial.println(host);
  return 0;

}


void get_adc() {
  for (uint8_t pin = 0; pin < iocount; pin++) {
    pinMode(io[pin], OUTPUT);
    digitalWrite(io[pin], HIGH);
    for (uint8_t i = 0; i < 5; i++) { //5个adc
      uint16_t Max = 0, Min = 0xffff, sum = 0;
      for (uint8_t i0 = 0; i0 < 3; i0++) { //测5次取平均
        uint16_t temp = analogRead(i);
        sum = sum + temp;
        if (Max < temp)Max = temp;
        if (Min > temp)Min = temp;
      }
      gpio[pin][i] = sum - Max - Min;
    }
    digitalWrite(io[pin], LOW);
    pinMode(io[pin], INPUT);
  }
}
void sampling() {
  client.println();
  get_adc();
  for (int i = 0; i < iocount; i++) {
    client.println();
    for (int i0 = 0; i0 < 5; i0++) {
      client.print(i);
      client.print(",");
      client.print(gpio[i][i0]);
      if (i0 < 4)client.print(",");
    }
  }
  client.print(count++);
}
void setup() {

  analogSetAttenuation(ADC_2_5db);
  pinMode(5, OUTPUT); digitalWrite(5, HIGH); //for OTA

  int stat = setupserver();
  if (stat == 1)return; else conn = true;

  pinMode(19, INPUT); pinMode(18, INPUT);

  pinMode(9, INPUT); pinMode(10, INPUT);
  pinMode(20, INPUT); pinMode(21, INPUT);
  pinMode(8, INPUT); pinMode(6, INPUT); pinMode(7, INPUT);
  //  pinMode(CS,OUTPUT);pinMode(Y0,OUTPUT);digitalWrite(CS,HIGH);digitalWrite(Y0,LOW);
  //if(conn==true)ticker_getadc.attach_ms(1000/25,sampling);//125Hzsampling rate
}

bool ticker1 = false;
bool ticker2 = false;
int times = 1;
int count=0;
void loop() {
  if (millis() < 15000) ArduinoOTA.handle();
  else {
    ticker1 = true;
  }
  if (ticker1 == true && ticker2 != true) {
    //  if (ticker1 == true ) {
    ticker_getadc.attach_ms(1000 / 100, sampling);
    ticker2 = true;
    //    if (times % 100 == 0) {
    //      client.println("100times: ");
    //      client.print(millis());
    //      client.println("");
    //    }
  }
}
