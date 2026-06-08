const int LED_CTRL_PIN = 22;  // P22 通常就是 GPIO22

void setup() {
  pinMode(LED_CTRL_PIN, OUTPUT);
  digitalWrite(LED_CTRL_PIN, LOW);
}

void loop() {
  digitalWrite(LED_CTRL_PIN, HIGH);  // 通常是灯亮
  delay(1000);

  digitalWrite(LED_CTRL_PIN, LOW);   // 通常是灯灭
  delay(1000);
}