#ifndef MQTT_H_
#define MQTT_H_

#ifndef MQTT_SERVER
#error MQTT_SERVER not defined
#endif

#ifndef MQTT_PORT
#error MQTT_PORT not defined
#endif

#ifndef MQTT_USERNAME
#error MQTT_USERNAME not defined
#endif

#ifndef MQTT_PASSWORD
#error MQTT_PASSWORD not defined
#endif

struct mqtt_device_t {
  const char *id;
  const char *name;
};

enum mqtt_sensor_type_t {
  MQTT_SENSOR_TYPE_TEMPERATURE = 1,
  MQTT_SENSOR_TYPE_HUMIDITY = 2,
};

struct mqtt_sensor_t {
  mqtt_sensor_type_t sensor_type;

  const char *id;
  const char *name;
};

bool mqtt_connect();
bool mqtt_advertise(mqtt_device_t *device, mqtt_sensor_t *sensor);
bool mqtt_publish(mqtt_device_t *device, mqtt_sensor_t *sensor, float value);
void mqtt_disconnect();

#endif