#pragma once

#include <Arduino.h>

struct DateTime;

namespace RealTimeClock
{
    class IRealTimeClock
    {
    public:
        virtual ~IRealTimeClock() = default;

        virtual void Initialise() = 0;
        virtual void CheckDaylightSavingsTime() = 0;
        virtual DateTime& GetDateTime() = 0;
        virtual time_t GetUnixTime() = 0;
        virtual String TimeStamp() = 0;
    };
} // namespace RealTimeClock
