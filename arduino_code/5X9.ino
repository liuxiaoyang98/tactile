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
int httpPort = 8802;
uint32_t chipId;

//8801 131 left
//8802 103 right
const char* host = "192.168.43.29"; // 网络服务器地址

uint32_t getChipId() {
  //0x00006c944c4e76a0 = > a0:76:4e:4c:94:6c
  uint32_t i = ESP.getEfuseMac() >> 24;
  return (uint32_t)  (i & 0xff) << 16
         | (i & 0xff00)
         | (i & 0xff0000) >> 16;
}



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
  });

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

int adc_count=0;
void get_adc() {
  for (uint8_t pin = 0; pin < iocount; pin++) {
    pinMode(io[pin], OUTPUT);
    digitalWrite(io[pin], HIGH);
    for (uint8_t i = 0; i < 5; i++) { //5个adc
      uint16_t Max = 0, Min = 0xffff, sum = 0;
      for (uint8_t i0 = 0; i0 < 3; i0++) { //测5次取平均
        uint16_t temp = analogRead(i);
        adc_count++;
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

int count = 0;
void sampling() {
  //get_adc();
  String prt;
  prt.reserve(2000);
  for (int i = 0; i < iocount; i++) {
    prt=prt+"\n";
//    client.println();
    for (int i0 = 0; i0 < 5; i0++) {
//      client.print(i);
//      client.print(",");
//      client.print(gpio[i][i0]);
//      if (i0 < 4)client.print(",");
      prt=prt+i;
      prt=prt+",";
      prt=prt+String(gpio[i][i0]);
      if (i0 < 4)prt=prt+",";
    }
  }
  prt=prt+"adc_counts :\n";
  prt=prt+String(adc_count)+"\n";
  prt=prt+"sampings :\n";
  prt=prt+String(count)+"\n";
  client.print(prt);
  count++;
}

void setup() {
  chipId = getChipId();
  switch (chipId) {
    case 0x6DE219:
      httpPort = 8802;
    default:
      httpPort = 8801;
  }
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
void loop() {
  if (millis() < 15000) ArduinoOTA.handle();
  else {
    ticker1 = true;
  }
  //  if (ticker1 == true && ticker2 != true) {
  //    ticker_getadc.attach_ms(12, sampling);
  //    ticker2 = true;
  if (ticker1 == true ) {
    if (times % 100 == 0) {
      String prt;
      prt.reserve(2000);
      prt+="100times: \n";
      prt+=String(millis() / 1000)+"s \n";
      client.print(prt);
    }
    sampling();
    times++;
  }
}
