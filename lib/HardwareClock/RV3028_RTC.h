#pragma once

#include "IRTC.h"

#include <RV3028.h>

#include <Arduino.h>

namespace HardwareClock
{
    class RV3028_RTC final : public IRTC
    {
    public:
        RV3028_RTC();

        bool Begin() override;
        byte GetSeconds() const override;
        byte GetMinutes() const override;
        byte GetHours() const override;
        byte GetWeekDay() const override;
        byte GetDay() const override;
        byte GetMonth() const override;
        byte GetYear() const override;

        DateTime GetDateTime() const override;
        DateTime GetDaylightSavingsChangeDateTime() const override;

        bool IsDaylightSaving() const override;
        byte GetDaylightSavingOffset() const override;

        time_t GetUnixTime() const override;
        void SetUnixTime(time_t unixTime) override;

        String GetTimeStamp() const override;

        void SaveSettings(DateTime const& datetime, bool daylightSaving, byte dstOffsetMinutes,
                          DateTime const& dstChangeDateTime) override;

        void SetTime(DateTime const& datetime) override;

        void SetDaylightSavings(bool daylightSavings) override;

    private:
        mutable RV3028 m_RTC;
        time_t m_unixTime;
    };
} // namespace HardwareClock
