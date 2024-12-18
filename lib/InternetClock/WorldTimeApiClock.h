#pragma once

#include "IInternetClock.h"

#include "DataTypes.h"

#include <Arduino.h>

namespace InternetClock
{
    class WorldTimeApiClock final : public IInternetClock
    {
    public:
        WorldTimeApiClock();

    public:
        bool GetInternetDateTime() override;
        bool IsDaylightSavings() override;
        byte GetDaylightSavingsOffset() override;
        DateTime GetDateTime() override;
        time_t GetUnixTime() override;
        DateTime GetDaylightSavingsChangeDateTime() override;

    private:
        DateTime m_dateTime;
        time_t m_unixTime;
        bool m_isDaylightSavingsTime;
        byte m_dstOffsetMinutes;
        DateTime m_dstChangeDateTime;
    };
} // namespace InternetClock
