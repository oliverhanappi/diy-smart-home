#include "sensors.h"

#ifndef HOMEASSISTANT_H_
#define HOMEASSISTANT_H_

bool homeassistant_init();
bool homeasisstant_transmit(sensor_data_t *raw_data, sensor_data_t *filtered_data);

#endif