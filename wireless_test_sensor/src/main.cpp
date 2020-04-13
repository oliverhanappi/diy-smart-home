#include <Arduino.h>
#include <RF24.h>

RF24 radio(7, 6);
const byte address[6] = "4SENS";

void setup()
{
  Serial.begin(9600);

  if (!radio.begin())
  {
    Serial.println(F("Radio initialization failed."));
  }

  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Initialized.");
}

union Message {
  struct
  {
    float temperature;
    float humidity;
  } values;

  byte bytes[sizeof(float) * 2];
};

void loop()
{
  Serial.print("Sending data on address ");
  Serial.write(&address[0], 5);
  Serial.println();

  Message message;
  message.values.temperature = 20;
  message.values.humidity = 30;

  if (!radio.write(&message, sizeof(message)))
  {
    Serial.println(F("Failed to send temperature data."));
  }

  delay(8000);
}