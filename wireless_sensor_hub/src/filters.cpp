#include "Arduino.h"
#include "filters.h"
#include "log.h"
#include "sensors.h"

#define FILTER_TEMPERATURE_WEIGHT1  0.25f
#define FILTER_TEMPERATURE_WEIGHT2  0.25f
#define FILTER_TEMPERATURE_SCALE    100.0f

#define FILTER_HUMIDITY_WEIGHT1  0.25f
#define FILTER_HUMIDITY_WEIGHT2  0.75f
#define FILTER_HUMIDITY_SCALE    10.0f

struct filter_state_t {
  float state1;
  float state2;
};

bool initialized = false;
filter_state_t filter_states_temperature[SENSOR_MAX_SENSOR + 1];
filter_state_t filter_states_humidity[SENSOR_MAX_SENSOR + 1];

void ensure_initialized()
{
  if (initialized)
    return;

  LOG_PRINTLN(F("Initializing filter states..."));

  for (int i = SENSOR_MIN_SENSOR; i <= SENSOR_MAX_SENSOR; i++)
  {
    filter_states_temperature[i].state1 = NAN;
    filter_states_temperature[i].state2 = NAN;
    filter_states_humidity[i].state1 = NAN;
    filter_states_humidity[i].state2 = NAN;
  }

  initialized = true;
}

float compute(filter_state_t *state, float value, float weight1, float weight2, float scale)
{
  if (isnan(state->state1))
  {
    LOG_PRINT(F("No previous value. Using raw value: "));
    LOG_PRINTLN(value);

    state->state1 = value;
    state->state2 = value;
  }
  else
  {
    LOG_PRINT(F("Computing filtered value for raw value: "));
    LOG_PRINT(value);
    LOG_PRINT(F(", state1: "));
    LOG_PRINT(state->state1);
    LOG_PRINT(F(", weight1: "));
    LOG_PRINT(weight1);
    LOG_PRINT(F(", state2: "));
    LOG_PRINT(state->state2);
    LOG_PRINT(F(", weight2: "));
    LOG_PRINT(weight2);
    LOG_PRINT(F(", scale: "));
    LOG_PRINTLN(scale);

    state->state1 = weight1 * value + (1 - weight1) * state->state1;
    state->state2 = weight2 * state->state1 + (1 - weight2) * state->state2;
  }

  float result = roundf(state->state2 * scale) / scale;

  LOG_PRINT(F("Result is: "));
  LOG_PRINTLN(result);

  return result;
}

sensor_data_t apply_filter(sensor_data_t *raw_data)
{
  ensure_initialized();

  sensor_data_t result;
  result.sensor = raw_data->sensor;
  result.temperature = compute(&filter_states_temperature[result.sensor], raw_data->temperature, FILTER_TEMPERATURE_WEIGHT1, FILTER_TEMPERATURE_WEIGHT2, FILTER_TEMPERATURE_SCALE);
  result.humidity = compute(&filter_states_humidity[result.sensor], raw_data->humidity, FILTER_HUMIDITY_WEIGHT1, FILTER_HUMIDITY_WEIGHT2, FILTER_HUMIDITY_SCALE);

  LOG_PRINT(F("Applied filter. Temperature: "));
  LOG_PRINT(result.temperature);
  LOG_PRINT(F(" °C, Humidity: "));
  LOG_PRINT(result.humidity);
  LOG_PRINTLN(F(" %"));

  return result;
}
