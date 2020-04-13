# DIY Smart Home

This repository contains code to build smart home components on your own, which can be integrated with [Home Assistant](https://www.home-assistant.io/).

It currently consists of the following components:
* **Wireless Temperature Sensor:** Battery powered temperature and humidity sensor based on the popular ATmega328p microcontroller, which uses a DHT22 module for measurements and a NRF24L01+ module for wireless transmission.
* **Wireless Sensor Hub:** ESP32 based hub which receives wireless sensor measurements through a NRF24L01+ module and forwards it to an MQTT broker for consumption by Home Assistant. Supports [MQTT Discovery](https://www.home-assistant.io/docs/mqtt/discovery/).
