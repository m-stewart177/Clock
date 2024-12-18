//
// Created by mike on 18/12/24.
//

#include "WorldTimeApiClock.h"


#include <Arduino_JSON.h>

#include <WiFiUtils.h>

namespace
{
    String const InternetTimeApi = "worldtimeapi.org";
    byte constexpr InternetTimeApiPort = 80;

    String const LocalDateTimeQuery = "/api/ip";


    DateTime ParseISO8601DateTime(String const& iso8601DateTime)
    {
        return {
            static_cast<byte>(iso8601DateTime.substring(2, 4).toInt()),
            static_cast<byte>(iso8601DateTime.substring(5, 7).toInt()),
            static_cast<byte>(iso8601DateTime.substring(8, 10).toInt()),
            static_cast<byte>(iso8601DateTime.substring(11, 13).toInt()),
            static_cast<byte>(iso8601DateTime.substring(14, 16).toInt()),
            static_cast<byte>(iso8601DateTime.substring(17, 19).toInt())
        };
    }
} // namespace

namespace InternetClock
{
    WorldTimeApiClock::WorldTimeApiClock() :
        m_dateTime(),
        m_unixTime(),
        m_isDaylightSavingsTime(),
        m_dstOffsetMinutes(),
        m_dstChangeDateTime()
    {
    }

    bool WorldTimeApiClock::GetInternetDateTime()
    {
        ;
        JSONVar json = GetJson(const_cast<char*>(InternetTimeApi.c_str()), InternetTimeApiPort, LocalDateTimeQuery);

        if (json.length() == 0)
        {
            Serial.println("World Time Api offline?");
            return false;
        }

        m_dateTime = ParseISO8601DateTime(json["datetime"]);
        m_unixTime = json["unixtime"];
        m_isDaylightSavingsTime = json["dst"];
        m_dstOffsetMinutes = static_cast<int>(json["dst_offset"]) / 60;
        if (m_isDaylightSavingsTime)
        {
            m_dstChangeDateTime = ParseISO8601DateTime(json["dst_until"]);
        }
        else
        {
            m_dstChangeDateTime = ParseISO8601DateTime(json["dst_from"]);
        }

        return true;
    }

    bool WorldTimeApiClock::IsDaylightSavings()
    {
        return m_isDaylightSavingsTime;
    }

    byte WorldTimeApiClock::GetDaylightSavingsOffset()
    {
        return m_dstOffsetMinutes;
    }

    DateTime WorldTimeApiClock::GetDateTime()
    {
        return m_dateTime;
    }

    time_t WorldTimeApiClock::GetUnixTime()
    {
        return m_unixTime;
    }

    DateTime WorldTimeApiClock::GetDaylightSavingsChangeDateTime()
    {
        return m_dstChangeDateTime;
    }
} // namespace InternetClock
