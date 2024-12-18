#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>

using String = std::string;
using byte = u_int8_t;
using time_t = long;
#endif

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
