#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "log.h"
#include "homeassistant.h"
#include "mqtt.h"
#include "sensors.h"

enum device_class_t
{
  HA_DEVICE_CLASS_TEMPERATURE = 0,
  HA_DEVICE_CLASS_HUMDITY = 1
};

mqtt_device_t create_mqtt_device(sensor_t sensor)
{
  mqtt_device_t device;
  device.id = sensor_to_identifier(sensor);
  device.name = sensor_to_display_name(sensor);

  return device;
}

mqtt_sensor_t create_mqtt_sensor(device_class_t device_class, bool raw_data)
{
  mqtt_sensor_t sensor;

  switch (device_class)
  {
    case HA_DEVICE_CLASS_TEMPERATURE:
      sensor.sensor_type = MQTT_SENSOR_TYPE_TEMPERATURE;
      sensor.id = raw_data ? "temperature_raw" : "temperature";
      sensor.name = raw_data ? "Temperatur (Rohdaten)" : "Temperatur";
      break;

    case HA_DEVICE_CLASS_HUMDITY:
      sensor.sensor_type = MQTT_SENSOR_TYPE_HUMIDITY;
      sensor.id = raw_data ? "humidity_raw" : "humidity";
      sensor.name = raw_data ? "Feuchtigkeit (Rohdaten)" : "Feuchtigkeit";
      break;
  }

  return sensor;
}

bool homeassistant_init()
{
  bool success = true;

  mqtt_connect();

  for (int i = SENSOR_MIN_SENSOR; i <= SENSOR_MAX_SENSOR; i++)
  {
    sensor_t sensor = static_cast<sensor_t>(i);

    mqtt_device_t device = create_mqtt_device(sensor);
    mqtt_sensor_t sensor_temperature_raw = create_mqtt_sensor(HA_DEVICE_CLASS_TEMPERATURE, true);
    mqtt_sensor_t sensor_temperature = create_mqtt_sensor(HA_DEVICE_CLASS_TEMPERATURE, false);
    mqtt_sensor_t sensor_humidity_raw = create_mqtt_sensor(HA_DEVICE_CLASS_HUMDITY, true);
    mqtt_sensor_t sensor_humidity = create_mqtt_sensor(HA_DEVICE_CLASS_HUMDITY, false);

    bool temperature_raw_success = mqtt_advertise(&device, &sensor_temperature_raw);
    bool temperature_success = mqtt_advertise(&device, &sensor_temperature);
    bool humidity_raw_success = mqtt_advertise(&device, &sensor_humidity_raw);
    bool humidity_success = mqtt_advertise(&device, &sensor_humidity);

    success = success && temperature_raw_success && temperature_success && humidity_raw_success && humidity_success;
  }

  mqtt_disconnect();
  return success;
}

bool homeasisstant_transmit(sensor_data_t *raw_data, sensor_data_t *filtered_data)
{
  if (raw_data->sensor != filtered_data->sensor)
  {
    LOG_PRINT(F("Sensor mismatch. Raw data: "));
    LOG_PRINT(sensor_to_identifier(raw_data->sensor));
    LOG_PRINT(F(", filtered data: "));
    LOG_PRINTLN(sensor_to_identifier(filtered_data->sensor));

    return false;
  }

  mqtt_device_t device = create_mqtt_device(raw_data->sensor);
  mqtt_sensor_t sensor_temperature_raw = create_mqtt_sensor(HA_DEVICE_CLASS_TEMPERATURE, true);
  mqtt_sensor_t sensor_temperature = create_mqtt_sensor(HA_DEVICE_CLASS_TEMPERATURE, false);
  mqtt_sensor_t sensor_humidity_raw = create_mqtt_sensor(HA_DEVICE_CLASS_HUMDITY, true);
  mqtt_sensor_t sensor_humidity = create_mqtt_sensor(HA_DEVICE_CLASS_HUMDITY, false);

  mqtt_connect();
  bool temperature_raw_success = mqtt_publish(&device, &sensor_temperature_raw, raw_data->temperature);
  bool temperature_success = mqtt_publish(&device, &sensor_temperature, filtered_data->temperature);
  bool humidity_raw_success = mqtt_publish(&device, &sensor_humidity_raw, raw_data->humidity);
  bool humidity_success = mqtt_publish(&device, &sensor_humidity, filtered_data->humidity);
  mqtt_disconnect();

  return temperature_raw_success && temperature_success && humidity_raw_success && humidity_success;
}
