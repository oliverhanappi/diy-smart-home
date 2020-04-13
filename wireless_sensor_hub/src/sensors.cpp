#include <Arduino.h>
#include <RF24.h>
#include "log.h"
#include "sensors.h"

union Message {
  struct
  {
    float temperature;
    float humidity;
  } values;

  byte bytes[sizeof(float) * 2];
};

RF24 radio(21, 22);
bool radioInitialized = true;

const byte address[][6] = {"1SENS", "2SENS", "3SENS", "4SENS"};

bool sensors_setup()
{
  LOG_PRINTLN(F("Initializing radio..."));
  if (!radio.begin())
  {
    LOG_PRINTLN(F("Radio initialization failed."));
    radioInitialized = false;

    return false;
  }

  for (int i = SENSOR_MIN_SENSOR; i <= SENSOR_MAX_SENSOR; i++)
  {
    radio.openReadingPipe(i, address[i - 1]);
  }

  radio.startListening();

  LOG_PRINTLN(F("Radio initialized. Listening for sensor data..."));
  return true;
}

bool sensors_read(sensor_data_t *result)
{
  byte pipe;
  if (radioInitialized && radio.available(&pipe))
  {
    Message message;
    radio.read(&message, sizeof(message));

    if (pipe < SENSOR_MIN_SENSOR || pipe > SENSOR_MAX_SENSOR)
    {
      LOG_PRINT(F("Received data from unknown sensor "));
      LOG_PRINTLN(pipe);

      return false;
    }

    result->sensor = static_cast<sensor_t>(pipe);
    result->temperature = message.values.temperature;
    result->humidity = message.values.humidity;

    LOG_PRINT(F("Received data from sensor "));
    LOG_PRINT(sensor_to_identifier(result->sensor));
    LOG_PRINT(F(": Temperature = "));
    LOG_PRINT(result->temperature);
    LOG_PRINT(F(" °C Humidity = "));
    LOG_PRINT(result->humidity);
    LOG_PRINTLN(F(" %"));

    return true;
  }
  else
  {
    return false;
  }
}

const char* sensor_to_identifier(sensor_t sensor)
{
  switch (sensor)
  {
    case SENSOR_LIVING_ROOM:
      return "livingroom";

    case SENSOR_BATH_ROOM:
      return "bathroom";

    case SENSOR_BED_ROOM:
      return "bedroom";

    case SENSOR_TEST:
      return "test";

    default:
      return "<unknown>";
  }
}

const char *sensor_to_display_name(sensor_t sensor)
{
  switch (sensor)
  {
    case SENSOR_LIVING_ROOM:
      return "Wohnzimmer";

    case SENSOR_BATH_ROOM:
      return "Badezimmer";

    case SENSOR_BED_ROOM:
      return "Schlafzimmer";

    case SENSOR_TEST:
      return "Test";

    default:
      return "<Unbekannt>";
  }
}