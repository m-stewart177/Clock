#pragma once

#include "IRealTimeClock.h"

#include "IInternetClock.h"
#include "IRTC.h"

#include "DataTypes.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>

using String = std::string;
using byte = u_int8_t;
using time_t = long;
#endif

using HardwareClock::IRTC;
using InternetClock::IInternetClock;

namespace RealTimeClock
{
    class RealTimeClock : public IRealTimeClock
    {
    public:
        RealTimeClock(IRTC& rtc, IInternetClock& internetClock);

    public:
        void Initialise() override;
        void CheckDaylightSavingsTime() override;
        DateTime& GetDateTime() override;
        time_t GetUnixTime() override;
        String TimeStamp() override;

    private:
        void UpdateToInternetTime();

    private:
        IRTC& m_RTC;
        IInternetClock& m_internetClock;
        DateTime m_dateTime;
        time_t m_unixTime;
        bool m_daylightSavingsTime;
        byte m_dstOffsetMinutes;
        DateTime m_dstChange;
    };
} // RealTimeClock
