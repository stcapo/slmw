/*
  T5AI-Core Phase 1 P24/P25 logic-analyzer PWM test.

  This is a minimal local test firmware:
  - No Tuya cloud/App/DP code.
  - No hardware PWM peripheral.
  - Software PWM only, so P24 and P25 are controlled independently.

  Current physical channel convention:
  - WW warm channel uses P25.
  - CW cold channel uses P24.

  Logic analyzer:
  - LA GND -> T5 GND
  - LA CH0 -> T5 P24
  - LA CH1 -> T5 P25
*/

const int P24_PIN = 24;
const int P25_PIN = 25;

const int WW_PIN = P25_PIN; // Keep the current swapped wiring.
const int CW_PIN = P24_PIN; // Keep the current swapped wiring.

const unsigned long PWM_PERIOD_US = 5000; // 200 Hz

int dutyP24 = 0;
int dutyP25 = 0;
bool forceDigital = false;
bool forceP24High = false;
bool forceP25High = false;

char serialBuffer[80];
uint8_t serialIndex = 0;

int clampPercent(int value)
{
  if (value < 0) {
    return 0;
  }
  if (value > 100) {
    return 100;
  }
  return value;
}

void writePinPwm(int pin, int duty, unsigned long phaseUs)
{
  duty = clampPercent(duty);

  if (duty <= 0) {
    digitalWrite(pin, LOW);
    return;
  }

  if (duty >= 100) {
    digitalWrite(pin, HIGH);
    return;
  }

  unsigned long highUs = (PWM_PERIOD_US * (unsigned long)duty) / 100;
  digitalWrite(pin, phaseUs < highUs ? HIGH : LOW);
}

void updateOutputs()
{
  if (forceDigital) {
    digitalWrite(P24_PIN, forceP24High ? HIGH : LOW);
    digitalWrite(P25_PIN, forceP25High ? HIGH : LOW);
    return;
  }

  unsigned long phaseUs = micros() % PWM_PERIOD_US;
  writePinPwm(P24_PIN, dutyP24, phaseUs);
  writePinPwm(P25_PIN, dutyP25, phaseUs);
}

void setPinPwm(int p24Duty, int p25Duty)
{
  forceDigital = false;
  dutyP24 = clampPercent(p24Duty);
  dutyP25 = clampPercent(p25Duty);

  Serial.print("Pin PWM: P24=");
  Serial.print(dutyP24);
  Serial.print("%, P25=");
  Serial.print(dutyP25);
  Serial.println("%, freq=200Hz");
}

void setWwCwPwm(int wwDuty, int cwDuty)
{
  wwDuty = clampPercent(wwDuty);
  cwDuty = clampPercent(cwDuty);

  // Current swapped wiring: WW=P25, CW=P24.
  setPinPwm(cwDuty, wwDuty);

  Serial.print("Channel map: WW(P25)=");
  Serial.print(wwDuty);
  Serial.print("%, CW(P24)=");
  Serial.print(cwDuty);
  Serial.println("%");
}

void setDigital(bool p24High, bool p25High)
{
  forceDigital = true;
  forceP24High = p24High;
  forceP25High = p25High;

  digitalWrite(P24_PIN, forceP24High ? HIGH : LOW);
  digitalWrite(P25_PIN, forceP25High ? HIGH : LOW);

  Serial.print("Digital: P24=");
  Serial.print(forceP24High ? "HIGH" : "LOW");
  Serial.print(", P25=");
  Serial.println(forceP25High ? "HIGH" : "LOW");
}

void printState()
{
  Serial.print("State: mode=");
  Serial.print(forceDigital ? "digital" : "pwm");
  Serial.print(", P24=");
  Serial.print(forceDigital ? (forceP24High ? 100 : 0) : dutyP24);
  Serial.print("%, P25=");
  Serial.print(forceDigital ? (forceP25High ? 100 : 0) : dutyP25);
  Serial.print("%, WW(P25)=");
  Serial.print(forceDigital ? (forceP25High ? 100 : 0) : dutyP25);
  Serial.print("%, CW(P24)=");
  Serial.print(forceDigital ? (forceP24High ? 100 : 0) : dutyP24);
  Serial.println("%");
}

void printHelp()
{
  Serial.println("Commands:");
  Serial.println("  help             show commands");
  Serial.println("  state            print current output state");
  Serial.println("  stop             P24=0%, P25=0%");
  Serial.println("  p24hi            P24=HIGH, P25=LOW");
  Serial.println("  p24lo            P24=LOW, P25=LOW");
  Serial.println("  p25hi            P24=LOW, P25=HIGH");
  Serial.println("  p25lo            P24=LOW, P25=LOW");
  Serial.println("  pwm              resume PWM with current duties");
  Serial.println("  mix <p24> <p25>  set pin PWM duties directly");
  Serial.println("  ww <0-100>       WW only: P25=duty, P24=0");
  Serial.println("  cw <0-100>       CW only: P24=duty, P25=0");
  Serial.println("  wwcw <ww> <cw>   channel PWM: WW=P25, CW=P24");
}

void processCommand(char *cmd)
{
  while (*cmd == ' ' || *cmd == '\t') {
    cmd++;
  }

  if (*cmd == '\0') {
    return;
  }

  int a = 0;
  int b = 0;

  if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
    printHelp();
  } else if (strcmp(cmd, "state") == 0) {
    printState();
  } else if (strcmp(cmd, "stop") == 0) {
    setPinPwm(0, 0);
  } else if (strcmp(cmd, "p24hi") == 0) {
    setDigital(true, false);
  } else if (strcmp(cmd, "p24lo") == 0) {
    setDigital(false, false);
  } else if (strcmp(cmd, "p25hi") == 0) {
    setDigital(false, true);
  } else if (strcmp(cmd, "p25lo") == 0) {
    setDigital(false, false);
  } else if (strcmp(cmd, "pwm") == 0) {
    forceDigital = false;
    setPinPwm(dutyP24, dutyP25);
  } else if (sscanf(cmd, "mix %d %d", &a, &b) == 2) {
    setPinPwm(a, b);
  } else if (sscanf(cmd, "wwcw %d %d", &a, &b) == 2) {
    setWwCwPwm(a, b);
  } else if (sscanf(cmd, "ww %d", &a) == 1) {
    setWwCwPwm(a, 0);
  } else if (sscanf(cmd, "cw %d", &a) == 1) {
    setWwCwPwm(0, a);
  } else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    printHelp();
  }
}

void readSerialCommands()
{
  while (Serial.available() > 0) {
    char ch = (char)Serial.read();

    if (ch == '\r' || ch == '\n') {
      serialBuffer[serialIndex] = '\0';
      processCommand(serialBuffer);
      serialIndex = 0;
    } else if (serialIndex < (sizeof(serialBuffer) - 1)) {
      serialBuffer[serialIndex++] = ch;
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(P24_PIN, OUTPUT);
  pinMode(P25_PIN, OUTPUT);
  digitalWrite(P24_PIN, LOW);
  digitalWrite(P25_PIN, LOW);

  delay(500);
  Serial.println("T5AI-Core P24/P25 minimal LA PWM test start");
  Serial.println("Mapping: LA CH0=P24, LA CH1=P25, WW=P25, CW=P24");
  printHelp();
  setPinPwm(0, 0);
}

void loop()
{
  readSerialCommands();
  updateOutputs();
}
