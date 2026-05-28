/*
  T5AI-Core Phase 1 PWM smoke test.

  Purpose:
  - Generate two 3.3V PWM-like GPIO signals before connecting MOSFET/LED loads.
  - Use 200 Hz so the YYNMOS/NMOS prototype module's 500 Hz limit is respected.
  - Duty values change every 3 seconds for logic analyzer verification.

  Wiring for logic analyzer only:
  - LA GND -> T5 GND
  - LA CH0 -> T5 P24
  - LA CH1 -> T5 P25
*/

const int PWM_A_PIN = 24;   // T5 pin P24, later can map to one LED channel
const int PWM_B_PIN = 25;   // T5 pin P25, later can map to another LED channel

const unsigned long PWM_PERIOD_US = 5000; // 200 Hz
const unsigned long STEP_MS = 3000;

struct DutyStep {
  int dutyA;
  int dutyB;
};

DutyStep steps[] = {
  {10, 90},
  {50, 50},
  {90, 10},
  {0, 100},
  {100, 0},
};

const int STEP_COUNT = sizeof(steps) / sizeof(steps[0]);
int currentStep = 0;
unsigned long lastStepMs = 0;

void printStep()
{
  Serial.print("PWM_A P24 duty=");
  Serial.print(steps[currentStep].dutyA);
  Serial.print("%, PWM_B P25 duty=");
  Serial.print(steps[currentStep].dutyB);
  Serial.println("%, freq=200Hz");
}

void onePwmCycle(int dutyA, int dutyB)
{
  unsigned long start = micros();
  unsigned long highA = (PWM_PERIOD_US * dutyA) / 100;
  unsigned long highB = (PWM_PERIOD_US * dutyB) / 100;
  bool aHigh = dutyA > 0;
  bool bHigh = dutyB > 0;

  digitalWrite(PWM_A_PIN, aHigh ? HIGH : LOW);
  digitalWrite(PWM_B_PIN, bHigh ? HIGH : LOW);

  while ((micros() - start) < PWM_PERIOD_US) {
    unsigned long elapsed = micros() - start;

    if (aHigh && elapsed >= highA) {
      digitalWrite(PWM_A_PIN, LOW);
      aHigh = false;
    }

    if (bHigh && elapsed >= highB) {
      digitalWrite(PWM_B_PIN, LOW);
      bHigh = false;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);
  digitalWrite(PWM_A_PIN, LOW);
  digitalWrite(PWM_B_PIN, LOW);
  delay(1000);
  Serial.println("T5AI-Core PWM smoke test start");
  printStep();
}

void loop()
{
  if (millis() - lastStepMs >= STEP_MS) {
    lastStepMs = millis();
    currentStep = (currentStep + 1) % STEP_COUNT;
    printStep();
  }

  onePwmCycle(steps[currentStep].dutyA, steps[currentStep].dutyB);
}

