/**
 * @file main.ino
 * @brief Demonstracija rada s višestrukim prekidima i prioritetima
 *
 * Sustav implementira:
 * - tri tipkala s različitim prioritetima (HIGH, MID, LOW)
 * - periodički timer prekid
 * - senzor (digitalni ulaz)
 *
 * Prekidi postavljaju zastavice koje se obrađuju u glavnoj petlji.
 * Prioriteti se implementiraju softverski u funkciji handleInterrupts().
 *
 * @author 
 * @date 2026
 */

/** @name LED pinovi */
//@{
#define LED_CHECK0 4
#define LED_CHECK1 5
#define LED_CHECK2 6
#define LED_CHECK3 7
//@}

/** @name Ulazni pinovi */
//@{
#define BUTTON_INPUT0 12 ///< Tipkalo - visoki prioritet
#define BUTTON_INPUT1 13 ///< Tipkalo - srednji prioritet
#define BUTTON_INPUT2 14 ///< Tipkalo - niski prioritet
#define SENSOR_INPUT 15  ///< PIR senzor
//@}

/**
 * @brief Zastavice za obradu prekida
 *
 * Svaki indeks predstavlja jedan izvor prekida:
 * 0 - HIGH, 1 - MID, 2 - LOW
 */
volatile bool actionFlag[3] = {false, false, false};

/**
 * @brief Vrijeme zadnjeg prekida za debounce
 */
volatile unsigned long lastInterruptTime[3] = {0, 0, 0};

/// Zadnje vrijeme aktivacije timera
volatile unsigned long lastTimerTime = 0;

/// Interval blinkanja LED-a (ms)
const unsigned long BLINK_INTERVAL = 100;

/// Debounce vrijeme za tipkala (ms)
const int DEBOUNCE_DELAY = 50;

/// Period timera (ms)
const int TIMER_DELAY = 1000;

/// Trajanje LED indikacije (ms)
const int LED_DURATION = 1000;

/**
 * @brief Oznaka da je obrada prekida u tijeku
 *
 * Koristi se za sprječavanje konflikata između ISR i glavne petlje.
 */
volatile bool interruptInProgress = false; 

/** 
 * Timer handler i zastavica za označavanje da se obraduje timer prekid
 */
hw_timer_t *timer = NULL;
volatile bool timerFlag = false;

/**
 * @brief Timer ISR
 *
 * Aktivira se periodički i postavlja zastavicu za obradu senzora.
 */
void IRAM_ATTR onTimer() {
  timerFlag = true; 
}

/**
 * @brief Inicijalizacija sustava
 *
 * Postavlja:
 * - pinove
 * - tipkala
 * - prekide za tipkala
 * - timer prekid
 * - serijsku komunikaciju
 */
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

/**
 * @brief Glavna petlja programa
 *
 * Redoslijed:
 * 1. Obrada timer događaja (senzor) ukoliko je zastavica postavljena
 * 2. Obrada prekida prema prioritetu
 * 3. Idle stanje (blink LED)
 */
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

/**
 * @brief Obrada podataka senzora
 *
 * Poziva se nakon timer prekida.
 */
void handleSensorUpdate() {
  int result = digitalRead(SENSOR_INPUT);
  if (result) {
    Serial.println("Motion");
  }
  timerFlag = false;
}

/**
 * @brief Obrada prekida prema prioritetu
 *
 * Prioritet:
 * 1. HIGH (index 0)
 * 2. MID (index 1)
 * 3. LOW (index 2)
 *
 * Koristi else-if kako bi se osiguralo izvršavanje samo jednog prekida.
 */
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

/**
 * @brief ISR za tipkalo visokog prioriteta
 */
void HIGH_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(0, "HIGH Priority");
  }
}

/**
 * @brief ISR za tipkalo srednjeg prioriteta
 */
void MID_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(1, "MID Priority");
  }
}

/**
 * @brief ISR za tipkalo niskog prioriteta
 */
void LOW_INTERRUPT() {
  if (!interruptInProgress) {
    handleInterrupt(2, "LOW Priority");
  }
}

/**
 * @brief Blinkanje LED-a
 *
 * @param ledPin Pin LED-a
 *
 * Funkcija za treperenje LED svjetla.
 */
void blinkLed(int ledPin) {
  unsigned long startTime = millis();
  while (millis() - startTime < LED_DURATION) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(BLINK_INTERVAL);
  }
  digitalWrite(ledPin, LOW);
}

/**
 * @brief Obrada prekida (zajednička funkcija)
 *
 * @param index Indeks prekida (0-2)
 * @param message Poruka za serijski izlaz
 *
 * Implementira:
 * - debounce zaštitu
 * - postavljanje zastavice
 */
void handleInterrupt(int index, const char* message) {
  if (millis() - lastInterruptTime[index] < DEBOUNCE_DELAY) {
    return;
    }
  lastInterruptTime[index] = millis();
  actionFlag[index] = true;
  Serial.println(message);
}