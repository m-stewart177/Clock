#include "RealTimeClock.h"

#define InternetTimeApi "worldtimeapi.org"
#define InternetTimeApiPort 80

String LocalDateTimeQuery = "/api/ip";

namespace RealTimeClock
{
    RealTimeClock::RealTimeClock(IRTC& rtc, IInternetClock& internetClock) :
        m_RTC(rtc),
        m_internetClock(internetClock),
        m_dateTime(),
        m_unixTime(),
        m_daylightSavingsTime(),
        m_dstOffsetMinutes(),
        m_dstChange()
    {
    }

    void RealTimeClock::Initialise()
    {
        if (!m_RTC.Begin())
        {
            Serial.println("Failed to initialize RTC");
        }

        m_dateTime = m_RTC.GetDateTime();
        m_unixTime = m_RTC.GetUnixTime();
        m_daylightSavingsTime = m_RTC.IsDaylightSaving();
        m_dstOffsetMinutes = m_RTC.GetDaylightSavingOffset();
        m_dstChange = m_RTC.GetDaylightSavingsChangeDateTime();

        if (m_internetClock.GetInternetDateTime())
        {
            UpdateToInternetTime();

            m_daylightSavingsTime = m_internetClock.IsDaylightSavings();;
        }
    }

    void RealTimeClock::CheckDaylightSavingsTime()
    {
        if (m_internetClock.GetInternetDateTime())
        {
            UpdateToInternetTime();
        }
        else
        {
            if (!(m_dstChange == DateTime()) && (m_dateTime > m_dstChange))
            {
                int offsetMinutes = m_dstOffsetMinutes % 60;
                int offsetHours = m_dstOffsetMinutes / 60;
                if (m_daylightSavingsTime)
                {
                    m_dateTime.Hour -= offsetHours;
                    m_dateTime.Minute -= offsetMinutes;
                }
                else
                {
                    m_dateTime.Hour += offsetHours;
                    m_dateTime.Minute += offsetMinutes;
                }
                m_RTC.SetTime(m_dateTime);
                m_daylightSavingsTime = !m_daylightSavingsTime;
                m_dstChange = DateTime();
                m_RTC.SetDaylightSavings(m_daylightSavingsTime);
            }
        }
    }

    DateTime& RealTimeClock::GetDateTime()
    {
        m_dateTime = m_RTC.GetDateTime();;

        return m_dateTime;
    }

    time_t RealTimeClock::GetUnixTime()
    {
        m_unixTime = m_RTC.GetUnixTime();

        return m_unixTime;
    }

    String RealTimeClock::TimeStamp()
    {
        return m_RTC.GetTimeStamp();
    }

    void RealTimeClock::UpdateToInternetTime()
    {
        auto const internetDateTime = m_internetClock.GetDateTime();
        auto const internetDaylightSavingsTime = m_internetClock.IsDaylightSavings();
        auto const dstOffsetMinutes = m_internetClock.GetDaylightSavingsOffset();
        auto const dstChangeDateTime = m_internetClock.GetDaylightSavingsChangeDateTime();
        auto const unixTime = m_internetClock.GetUnixTime();

        m_RTC.SaveSettings(internetDateTime, internetDaylightSavingsTime, dstOffsetMinutes, dstChangeDateTime);
        m_dstChange = dstChangeDateTime;
        m_dstOffsetMinutes = dstOffsetMinutes;
        m_daylightSavingsTime = internetDaylightSavingsTime;
        m_dateTime = internetDateTime;
        m_unixTime = unixTime;
    }
} // RealTimeClock
