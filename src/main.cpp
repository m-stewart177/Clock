#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include <WiFiUtils.h>
#include <LedClock.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN     13
#define NEOPIXEL_PIN    16
#define NUM_PIXELS      60

#define HOLD 200

int minutes = 0;
int hours = 0;

char hostName[] = "worldtimeapi.org";

LedClock::LedClock *ledClock;

void setup()
{

    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    InitialiseWiFi();


    LedClock::DisplayConfiguration config;
    config.NeoPixelPin = NEOPIXEL_PIN;
    config.DisplaySize = NUM_PIXELS;
    config.NeoPixelType = NEO_GRB + NEO_KHZ800;
    config.ClockRingStart = 0;
    config.ClockRingSize = NUM_PIXELS;
    config.ClockPhase = 30;

    LedClock::ClockDisplayMarkerColours colours;

    ledClock = new LedClock::LedClock(config, colours);
    ledClock->InitialiseDisplay();

    WiFiClient client;
    if (client.connect(hostName, 80))
    {
        Serial.print("Connected to server: ");
        Serial.println(client.remoteIP());
    }
    else
    {
        Serial.println("Failed to connect to server");
    }

    client.println("GET /api/ip HTTP/1.1");
    Serial.println("GET /api/ip HTTP/1.1");
    client.println("Host: " + String(hostName));
    Serial.println("Host: " + String(hostName));
//    client.println("Connection: close");
//    Serial.println("Connection: close");
    client.println("Accept: */*");
    Serial.println("Accept: */*");
    client.println();
    Serial.println();

    while (!client.available());

    while (client.available())
    {
        char c = client.read();
        Serial.print(c);
    }
    Serial.println();
    Serial.println("End message");

    if (!client.connected())
    {
        Serial.println("disconnected");
        client.stop();
    }
    else
    {
        Serial.println("Not connected");
    }
}

void loop()
{
// write your code here
    Serial.println("Hello. world!");

    ++minutes;
    minutes %= 60;
    if (minutes == 0)
    {
        ++hours;
        hours %= 12;
    }

    ledClock->UpdateTime(hours, minutes);
    delay(HOLD);
}