void setup() {
  // put your setup code here, to run once:

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
