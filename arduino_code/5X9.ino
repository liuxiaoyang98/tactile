/*5x9测试程序*/
/*
  socat tcp-listen:8801,fork,reuseaddr - |tee /tmp/2.log
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <Ticker.h>

Ticker ticker_getadc;
WiFiClient client;

//RX-gpio20
//TX-gpio21
uint8_t io[] = {21,20,19,18,9,10,8,7,6};
uint16_t gpio[sizeof(io)][5];
const char* ssid = "Pixel_8008";
const char* password = "lkjh123123";
uint32_t chipId;
bool ota_mode = true; //上电启动是ota模式10秒后休眠，从休眠唤醒是测试模式

const char* host = "192.168.43.29"; // 网络服务器地址
uint32_t ota_run = 0; //ota begin millis();
uint32_t getChipId() {
  //0x00006c944c4e76a0 = > a0:76:4e:4c:94:6c
  uint32_t i = ESP.getEfuseMac() >> 24;
  return (uint32_t) (i & 0xff) << 16
         | (i & 0xff00)
         | (i & 0xff0000) >> 16;
}

void setupserver() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  if (ota_mode) {
    ArduinoOTA
    .onStart([]() {
      if (ota_run < millis()) {
        ota_run = millis() + 15000;
        if (client.connected())
          client.println("\n开始ota");
      }
    });
    ArduinoOTA.begin();
  }
  client_link();
}

uint32_t volatile up_time; //volatile 是说让gcc不要进行变量优化， 这个值会被定时中断修改，gcc可能觉得会它在setup()中没有变化，而忽略这个变量
bool adc_ok = false;
uint16_t adc_count = 0;
#define TESTS 3 /*tests=20 is 39hz, tests=10 is 75hz, tests = 6 is 121hz*/
uint16_t check_v = 10;
void get_adc() {
  uint8_t count = 0;
  if (adc_ok) return; //stop adc, when the net_send is stoped
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  for (uint8_t pin = 0; pin < sizeof(io); pin++) {
    pinMode(io[pin], OUTPUT);
    digitalWrite(io[pin], HIGH); //当前扫描腿 设为输出高
    for (uint8_t i = 0; i < 5; i++) { //5个adc

      uint16_t Max = 0, Min = 0xffff;
      uint32_t sum = 0;
      for (uint8_t i0 = 0; i0 < TESTS; i0++) { //测n次取平均
        uint16_t temp = analogRead(i);
        sum += temp;
        if (Max < temp) Max = temp;
        if (Min > temp) Min = temp;
      }

      gpio[pin][i] = (sum - Max - Min) / (TESTS - 2); //每次测n次，去掉最大值， 再去掉最小值， 然后平均
      //gpio[pin][i] = analogRead(i);
      if (gpio[pin][i] > check_v)
        count++;
    }
    pinMode(io[pin], INPUT); //当前扫描腿用完后，设置回高阻状态
    digitalWrite(io[pin], LOW);
  }

  adc_count++;
  if (count > 3)
    up_time = millis() + 60000; //有压力大于30的点超过3个, 就开机60秒

  adc_ok = true; //数据ok ，可以发送了
}

String prt; //数据发送缓冲
void net_send() { //数据发送函数
  if (!adc_ok) return;
  if (!client_link()) //如果没有连接到服务器，就连一下，如果还是没有通， 就直接退出发送
    return;
  char buf[50];
  uint32_t ms = millis();
  uint8_t off_t = (up_time - ms) / 1000;
  uint16_t ms0 = ms % 1000;
  ms = ms / 1000;
  uint8_t s = ms % 60;
  ms = ms / 60;
  uint8_t m = ms % 60;
  ms = ms / 60;
  snprintf(buf, sizeof(buf), "\nESP-%06X %02d:%02d:%02d.%03d, %05d, %dhz, %d\n", chipId, ms, m, s, ms0, adc_count, adc_count / (millis() / 1000), off_t);
  prt += String(buf);
  for (int i = 0; i < sizeof(io); i++) {
    prt += i;
    for (int i0 = 0; i0 < 5; i0++) {
      prt += ",";
      prt += gpio[i][i0];
    }
    prt += "\n";
  }
  adc_ok = false;
  if (prt.length() > 1200) { //1200字节发送一个包
    client.print(prt);
    prt = "";
  }
}
bool client_link() {
  if (client.connected())
    return true;
  if (client.connect(host, httpPort)) { //连接成功
    client.printf("\nSN:%06x\n", chipId);
    if (!ota_mode)
      net_send();
  }
  return client.connected();
}

void poweroff() { //目前关机状态，功耗1毫安， 电池可以工作200小时， 应该还可以继续优化功耗。
  if (ota_mode || millis() > 10000) { // 从测量状态关机， 先断开wifi
    client.stop();
    WiFi.disconnect(true);
    digitalWrite(5, LOW); //关闭3.3V大功率电源
  }
  for (uint8_t i = 0; i < sizeof(io); i++) {
    pinMode(io[i], OUTPUT);
    digitalWrite(io[i], LOW);
  }

  for (uint8_t i = 0; i < 5; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  esp_deep_sleep(1000000L); //1秒后唤醒
}

void setup() {
  for (uint8_t i = 0; i < sizeof(io); i++) {
    pinMode(io[i], INPUT);
    digitalWrite(io[i], LOW); //扫描腿都设为高阻状态
  }
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  up_time = 0;
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) { //如果是timer唤醒， 就是低功耗状态下， 每秒一次的唤醒检查，
    get_adc(); //低功耗状态， 每秒一次检查压力是否超过30， 超过30的点有3个, up_time就会增加60秒
    if (up_time == 0) { //如果up_time没有变化， 说明压力值都不超过30， 继续休眠
      poweroff(); //关机， 1秒钟后唤醒重复测试
    }
    ota_mode = false; //测试模式
  }
  check_v = 30; //开wifi后， adc会有噪声，加大压力检查值到30
  up_time = 61000;
  prt.reserve(1600); //输出缓冲区，保留1600字节
  chipId = getChipId();
  switch (chipId) { //根据序列号， 选择不同的服务器端口
    case 0x6DE219:
      httpPort = 8802;
    default:
      httpPort = 8801;
  }
  analogSetAttenuation(ADC_11db); //输入电压衰减 ADC_0db/ADC_2_5db/ADC_6db/ADC_11db
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); //打开3.3V大功率电源
  setupserver();
  ticker_getadc.attach_ms(int(1000 / 50), get_adc); //50Hz采样
}

void loop() {
  if (ota_mode) {
    if (ota_run || millis() < 15000) //15秒之内可以启动ota下载
      ArduinoOTA.handle();
    if (!ota_run)
      net_send();
  } else if (adc_ok)  //如果数据准备好，就发送
    net_send();

  if (millis() > up_time) { //无压力超过60秒， 进入关机状态, 在关机状态1秒钟测一次压力，有压力时就恢复运行
    poweroff();
  }
}
