#include <Arduino.h>
#include "esp_sleep.h"
#include <DHTesp.h>
#include <WiFi.h>

//pinout
#define LED_PIN 2
#define DHT_PIN 15

#define TRIG_PIN 5
#define ECHO_PIN 17

//save interval and measurement info
#define WAKEUP_INTERVAL_SEC 2
#define MAX_MEASUREMENTS 10

DHTesp dht;

//sensor data arrays
RTC_DATA_ATTR float storedHumidity[MAX_MEASUREMENTS];
RTC_DATA_ATTR float storedTemperature[MAX_MEASUREMENTS];
RTC_DATA_ATTR float storedDistance[MAX_MEASUREMENTS];
RTC_DATA_ATTR int measurementCounter = 0;

struct SensorData {
  float humidity;
  float temperature;
  float distance;
};

void initSensors() {
  Serial.println("Initializing sensors...");
  dht.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  //calculating distance using speed of sound
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2.0;
  return distance;
}

SensorData readSensors() {
  SensorData data;
  //getting sensor data
  TempAndHumidity dhtData = dht.getTempAndHumidity();
  data.temperature = dhtData.temperature;
  data.humidity = dhtData.humidity;
  data.distance = readDistance();
  return data;
}

void saveMeasurement(SensorData data) {
  //storing new sensor data
  storedHumidity[measurementCounter] = data.humidity;
  storedTemperature[measurementCounter] = data.temperature;
  storedDistance[measurementCounter] = data.distance;
  measurementCounter++;
}

void printStoredData() {
  Serial.println();
  Serial.println(" STORED ENVIRONMENT DATA");

  for (int i = 0; i < MAX_MEASUREMENTS; i++) {
    Serial.print("Measurement ");
    Serial.print(i + 1);

    Serial.print(" | Temp: ");
    Serial.print(storedTemperature[i]);
    Serial.print(" C");

    Serial.print(" | Humidity: ");
    Serial.print(storedHumidity[i]);
    Serial.print(" %");

    Serial.print(" | Distance: ");
    Serial.print(storedDistance[i]);
    Serial.println(" cm");
  }

  Serial.println("================================");
}

void clearStorage() {
  measurementCounter = 0;
  Serial.println("Storage reset.");
}

void activePhase() {
  Serial.println();
  Serial.println("=== ACTIVE MODE ===");
  digitalWrite(LED_PIN, HIGH);
  SensorData data = readSensors();

  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("DHT22 read failed!");
    digitalWrite(LED_PIN, LOW);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  Serial.print("Distance: ");
  Serial.print(data.distance);
  Serial.println(" cm");

  saveMeasurement(data);

  Serial.print("Stored measurements: ");
  Serial.println(measurementCounter);

  delay(1000);

  digitalWrite(LED_PIN, LOW);
}

void handleWakeupReason() {
  //checking cause of exiting sleep
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

  Serial.println();
  Serial.println("=== WAKEUP EVENT ===");

  switch (reason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by TIMER");
      break;

    default:
      Serial.println("Normal startup");
      break;
  }
}

void enterSleepMode() {
  Serial.println();
  Serial.println("Preparing LIGHT SLEEP...");

  // disable WiFi for lower power
  WiFi.mode(WIFI_OFF);

  // timer wakeup
  esp_sleep_enable_timer_wakeup(
      WAKEUP_INTERVAL_SEC * 1000000ULL
  );

  Serial.println("Entering LIGHT SLEEP...");
  Serial.flush();

  esp_light_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  initSensors();
  handleWakeupReason();
}

void loop() {
  //get sensor data
  activePhase();

  if (measurementCounter >= MAX_MEASUREMENTS) {
    printStoredData();
    clearStorage();
  }

  enterSleepMode();
}