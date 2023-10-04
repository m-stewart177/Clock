//
// Created by mike on 04/10/23.
//

#include "LedClock.h"

namespace LedClock
{
    LedClock::LedClock(DisplayConfiguration config, ClockDisplayMarkerColours clockClours) :
            m_clockRingSize(config.ClockRingSize),
            m_clockRingStart(config.ClockRingStart),
            m_clockPhase(config.ClockPhase),
            m_neoPixel(config.DisplaySize, config.NeoPixelPin, config.NeoPixelType),
            m_clockColours(clockClours)
    {}

    void LedClock::InitialiseDisplay()
    {
        m_neoPixel.begin();
        m_neoPixel.clear();
        for (int i = 0; i < m_clockRingSize; i += 5)
        {
            int pixel = m_clockRingStart + i;
            if (i % 15 == 0)
            {
                m_neoPixel.setPixelColor(pixel, m_clockColours.Cardinal);
            }
            else
            {
                m_neoPixel.setPixelColor(pixel, m_clockColours.Marker);
            }
        }
        m_neoPixel.show();
    }

    void LedClock::UpdateTime(int hours, int minutes)
    {
        int last_pixel = (minutes + m_clockPhase - 1) % m_clockRingSize;
        if (last_pixel % 5 == 0)
        {
            if (last_pixel % 15 == 0)
            {
                m_neoPixel.setPixelColor(last_pixel, m_clockColours.Cardinal);
            }
            else
            {
                m_neoPixel.setPixelColor(last_pixel, m_clockColours.Marker);
            }
        }
        else
        {
            m_neoPixel.setPixelColor(last_pixel, m_clockColours.Blank);
        }

        if (minutes == 0)
        {
            last_pixel = ((hours - 1) * 5 + m_clockPhase) % m_clockRingSize;
            if (last_pixel % 15 == 0)
            {
                m_neoPixel.setPixelColor(last_pixel, m_clockColours.Cardinal);
            }
            else
            {
                m_neoPixel.setPixelColor(last_pixel, m_clockColours.Marker);
            }
        }

        int pixel = (hours * 5 + m_clockPhase) % m_clockRingSize;
        m_neoPixel.setPixelColor(pixel, m_clockColours.Hour);

        pixel = (minutes + m_clockPhase) % m_clockRingSize;
        m_neoPixel.setPixelColor(pixel, m_clockColours.Minute);
        m_neoPixel.show();
    }
} // LedClock