#define LED_CHECK0 4
#define LED_CHECK1 5
#define LED_CHECK2 6
#define LED_CHECK3 7

#define BUTTON_INPUT0 12
#define BUTTON_INPUT1 13
#define BUTTON_INPUT2 14

#define SENSOR_INPUT 15

volatile bool actionFlag[3] = {false, false, false};
volatile unsigned long lastInterruptTime[3] = {0, 0, 0};
volatile unsigned long lastTimerTime = 0;
const unsigned long BLINK_INTERVAL = 100;
const int DEBOUNCE_DELAY = 50;
const int TIMER_DELAY = 1000;
const int ALARM_DISTANCE = 100;
const int LED_DURATION = 1000;

volatile bool interruptInProgress = false;   

hw_timer_t *timer = NULL;
volatile bool timerFlag = false;

void IRAM_ATTR onTimer() {
  timerFlag = true; 
}

void setup() {
  pinMode(LED_CHECK0, OUTPUT);
  pinMode(LED_CHECK1, OUTPUT);
  pinMode(LED_CHECK2, OUTPUT);
  pinMode(LED_CHECK3, OUTPUT);

  pinMode(BUTTON_INPUT0, INPUT_PULLUP);
  pinMode(BUTTON_INPUT1, INPUT_PULLUP);
  pinMode(BUTTON_INPUT2, INPUT_PULLUP);

  pinMode(SENSOR_INPUT, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT0), HIGH_INTERRUPT, FALLING); 
  attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT1), MID_INTERRUPT, FALLING); 
  attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT2), LOW_INTERRUPT, FALLING); 

  timer = timerBegin(1000);

  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, TIMER_DELAY, true, 0);

  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {

  if (timerFlag) {
    handleSensorUpdate();
  }

  handleInterrupts();

  if (!interruptInProgress) {
    blinkLed(LED_CHECK0);
    Serial.println("Idle");
  }

}

void handleSensorUpdate() {
  int result = digitalRead(SENSOR_INPUT);
  if (result) {
    Serial.println("Motion");
  }
  timerFlag = false;
}

void handleInterrupts() {
  interruptInProgress = true;

  if (actionFlag[0]) {
    blinkLed(LED_CHECK1);
    actionFlag[0] = false;
  }
  else if (actionFlag[1]) {
    blinkLed(LED_CHECK2);
    actionFlag[1] = false;
  }
  else if (actionFlag[2]) {
    blinkLed(LED_CHECK3);
    actionFlag[2] = false;
  }

  interruptInProgress = false;
}

void HIGH_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(0, "HIGH Priority");
  }
}

void MID_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(1, "MID Priority");
  }
}

void LOW_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(2, "LOW Priority");
  }
}

void blinkLed(int ledPin) {
  unsigned long startTime = millis();
  while (millis() - startTime < LED_DURATION) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(BLINK_INTERVAL);
  }
  digitalWrite(ledPin, LOW);
}

void handleInterrupt(int index, const char* message) {
  if (millis() - lastInterruptTime[index] < DEBOUNCE_DELAY) {
    return;
    }
  lastInterruptTime[index] = millis();
  actionFlag[index] = true;
  Serial.println(message);
}