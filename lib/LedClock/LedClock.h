//
// Created by mike on 04/10/23.
//

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

namespace LedClock
{
    struct DisplayConfiguration
    {
        uint16_t NeoPixelPin;       // GPIO pin for NeoPixels
        int16_t DisplaySize;        // Total number of NeoPixels in display
        uint16_t NeoPixelType;      // Type of NeoPixel
        int16_t ClockRingSize = 60; // Number of NeoPixels in Clock ring
        int16_t ClockRingStart = 0; // NeoPixel "address" for first pixel in clock ring
        int16_t ClockPhase;         // Pixel offest to 12 o'clock position in the ring
                                    // For example, if pixel 0 is at 6 o'clock position
                                    // ClockPhase = 30
    };

    struct ClockDisplayMarkerColours
    {
        uint32_t Marker = Adafruit_NeoPixel::Color(8, 8, 8);    // Default hour marker
        uint32_t Cardinal = Adafruit_NeoPixel::Color(0, 32, 0); // 12, 3, 6, and 9 hour
        uint32_t Minute = Adafruit_NeoPixel::Color(16, 0, 0);   // Minute "hand"
        uint32_t Hour = Adafruit_NeoPixel::Color(0, 0, 16);     // Hour "hand"
        uint32_t Blank = Adafruit_NeoPixel::Color(0, 0, 0);
    };

    class LedClock
    {
    public:
        LedClock(DisplayConfiguration config, ClockDisplayMarkerColours clockClours);

    public:
        void InitialiseDisplay();
        void UpdateTime(int hours, int minutes);

    private:
        int16_t m_clockRingSize;
        int16_t m_clockRingStart;
        int16_t m_clockPhase;
        Adafruit_NeoPixel m_neoPixel;
        ClockDisplayMarkerColours m_clockColours;
    };

} // LedClock
