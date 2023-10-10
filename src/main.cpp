#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#include <RV3028.h>

#include <WiFiUtils.h>
#include <LedClock.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN     13
#define NEOPIXEL_PIN    16
#define NUM_PIXELS      60

#define RV3028_I2C_ADDR 0x52

#define HOLD 200

//#define DEBUG

LedClock::LedClock *ledClock;
RV3028* RTC;

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

    JSONVar json = GetJson("worldtimeapi.org", 80, "/api/ip");

    Serial.println(json);
    Serial.println();

    uint32_t internet_unix_time = (unsigned long)json["unixtime"];
    String utc_date_time = json["utc_datetime"];
    String local_date_time = json["datetime"];

    Serial.print("Unix time: ");
    Serial.println( (unsigned long)json["unixtime"]);
    Serial.print("Date time (local):");
    Serial.println(local_date_time);
    Serial.print("Date time (UTC):");
    Serial.println(utc_date_time);

    Serial.println("       RTC Functions      ");
    Serial.println("--------------------------");

    // Start RTC
    RTC = new RV3028();
    RTC->Begin();
    uint32_t rtc_unix_time = RTC->Get_UNIX_Time();
    if (abs((int)(rtc_unix_time - internet_unix_time)) > 2)
    {
        byte year = local_date_time.substring(2, 4).toInt();
        byte month = local_date_time.substring(5, 7).toInt();
        byte day = local_date_time.substring(8, 10).toInt();
        byte hour = local_date_time.substring(11,13).toInt();
        byte minute = local_date_time.substring(14, 16).toInt();
        byte second = local_date_time.substring(17, 19).toInt();

        Serial.print(String(year) + "-" + String(month) + "-" + String(day) + " ");
        Serial.println(String(hour) + ":" + String(minute) + ":" + String(second));

        RTC->Set_Time(second, minute, hour, day, month, year);
        RTC->Update_Time_Stamp();
        RTC->Set_UNIX_Time(internet_unix_time);
    }
}

void loop()
{
// write your code here
    RTC->Update_Time_Stamp();
//    Serial.println("Hello. world!");
    Serial.println(RTC->Time_Stamp);
    Serial.println(RTC->Get_UNIX_Time());

#ifdef DEBUG
    int minutes = RTC->Get_Second();
    int hours = RTC->Get_Minute() % 12;
#else
    int minutes = RTC->Get_Minute();
    int hours = RTC->Get_Hour();
#endif

    ledClock->UpdateTime(hours, minutes);
    delay(HOLD);
}