#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#include <WiFiUtils.h>
#include <LedClock.h>
#include <RealTimeClock.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN     13
#define NEOPIXEL_PIN    16
#define NUM_PIXELS      60

#define HOLD 200

//#define DEBUG

LedClock::LedClock *ledClock;
RealTimeClock::RealTimeClock* RTC;

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

    Serial.println("       RTC Functions      ");
    Serial.println("--------------------------");

    // Start RTC
    RTC = new RealTimeClock::RealTimeClock();
}

void loop()
{
//    Serial.println("Hello. world!");
    Serial.println(RTC->GetUnixTime());

    RealTimeClock::DateTime dateTime = RTC->GetDateTime();
#ifdef DEBUG
    int minutes = dateTime.Second;
    int hours = dateTime.Minute % 12;
#else
    int minutes = dateTime.Minute;
    int hours = dateTime.Hour;
#endif

    if (dateTime.Hour == 2 && dateTime.Minute == 0 && dateTime.Second)
    {
        Serial.println("Check Daylights Savings Time.");
        RTC->CheckDaylightSavingsTime();
    }

    ledClock->UpdateTime(hours, minutes);
    delay(HOLD);
}