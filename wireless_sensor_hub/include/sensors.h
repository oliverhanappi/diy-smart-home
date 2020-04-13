#ifndef SENSORS_H_
#define SENSORS_H_

#define SENSOR_MIN_SENSOR 1
#define SENSOR_MAX_SENSOR 4

enum sensor_t
{
  SENSOR_LIVING_ROOM = 1,
  SENSOR_BATH_ROOM = 2,
  SENSOR_BED_ROOM = 3,
  SENSOR_TEST = 4
};

struct sensor_data_t
{
  sensor_t sensor;
  float temperature;
  float humidity;
};

bool sensors_setup();
bool sensors_read(sensor_data_t *result);

const char* sensor_to_identifier(sensor_t sensor);
const char* sensor_to_display_name(sensor_t sensor);

#endif