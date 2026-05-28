/*
  T5AI-Core Phase 1 CCT extreme channel test.

  Purpose:
  - Verify the two CCT channels are independently controlled through MOSFET CH1/CH2.
  - Alternate between pure cold and pure warm every 4 seconds.
  - Use 200 Hz to stay below the YYNMOS/NMOS prototype module's 500 Hz limit.

  Expected wiring:
  - T5 P24 -> MOSFET PWM1 -> WW warm channel negative
  - T5 P25 -> MOSFET PWM2 -> CW cold channel negative
  - T5 GND -> MOSFET GND1/GND2
  - LED common positive red wire -> 12V+
  - LED WW yellow wire -> MOSFET OUT1-
  - LED CW white wire -> MOSFET OUT2-
*/

const int WW_PIN = 24; // P24, expected warm white channel
const int CW_PIN = 25; // P25, expected cold white channel

const unsigned long PWM_PERIOD_US = 5000; // 200 Hz
const unsigned long STEP_MS = 4000;

struct CctStep {
  const char *name;
  int wwDuty;
  int cwDuty;
};

CctStep steps[] = {
  {"PURE_COLD", 0, 100},
  {"PURE_WARM", 100, 0},
};

const int STEP_COUNT = sizeof(steps) / sizeof(steps[0]);
int currentStep = 0;
unsigned long lastStepMs = 0;

void printStep()
{
  Serial.print("CCT step=");
  Serial.print(steps[currentStep].name);
  Serial.print(", WW P24 duty=");
  Serial.print(steps[currentStep].wwDuty);
  Serial.print("%, CW P25 duty=");
  Serial.print(steps[currentStep].cwDuty);
  Serial.println("%, freq=200Hz");
}

void onePwmCycle(int wwDuty, int cwDuty)
{
  unsigned long start = micros();
  unsigned long wwHigh = (PWM_PERIOD_US * wwDuty) / 100;
  unsigned long cwHigh = (PWM_PERIOD_US * cwDuty) / 100;
  bool wwHighNow = wwDuty > 0;
  bool cwHighNow = cwDuty > 0;

  digitalWrite(WW_PIN, wwHighNow ? HIGH : LOW);
  digitalWrite(CW_PIN, cwHighNow ? HIGH : LOW);

  while ((micros() - start) < PWM_PERIOD_US) {
    unsigned long elapsed = micros() - start;

    if (wwHighNow && elapsed >= wwHigh) {
      digitalWrite(WW_PIN, LOW);
      wwHighNow = false;
    }

    if (cwHighNow && elapsed >= cwHigh) {
      digitalWrite(CW_PIN, LOW);
      cwHighNow = false;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(WW_PIN, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  digitalWrite(WW_PIN, LOW);
  digitalWrite(CW_PIN, LOW);
  delay(1000);
  Serial.println("T5AI-Core CCT extreme channel test start");
  printStep();
}

void loop()
{
  if (millis() - lastStepMs >= STEP_MS) {
    lastStepMs = millis();
    currentStep = (currentStep + 1) % STEP_COUNT;
    printStep();
  }

  onePwmCycle(steps[currentStep].wwDuty, steps[currentStep].cwDuty);
}
