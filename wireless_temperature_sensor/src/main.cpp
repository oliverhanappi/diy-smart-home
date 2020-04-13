#include <Arduino.h>
#include <DHT.h>
#include <LowPower.h>
#include <RF24.h>
#include "log.h"

DHT dht(3, DHT22);
RF24 radio(7, 6);
const byte address[6] = SENSOR_ADDRESS;

union Message {
  struct {
    float temperature;
    float humidity;
  } values;

  byte bytes[sizeof(float) * 2];
};

void setup() {
  LOG_SETUP;

  LOG_PRINTLN(F("Initializing radio..."));
  if (!radio.begin()) {
    LOG_PRINTLN(F("Radio initialization failed."));

    for(;;) {
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
    }
  }

  radio.openWritingPipe(address);
  radio.stopListening();

  LOG_PRINTLN(F("Initializing temperature sensor..."));
  dht.begin();

  LOG_FLUSH;
  LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_ON);
}

void loop() {
  Message message;

  // The DHT library caches sensor values for 2 seconds before reading values again.
  // Due to standby timer0 is stopped, hence millis() is not counting correctly.
  // Therefore we force reading the sensor.

  boolean read = dht.read(true);
  message.values.temperature = dht.readTemperature();
  message.values.humidity = dht.readHumidity();

  if (!read || isnan(message.values.temperature) || isnan(message.values.humidity)) {
    LOG_PRINTLN(F("Failed to read DHT."));
  } else {
    LOG_PRINT(F("Temperature: "));
    LOG_PRINT(message.values.temperature);
    LOG_PRINTLN(F(" °C"));

    LOG_PRINT(F("Humidity: "));
    LOG_PRINT(message.values.humidity);
    LOG_PRINTLN(F(" %"));

    radio.powerUp();

    if (!radio.write(&message, sizeof(message))) {
      LOG_PRINTLN(F("Failed to send temperature data."));
    }

    radio.powerDown();
  }

  LOG_FLUSH;

  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_ON);
  LowPower.powerStandby(SLEEP_4S, ADC_OFF, BOD_ON);
}