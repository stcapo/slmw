/*
  T5AI-Core Phase 1 Tuya App CCT control test.

  Goal:
  - Bind the T5AI-Core to Tuya App through TuyaIoT.
  - Receive switch / brightness / color-temperature DP commands.
  - Map DP state to P24/P25 hardware PWM for the YYNMOS-4 MOSFET module.

  Wiring:
  - T5 P24 -> MOSFET PWM1 -> WW warm channel negative
  - T5 P25 -> MOSFET PWM2 -> CW cold channel negative
  - T5 GND -> MOSFET GND1/GND2
  - LED common positive red wire -> 12V+
  - LED WW yellow wire -> MOSFET OUT1-
  - LED CW white wire -> MOSFET OUT2-
*/

#include "TuyaIoT.h"
#include "Log.h"
#include "secrets.h"

extern "C" {
#include "tkl_pwm.h"
}

const int WW_PIN = 25;
const int CW_PIN = 24;

const unsigned long PWM_FREQ_HZ = 200; // Within current prototype MOSFET module limit.
const unsigned long PWM_CYCLE = 10000; // Tuya PWM duty scale: duty / cycle.

// Common Tuya light DPs. Keep alternate IDs for early platform-model uncertainty.
#define DPID_SWITCH          20
#define DPID_BRIGHT          22
#define DPID_TEMP            23
#define DPID_SWITCH_ALT      1
#define DPID_BRIGHT_ALT      2
#define DPID_TEMP_ALT        3
#define DPID_TEMP_ALT2       24

#define BUTTON_PIN           BUTTON_BUILTIN
#define BUTTON_PRESS_LEVEL   LOW
#define BUTTON_DEBOUNCE_MS   (50u)
#define BUTTON_LONG_MS       (3 * 1000u)

bool powerOn = false;
int brightnessPercent = 100;
int colorTempPercent = 50; // 0 = cold, 100 = warm in this test firmware.
int wwDuty = 0;
int cwDuty = 0;
TUYA_PWM_NUM_E wwPwmNum = TUYA_PWM_NUM_MAX;
TUYA_PWM_NUM_E cwPwmNum = TUYA_PWM_NUM_MAX;
bool pwmReady = false;
bool digitalOverrideActive = false;

void tuyaIoTEventCallback(tuya_event_msg_t *event);
void buttonCheck(void);
void serialCommandCheck(void);
void processSerialCommand(char *cmd);
void printSerialHelp(void);
void printCurrentState(void);
void setDirectPwm(int wwPercent, int cwPercent);
void setDigitalOverride(int pin, bool high);
void clearDigitalOverride(void);
void printDpDebugValue(tuya_event_msg_t *event, uint8_t dpid);

bool isSwitchDp(uint8_t dpid)
{
  return dpid == DPID_SWITCH || dpid == DPID_SWITCH_ALT;
}

bool isBrightDp(uint8_t dpid)
{
  return dpid == DPID_BRIGHT || dpid == DPID_BRIGHT_ALT;
}

bool isTempDp(uint8_t dpid)
{
  return dpid == DPID_TEMP || dpid == DPID_TEMP_ALT || dpid == DPID_TEMP_ALT2;
}

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

int dpValueToPercent(int value)
{
  if (value <= 100) {
    return clampPercent(value);
  }

  // Many Tuya lighting panels use 0-1000 or 10-1000 for brightness/CCT values.
  if (value <= 1000) {
    return clampPercent((value + 5) / 10);
  }

  return 100;
}

uint32_t percentToPwmDuty(int percent)
{
  return (uint32_t)clampPercent(percent) * 100;
}

bool initPwmChannel(int pin, TUYA_PWM_NUM_E &pwmNum)
{
  pwmNum = pwmPinToNum(pin);
  if (pwmNum == TUYA_PWM_NUM_MAX) {
    Serial.print("PWM pin not supported: P");
    Serial.println(pin);
    return false;
  }

  TUYA_PWM_BASE_CFG_T pwmCfg = pwmCfgGet(pin);
  pwmCfg.polarity = TUYA_PWM_POSITIVE;
  pwmCfg.count_mode = TUYA_PWM_CNT_UP;
  pwmCfg.frequency = PWM_FREQ_HZ;
  pwmCfg.cycle = PWM_CYCLE;
  pwmCfg.duty = 0;

  if (tkl_pwm_init(pwmNum, &pwmCfg) != OPRT_OK) {
    Serial.print("PWM init failed: P");
    Serial.println(pin);
    return false;
  }

  if (tkl_pwm_start(pwmNum) != OPRT_OK) {
    Serial.print("PWM start failed: P");
    Serial.println(pin);
    return false;
  }

  return true;
}

void initPwmOutputs()
{
  bool wwOk = initPwmChannel(WW_PIN, wwPwmNum);
  bool cwOk = initPwmChannel(CW_PIN, cwPwmNum);
  pwmReady = wwOk && cwOk;

  Serial.print("PWM init ");
  Serial.println(pwmReady ? "ok" : "failed");
}

void updatePwmOutputs()
{
  if (!pwmReady || digitalOverrideActive) {
    return;
  }

  tkl_pwm_duty_set(wwPwmNum, percentToPwmDuty(wwDuty));
  tkl_pwm_duty_set(cwPwmNum, percentToPwmDuty(cwDuty));
}

void clearDigitalOverride()
{
  if (!digitalOverrideActive) {
    return;
  }

  digitalOverrideActive = false;
  initPwmOutputs();
  updatePwmOutputs();
  Serial.println("Digital override cleared, PWM restored.");
}

void setDigitalOverride(int pin, bool high)
{
  digitalOverrideActive = true;

  if (pwmReady) {
    tkl_pwm_stop(wwPwmNum);
    tkl_pwm_stop(cwPwmNum);
  }

  pinMode(WW_PIN, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  digitalWrite(WW_PIN, LOW);
  digitalWrite(CW_PIN, LOW);
  digitalWrite(pin, high ? HIGH : LOW);

  Serial.print("Digital override: P");
  Serial.print(pin);
  Serial.println(high ? "=HIGH" : "=LOW");
}

void setDirectPwm(int wwPercent, int cwPercent)
{
  clearDigitalOverride();
  powerOn = true;
  wwDuty = clampPercent(wwPercent);
  cwDuty = clampPercent(cwPercent);

  Serial.print("Direct PWM: WW=");
  Serial.print(wwDuty);
  Serial.print("%, CW=");
  Serial.print(cwDuty);
  Serial.println("%");

  updatePwmOutputs();
}

void printCurrentState()
{
  Serial.print("State: power=");
  Serial.print(powerOn ? "on" : "off");
  Serial.print(", bright=");
  Serial.print(brightnessPercent);
  Serial.print("%, cct=");
  Serial.print(colorTempPercent);
  Serial.print("% warm, WW=");
  Serial.print(wwDuty);
  Serial.print("%, CW=");
  Serial.print(cwDuty);
  Serial.println("%");
}

void printSerialHelp()
{
  Serial.println("Serial commands:");
  Serial.println("  help             - show this help");
  Serial.println("  state            - print current light state");
  Serial.println("  on               - local power on");
  Serial.println("  off              - local power off");
  Serial.println("  b <0-1000>       - set local brightness raw value");
  Serial.println("  c <0-1000>       - set local CCT raw value, 0=cold, 1000=warm");
  Serial.println("  set <p> <b> <c>  - set power(0/1), brightness raw, CCT raw");
  Serial.println("  ww <0-100>       - direct PWM test: P24 warm only");
  Serial.println("  cw <0-100>       - direct PWM test: P25 cold only");
  Serial.println("  mix <ww> <cw>    - direct PWM test: set P24/P25 duty percent");
  Serial.println("  p24hi / p24lo    - force P24 digital output high/low, PWM disabled");
  Serial.println("  p25hi / p25lo    - force P25 digital output high/low, PWM disabled");
  Serial.println("  pwm              - restore PWM after digital override");
}

void processSerialCommand(char *cmd)
{
  while (*cmd == ' ' || *cmd == '\t') {
    cmd++;
  }

  if (*cmd == '\0') {
    return;
  }

  int valueA = 0;
  int valueB = 0;
  int valueC = 0;

  if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
    printSerialHelp();
  } else if (strcmp(cmd, "state") == 0) {
    printCurrentState();
  } else if (strcmp(cmd, "on") == 0) {
    powerOn = true;
    applyLightState();
  } else if (strcmp(cmd, "off") == 0) {
    powerOn = false;
    applyLightState();
  } else if (sscanf(cmd, "b %d", &valueA) == 1 || sscanf(cmd, "bright %d", &valueA) == 1) {
    brightnessPercent = dpValueToPercent(valueA);
    Serial.print("Local brightness raw=");
    Serial.print(valueA);
    Serial.print(", percent=");
    Serial.println(brightnessPercent);
    applyLightState();
  } else if (sscanf(cmd, "c %d", &valueA) == 1 || sscanf(cmd, "cct %d", &valueA) == 1) {
    colorTempPercent = dpValueToPercent(valueA);
    Serial.print("Local CCT raw=");
    Serial.print(valueA);
    Serial.print(", warm percent=");
    Serial.println(colorTempPercent);
    applyLightState();
  } else if (sscanf(cmd, "set %d %d %d", &valueA, &valueB, &valueC) == 3) {
    powerOn = valueA != 0;
    brightnessPercent = dpValueToPercent(valueB);
    colorTempPercent = dpValueToPercent(valueC);
    applyLightState();
  } else if (sscanf(cmd, "ww %d", &valueA) == 1) {
    setDirectPwm(valueA, 0);
  } else if (sscanf(cmd, "cw %d", &valueA) == 1) {
    setDirectPwm(0, valueA);
  } else if (sscanf(cmd, "mix %d %d", &valueA, &valueB) == 2) {
    setDirectPwm(valueA, valueB);
  } else if (strcmp(cmd, "p24hi") == 0) {
    setDigitalOverride(WW_PIN, true);
  } else if (strcmp(cmd, "p24lo") == 0) {
    setDigitalOverride(WW_PIN, false);
  } else if (strcmp(cmd, "p25hi") == 0) {
    setDigitalOverride(CW_PIN, true);
  } else if (strcmp(cmd, "p25lo") == 0) {
    setDigitalOverride(CW_PIN, false);
  } else if (strcmp(cmd, "pwm") == 0) {
    clearDigitalOverride();
  } else {
    Serial.print("Unknown serial command: ");
    Serial.println(cmd);
    printSerialHelp();
  }
}

void serialCommandCheck()
{
  static char buffer[80];
  static uint8_t index = 0;

  while (Serial.available() > 0) {
    char ch = (char)Serial.read();

    if (ch == '\r' || ch == '\n') {
      buffer[index] = '\0';
      processSerialCommand(buffer);
      index = 0;
    } else if (index < (sizeof(buffer) - 1)) {
      buffer[index++] = ch;
    }
  }
}

void printDpDebugValue(tuya_event_msg_t *event, uint8_t dpid)
{
  dp_prop_tp_t dpType;
  dp_value_t dpValue;

  if (TuyaIoT.objRead(event, dpid, dpType, dpValue) != OPRT_OK) {
    Serial.println("DP debug read failed");
    return;
  }

  Serial.print("DP debug type=");
  Serial.print((int)dpType);
  Serial.print(", value=");

  switch (dpType) {
    case PROP_BOOL:
      Serial.println(dpValue.dp_bool ? "true" : "false");
      break;
    case PROP_VALUE:
      Serial.println(dpValue.dp_value);
      break;
    case PROP_ENUM:
      Serial.println(dpValue.dp_enum);
      break;
    case PROP_BITMAP:
      Serial.println(dpValue.dp_bitmap);
      break;
    case PROP_STR:
      Serial.println(dpValue.dp_str);
      break;
    default:
      Serial.println("<unsupported>");
      break;
  }
}

void applyLightState()
{
  if (!powerOn || brightnessPercent <= 0) {
    wwDuty = 0;
    cwDuty = 0;
  } else {
    wwDuty = (brightnessPercent * colorTempPercent) / 100;
    cwDuty = brightnessPercent - wwDuty;
  }

  Serial.print("Apply light: power=");
  Serial.print(powerOn ? "on" : "off");
  Serial.print(", bright=");
  Serial.print(brightnessPercent);
  Serial.print("%, cct=");
  Serial.print(colorTempPercent);
  Serial.print("% warm, WW=");
  Serial.print(wwDuty);
  Serial.print("%, CW=");
  Serial.print(cwDuty);
  Serial.println("%");

  updatePwmOutputs();
}

void setup()
{
  Serial.begin(115200);
  Log.begin();

  pinMode(WW_PIN, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  digitalWrite(WW_PIN, LOW);
  digitalWrite(CW_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("T5AI-Core Tuya CCT app test start");
  initPwmOutputs();
  applyLightState();

  TuyaIoT.setEventCallback(tuyaIoTEventCallback);

  tuya_iot_license_t license;
  int rt = TuyaIoT.readBoardLicense(&license);
  if (OPRT_OK != rt) {
    license.uuid = (char *)TUYA_DEVICE_UUID;
    license.authkey = (char *)TUYA_DEVICE_AUTHKEY;
    Serial.println("Using license from sketch constants.");
  } else {
    Serial.println("Using board license.");
  }

  Serial.print("uuid: ");
  Serial.println(license.uuid);
  Serial.println("authkey: <hidden>");

  TuyaIoT.setLicense(license.uuid, license.authkey);
  TuyaIoT.begin(APP_PRODUCT_ID, PROJECT_VERSION);
}

void loop()
{
  serialCommandCheck();
  buttonCheck();
  delay(10);
}

void tuyaIoTEventCallback(tuya_event_msg_t *event)
{
  tuya_event_id_t eventId = TuyaIoT.eventGetId(event);

  switch (eventId) {
    case TUYA_EVENT_BIND_START: {
      Serial.println("TUYA_EVENT_BIND_START");
    } break;

    case TUYA_EVENT_ACTIVATE_SUCCESSED: {
      Serial.println("TUYA_EVENT_ACTIVATE_SUCCESSED");
    } break;

    case TUYA_EVENT_MQTT_CONNECTED: {
      Serial.println("TUYA_EVENT_MQTT_CONNECTED");
    } break;

    case TUYA_EVENT_TIMESTAMP_SYNC: {
      tal_time_set_posix(event->value.asInteger, 1);
      Serial.println("TUYA_EVENT_TIMESTAMP_SYNC");
    } break;

    case TUYA_EVENT_DP_RECEIVE_OBJ: {
      uint16_t dpNum = TuyaIoT.eventGetDpNum(event);
      Serial.print("TUYA_EVENT_DP_RECEIVE_OBJ count=");
      Serial.println(dpNum);

      for (uint16_t i = 0; i < dpNum; i++) {
        uint8_t dpid = TuyaIoT.eventGetDpId(event, i);
        Serial.print("Receive DP id=");
        Serial.println(dpid);

        if (isSwitchDp(dpid)) {
          bool value = false;
          int rt = TuyaIoT.read(event, dpid, value);
          if (rt == OPRT_OK) {
            powerOn = value;
            Serial.print("Switch DP=");
            Serial.println(powerOn ? "on" : "off");
            applyLightState();
            TuyaIoT.write(dpid, powerOn);
          } else {
            Serial.print("Switch DP read failed rt=");
            Serial.println(rt);
            printDpDebugValue(event, dpid);
          }
        } else if (isBrightDp(dpid)) {
          int value = 0;
          int rt = TuyaIoT.read(event, dpid, value);
          if (rt == OPRT_OK) {
            brightnessPercent = dpValueToPercent(value);
            Serial.print("Brightness raw=");
            Serial.print(value);
            Serial.print(", percent=");
            Serial.println(brightnessPercent);
            applyLightState();
            TuyaIoT.write(dpid, value);
          } else {
            Serial.print("Brightness DP read failed rt=");
            Serial.println(rt);
            printDpDebugValue(event, dpid);
          }
        } else if (isTempDp(dpid)) {
          int value = 0;
          int rt = TuyaIoT.read(event, dpid, value);
          if (rt == OPRT_OK) {
            colorTempPercent = dpValueToPercent(value);
            Serial.print("CCT raw=");
            Serial.print(value);
            Serial.print(", warm percent=");
            Serial.println(colorTempPercent);
            applyLightState();
            TuyaIoT.write(dpid, value);
          } else {
            Serial.print("CCT DP read failed rt=");
            Serial.println(rt);
            printDpDebugValue(event, dpid);
          }
        } else {
          Serial.print("Unhandled DP id=");
          Serial.println(dpid);
          printDpDebugValue(event, dpid);
        }
      }
    } break;

    case TUYA_EVENT_DP_RECEIVE_RAW: {
      Serial.println("TUYA_EVENT_DP_RECEIVE_RAW");
    } break;

    default:
      break;
  }
}

void buttonClick()
{
  powerOn = !powerOn;
  Serial.print("Button toggled power=");
  Serial.println(powerOn ? "on" : "off");
  applyLightState();
  TuyaIoT.write(DPID_SWITCH, powerOn);
}

void buttonLongPressStart()
{
  Serial.println("Button long press, remove Tuya IoT device.");
  TuyaIoT.remove();
}

void buttonCheck(void)
{
  static uint32_t buttonPressMs = 0;
  static uint8_t isPress = 0;

  if (digitalRead(BUTTON_PIN) == BUTTON_PRESS_LEVEL) {
    if (isPress == 0) {
      buttonPressMs = millis();
      isPress = 1;
    }

    if ((isPress == 1) && ((millis() - buttonPressMs) > BUTTON_DEBOUNCE_MS)) {
      isPress = 2;
    }

    if ((isPress == 2) && ((millis() - buttonPressMs) >= BUTTON_LONG_MS)) {
      isPress = 3;
      buttonLongPressStart();
    }
  } else {
    if (isPress == 2) {
      if ((millis() - buttonPressMs) < BUTTON_LONG_MS) {
        buttonClick();
      } else {
        buttonLongPressStart();
      }
    }
    isPress = 0;
  }
}
