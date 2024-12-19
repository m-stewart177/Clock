#pragma once

#include <DataTypes.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>

using String = std::string;
using byte = u_int8_t;
using time_t = long;
#endif


namespace HardwareClock
{
class IRTC
{
public:
    virtual ~IRTC() = default;

    virtual bool Begin() = 0;
    virtual byte GetSeconds() const = 0;
    virtual byte GetMinutes() const = 0;
    virtual byte GetHours() const = 0;
    virtual byte GetWeekDay() const = 0;
    virtual byte GetDay() const = 0;
    virtual byte GetMonth() const = 0;
    virtual byte GetYear() const = 0;

    virtual DateTime GetDateTime() const = 0;
    virtual DateTime GetDaylightSavingsChangeDateTime() const = 0;
    virtual bool IsDaylightSaving() const = 0;
    virtual byte GetDaylightSavingOffset() const = 0;

    virtual time_t GetUnixTime() const = 0;
    virtual void SetUnixTime(time_t unixTime) = 0;

    virtual String GetTimeStamp() const = 0;

    virtual void SaveSettings(DateTime const& datetime, bool daylightSaving, byte dstOffsetMinutes,
                              DateTime const& dstChangeDateTime) = 0;

    virtual void SetTime(DateTime const& datetime) = 0;

    virtual void SetDaylightSavings(bool daylightSavings) = 0;
};
}
