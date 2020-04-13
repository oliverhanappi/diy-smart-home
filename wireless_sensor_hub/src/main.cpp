#include <Arduino.h>
#include "filters.h"
#include "homeassistant.h"
#include "log.h"
#include "sensors.h"
#include "wlan.h"

void setup()
{
  LOG_SETUP;
  LOG_PRINTLN(F("Initializing..."));

  if (!wlan_setup())
  {
    LOG_PRINTLN(F("Failed to set up WiFi network."));
  }

  if (!sensors_setup())
  {
    LOG_PRINTLN(F("Failed to setup sensors."));
  }

  if (!homeassistant_init())
  {
    LOG_PRINTLN(F("Failed to setup Home Assisstant."));
  }

  LOG_PRINTLN(F("Initialization finished."));
}

void loop()
{
  sensor_data_t raw_data, filtered_data;

  if (sensors_read(&raw_data))
  {
    if (wlan_ensure_connected())
    {
      filtered_data = apply_filter(&raw_data);

      if (!homeasisstant_transmit(&raw_data, &filtered_data))
      {
        LOG_PRINTLN(F("Failed to transmit sensor data to Home Assistant."));
      }
    }
    else
    {
      LOG_PRINTLN(F("Skipping transmission of sensor data. Not connected to WiFi."));
    }

    LOG_PRINTLN();
    LOG_PRINTLN();
  }
}