#include <Arduino.h>
#include <WiFi.h>
#include "log.h"
#include "wlan.h"

bool wlan_setup()
{
  WiFi.mode(WIFI_STA);

  if (WiFi.status() == WL_CONNECTED)
  {
    LOG_PRINTLN(F("WiFi already connected. Disconnecting..."));
    if (!WiFi.disconnect(false, true))
    {
      LOG_PRINTLN(F("Disconnecting from WiFi failed."));
    }
  }

  LOG_PRINT(F("Connecting to WiFi network "));
  LOG_PRINTLN(F(WLAN_SSID));

  WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    LOG_PRINT(F("."));

    if (millis() - start >= WLAN_CONNECTION_TIMEOUT)
    {
      LOG_PRINTLN();
      LOG_PRINTLN("Connection attempt to WiFi network timed out.");

      return false;
    }
  }

  LOG_PRINTLN();
  LOG_PRINT(F("WiFi connected. IP address: "));
  LOG_PRINTLN(WiFi.localIP());

  return true;
}

bool wlan_ensure_connected()
{
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status == WL_CONNECTED)
  {
    return true;
  }

  LOG_PRINT(F("WiFi status is "));
  LOG_PRINT(wifi_status);
  LOG_PRINTLN(F(". Reconnecting..."));

  return wlan_setup();
}