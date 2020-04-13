#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "log.h"
#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

bool mqtt_connect()
{
  int retries = 0;

  while (!mqtt_client.connected())
  {
    if (retries >= 3)
    {
      LOG_PRINTLN(F("Failed to connect to MQTT broker after 3 retries."));
      return false;
    }

    LOG_PRINT(F("Connecting to MQTT broker "));
    LOG_PRINT(MQTT_SERVER);
    LOG_PRINT(F(":"));
    LOG_PRINT(MQTT_PORT);
    LOG_PRINT(" with user ");
    LOG_PRINT(MQTT_USERNAME);
    LOG_PRINTLN(F("..."));

    mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
    if (mqtt_client.connect("diy-sensor-hub", MQTT_USERNAME, MQTT_PASSWORD))
    {
      LOG_PRINTLN(F("Successfully connected to MQTT broker."));
      return true;
    }
    else
    {
      LOG_PRINTLN(F("Failed to connect to MQTT broker. Retrying in 5 seconds."));
      delay(5000);
    }
    
    retries++;
  }

  return true;
}

const char* mqtt_get_device_class(mqtt_sensor_type_t sensor_type)
{
  switch (sensor_type)
  {
    case MQTT_SENSOR_TYPE_TEMPERATURE:
      return "temperature";

    case MQTT_SENSOR_TYPE_HUMIDITY:
      return "humidity";

    default:
      return "<unknown>";
  }
}

const char* mqtt_get_unit_of_measurement(mqtt_sensor_type_t sensor_type)
{
  switch (sensor_type)
  {
    case MQTT_SENSOR_TYPE_TEMPERATURE:
      return "°C";

    case MQTT_SENSOR_TYPE_HUMIDITY:
      return "%";

    default:
      return "<unknown>";
  }
}

bool mqtt_advertise(mqtt_device_t *device, mqtt_sensor_t *sensor)
{
  String device_id = String("diy_sensor_") + device->id;
  String device_name = String(device->name) + " Sensor";
  String sensor_id = device_id + "_" + sensor->id;
  String sensor_name = device_name + " - " + sensor->name;

  String topic_base = String("homeassistant/sensor/") + sensor_id;
  String config_topic = topic_base + "/config";
  String state_topic = topic_base + "/state";

  StaticJsonDocument<512> json_payload;
  JsonObject json_device = json_payload.createNestedObject("device");
  JsonArray json_device_identifiers = json_device.createNestedArray("identifiers");
  json_device_identifiers.add(device_id);
  json_device["manufacturer"] = "DIY";
  json_device["model"] = "DIY Sensor";
  json_device["name"] = device_name;

  json_payload["device_class"] = mqtt_get_device_class(sensor->sensor_type);
  json_payload["name"] = sensor_name;
  json_payload["unique_id"] = sensor_id;
  json_payload["state_topic"] = state_topic;
  json_payload["unit_of_measurement"] = mqtt_get_unit_of_measurement(sensor->sensor_type);

  String payload;
  serializeJson(json_payload, payload);

  LOG_PRINT("Advertising sensor at topic ");
  LOG_PRINTLN(config_topic);
  LOG_PRINTLN(payload);
  LOG_PRINTLN();

  bool result = mqtt_client.publish(config_topic.c_str(), payload.c_str(), true);

  if (!result) {
    LOG_PRINTLN(F("Failed to advertise sensor."));
  }

  return result;
}

bool mqtt_publish(mqtt_device_t *device, mqtt_sensor_t *sensor, float value)
{
  String sensor_id = String("diy_sensor_") + device->id + "_" + sensor->id;

  String topic = String("homeassistant/sensor/") + sensor_id + "/state";
  String payload = String(value);

  LOG_PRINT("Publishing sensor value to topic ");
  LOG_PRINTLN(topic);
  LOG_PRINTLN(payload);
  LOG_PRINTLN();

  bool result = mqtt_client.publish(topic.c_str(), payload.c_str());

  if (!result) {
    LOG_PRINTLN(F("Failed to publish sensor data."));
  }

  return result;
}

void mqtt_disconnect()
{
  mqtt_client.disconnect();
}
