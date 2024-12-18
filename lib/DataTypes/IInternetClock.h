#pragma once

#include "DataTypes.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>

using String = std::string;
using byte = u_int8_t;
using time_t = long;
#endif

namespace InternetClock
{
    class IInternetClock
    {
    public:
        virtual ~IInternetClock() = default;

        virtual bool GetInternetDateTime() = 0;
        virtual bool IsDaylightSavings() = 0;
        virtual byte GetDaylightSavingsOffset() = 0;
        virtual DateTime GetDateTime() = 0;
        virtual time_t GetUnixTime() = 0;
        virtual DateTime GetDaylightSavingsChangeDateTime() = 0;
    };
} // namespace InternetClock
