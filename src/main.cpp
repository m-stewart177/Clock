#include <Arduino.h>

#include "Adafruit_NeoPixel.h"

#include <WiFi.h>
#include <WiFiManager.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN     13
#define NEOPIXEL_PIN    16
#define NUM_PIXELS      60

#define HOLD 200

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int minutes = 0;
int hours = 0;

uint32_t marker = Adafruit_NeoPixel::Color(8, 8, 8);
uint32_t cardinal = Adafruit_NeoPixel::Color(0, 32, 0);
uint32_t minute = Adafruit_NeoPixel::Color(16, 0, 0);
uint32_t hour = Adafruit_NeoPixel::Color(0, 0, 16);
uint32_t blank = Adafruit_NeoPixel::Color(0, 0, 0);

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    NeoPixel.begin();

    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
//    wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("ESP_Clock", "ESP_Clock"); // password protected ap

    if (!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } else {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
    }
    NeoPixel.clear();
    for (int pixel = 0; pixel < NUM_PIXELS; pixel += 5) {
        if (pixel % 15 == 0) {
            NeoPixel.setPixelColor(pixel, cardinal);
        } else {
            NeoPixel.setPixelColor(pixel, marker);
        }
    }
    NeoPixel.show();
}

void loop() {
// write your code here
    Serial.println("Hello. world!");

    int last_pixel = (minutes + 30) % 60;
    if (last_pixel % 5 == 0) {
        if (last_pixel % 15 == 0) {
            NeoPixel.setPixelColor(last_pixel, cardinal);
        } else {
            NeoPixel.setPixelColor(last_pixel, marker);
        }
    } else {
        NeoPixel.setPixelColor(last_pixel, blank);
    }

    ++minutes;
    minutes %= 60;
    if (minutes == 0) {
        last_pixel = (hours * 5 + 30) % 60;
        if (last_pixel % 15 == 0) {
            NeoPixel.setPixelColor(last_pixel, cardinal);
        } else {
            NeoPixel.setPixelColor(last_pixel, marker);
        }

        ++hours;
        hours %= 12;
    }

    int pixel = (hours * 5 + 30) % 60;
    NeoPixel.setPixelColor(pixel, hour);

    pixel = (minutes + 30) % 60;
    NeoPixel.setPixelColor(pixel, minute);
    NeoPixel.show();

    delay(HOLD);
}