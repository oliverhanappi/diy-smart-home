#include <Arduino.h>

#ifndef LOG_H_
#define LOG_H_

#ifdef LOG_ENABLED
  #define LOG_SETUP { Serial.begin(9600); }
  #define LOG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define LOG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
  #define LOG_FLUSH { Serial.flush(); }
#else
  #define LOG_SETUP {}
  #define LOG_PRINT(...) {}
  #define LOG_PRINTLN(...) {}
  #define LOG_FLUSH {}
#endif

#endif