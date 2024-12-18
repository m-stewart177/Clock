#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>

using String = std::string;
using byte = u_int8_t;
using time_t = long;
#endif

struct DateTime
{
public:
    DateTime() = default;

    DateTime(byte year, byte month, byte date, byte hour, byte minute, byte second) :
        Year(year),
        Month(month),
        Date(date),
        Hour(hour),
        Minute(minute),
        Second(second)
    {
    }

    byte Year;
    byte Month;
    byte Date;
    byte Hour;
    byte Minute;
    byte Second;

    bool operator>(const DateTime& other) const;
    bool operator==(DateTime const& other) const;
};
