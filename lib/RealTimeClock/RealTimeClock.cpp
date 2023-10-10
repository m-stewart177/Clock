//
// Created by mike on 10/10/23.
//

#include "RealTimeClock.h"

#include <Arduino_JSON.h>

#include <WiFiUtils.h>

#define InternetTimeApi "worldtimeapi.org"
#define InternetTimeApiPort 80

String LocalDateTimeQuery = "/api/ip";

namespace
{
    RealTimeClock::DateTime internetDateTime;
    time_t unixTime;
    bool internetDaylightSavingsTime;
    byte dstOffsetMinutes;
    RealTimeClock::DateTime dstChangeDateTime;

    RealTimeClock::DateTime ParseISO8601DateTime(String iso8601DateTime)
    {
        RealTimeClock::DateTime dateTime;
        dateTime.Year = iso8601DateTime.substring(2, 4).toInt();
        dateTime.Month = iso8601DateTime.substring(5, 7).toInt();
        dateTime.Date = iso8601DateTime.substring(8, 10).toInt();
        dateTime.Hour = iso8601DateTime.substring(11, 13).toInt();
        dateTime.Minute = iso8601DateTime.substring(14, 16).toInt();
        dateTime.Second = iso8601DateTime.substring(17, 19).toInt();

        return dateTime;
    }

    bool GetInternetDateTime()
    {
        JSONVar json = GetJson(InternetTimeApi, InternetTimeApiPort, LocalDateTimeQuery);
        if (json.length() == 0)
        {
            return false;
        }

        internetDateTime = ParseISO8601DateTime(json["datetime"]);
        unixTime = json["unixtime"];
        internetDaylightSavingsTime = json["dst"];
        dstOffsetMinutes = (int) json["dst_offset"] / 60;
        if (internetDaylightSavingsTime)
        {
            dstChangeDateTime = ParseISO8601DateTime(json["dst_until"]);
        }
        else
        {
            dstChangeDateTime = ParseISO8601DateTime(json["dst_from"]);
        }

        return true;
    }

} // namespace

namespace RealTimeClock
{

    RealTimeClock::RealTimeClock() :
            m_RTC(),
            m_dateTime(
                    m_RTC.Get_Year(), m_RTC.Get_Month(), m_RTC.Get_Date(),
                    m_RTC.Get_Hour(), m_RTC.Get_Minute(), m_RTC.Get_Second()),
            m_unixTime(m_RTC.Get_UNIX_Time()),
            m_daylightSavingsTime(m_RTC.Read_EEPROM(0x00)),
            m_dstOffsetMinutes(m_RTC.Read_EEPROM(0x01)),
            m_dstChange(
                    m_RTC.Read_EEPROM(0x02), m_RTC.Read_EEPROM(0x03), m_RTC.Read_EEPROM(0x04),
                    m_RTC.Read_EEPROM(0x05), m_RTC.Read_EEPROM(0x06), m_RTC.Read_EEPROM(0x07))
    {}

    void RealTimeClock::Initialise()
    {
        m_RTC.Begin();

        if (GetInternetDateTime())
        {
            if (abs((int) (m_unixTime - unixTime)) > 2)
            {
                UpdateToInternetTime();
            }

            m_daylightSavingsTime = internetDaylightSavingsTime;
        }
    }

    void RealTimeClock::CheckDaylightSavingsTime()
    {
        if (m_dstChange == DateTime() && (m_dateTime > m_dstChange))
        {
            if (GetInternetDateTime())
            {
                if (m_daylightSavingsTime != internetDaylightSavingsTime)
                {
                    UpdateToInternetTime();
                }
            }
            else
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
                m_RTC.Set_Time(
                        m_dateTime.Second, m_dateTime.Minute, m_dateTime.Hour,
                        m_dateTime.Date, m_dateTime.Month, m_dateTime.Year);
                m_daylightSavingsTime = ! m_daylightSavingsTime;
                m_RTC.Write_EEPROM(0x00, m_daylightSavingsTime);
            }
        }
    }

    DateTime &RealTimeClock::GetDateTime()
    {
        m_dateTime = DateTime(
                m_RTC.Get_Year(), m_RTC.Get_Month(), m_RTC.Get_Date(),
                m_RTC.Get_Hour(), m_RTC.Get_Minute(), m_RTC.Get_Second());

        return m_dateTime;
    }

    time_t RealTimeClock::GetUnixTime()
    {
        m_unixTime = m_RTC.Get_UNIX_Time();

        return m_unixTime;
    }

    void RealTimeClock::UpdateToInternetTime()
    {
        m_RTC.Set_Time(
                internetDateTime.Second, internetDateTime.Minute, internetDateTime.Hour,
                internetDateTime.Date, internetDateTime.Month, internetDateTime.Year);
        m_RTC.Update_Time_Stamp();
        m_RTC.Set_UNIX_Time(unixTime);
        m_RTC.Write_EEPROM(0x00, internetDaylightSavingsTime);
        m_RTC.Write_EEPROM(0x01, dstOffsetMinutes);
        m_RTC.Write_EEPROM(0x02, dstChangeDateTime.Year);
        m_RTC.Write_EEPROM(0x03, dstChangeDateTime.Month);
        m_RTC.Write_EEPROM(0x04, dstChangeDateTime.Date);
        m_RTC.Write_EEPROM(0x05, dstChangeDateTime.Hour);
        m_RTC.Write_EEPROM(0x06, dstChangeDateTime.Minute);
        m_RTC.Write_EEPROM(0x07, dstChangeDateTime.Second);

        m_dstChange = dstChangeDateTime;
        m_dstOffsetMinutes = dstOffsetMinutes;
        m_unixTime = unixTime;
    }
} // RealTimeClock