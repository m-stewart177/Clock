#include "Arduino.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN     13

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
}