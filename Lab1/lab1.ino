
// Definiranje pinova za LED indikatore
#define LED_CHECK0 4    ///< Pin za LED indikator prekida INT0 (visoki prioritet)
#define LED_CHECK1 5
#define LED_CHECK2 6
#define LED_CHECK3 7

#define BUTTON_INPUT0 12
#define BUTTON_INPUT1 13
#define BUTTON_INPUT2 14

#define SENSOR_INPUT 15

volatile bool actionFlag[3] = {false, false, false};    ///< Zastavice koje signaliziraju aktivaciju prekida (INT0, INT1, INT2)
volatile unsigned long lastInterruptTime[3] = {0, 0, 0}; ///< Vremenski žigovi posljednjih prekida za debounce
volatile unsigned long lastTimerTime = 0;             ///< Vremenski žig posljednjeg prekida tajmera
const unsigned long BLINK_INTERVAL = 200;             ///< Interval treptanja LED indikatora (u milisekundama)
const int DEBOUNCE_DELAY = 50;                       ///< Vrijeme ignoriranja uzastopnih pritisaka tipke (u milisekundama)
const int TIMER_DELAY = 1000;                         ///< Periodičnost Timer1 prekida (u milisekundama)
const int ALARM_DISTANCE = 100;                       
const int LED_DURATION = 1000;

volatile bool distanceAlert = false;                 ///< Zastavica koja označava da je udaljenost ispod praga
volatile bool timerFlag = false;                     ///< Zastavica koja označava da je Timer1 prekid nastupio
volatile bool interruptInProgress = false;   

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

  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {

  handleInterrupts();

  if (!interruptInProgress) {
    blinkLed(LED_CHECK0);
    Serial.println("Ceka input");
  }

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
    handleInterrupt(0, "VISOKI prioritet aktiviran");
  }
}

void MID_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(1, "SREDNJI prioritet aktiviran");
  }
}

void LOW_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(2, "NISKI prioritet aktiviran");
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
  Serial.print(message);
}