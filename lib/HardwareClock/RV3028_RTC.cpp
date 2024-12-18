#include "RV3028_RTC.h"

namespace
{
    byte constexpr DaylightSavingsAddress = 0x00;
    byte constexpr DaylightSavingsOffsetAddress = 0x01;
    byte constexpr DaylightSavingsChangeYearAddress = 0x02;
    byte constexpr DaylightSavingsChangeMonthAddress = 0x03;
    byte constexpr DaylightSavingsChangeDateAddress = 0x04;
    byte constexpr DaylightSavingsChangeHourAddress = 0x05;
    byte constexpr DaylightSavingsChangeMinuteAddress = 0x06;
    byte constexpr DaylightSavingsChangeSecondAddress = 0x07;
} // namespace

namespace HardwareClock
{
    RV3028_RTC::RV3028_RTC() :
        m_RTC(),
        m_unixTime()
    {
    }

    bool RV3028_RTC::Begin()
    {
        return m_RTC.Begin();
    }

    byte RV3028_RTC::GetSeconds() const
    {
        return m_RTC.Get_Second();
    }

    byte RV3028_RTC::GetMinutes() const
    {
        return m_RTC.Get_Minute();
    }

    byte RV3028_RTC::GetHours() const
    {
        return m_RTC.Get_Hour();
    }

    byte RV3028_RTC::GetWeekDay() const
    {
        return m_RTC.Get_Week_Day();
    }

    byte RV3028_RTC::GetDay() const
    {
        return m_RTC.Get_Date();
    }

    byte RV3028_RTC::GetMonth() const
    {
        return m_RTC.Get_Month();
    }

    byte RV3028_RTC::GetYear() const
    {
        return m_RTC.Get_Year();
    }

    DateTime RV3028_RTC::GetDateTime() const
    {
        return {
            m_RTC.Get_Year(), m_RTC.Get_Month(), m_RTC.Get_Date(),
            m_RTC.Get_Hour(), m_RTC.Get_Minute(), m_RTC.Get_Second()
        };
    }

    DateTime RV3028_RTC::GetDaylightSavingsChangeDateTime() const
    {
        return {
            m_RTC.Read_EEPROM(DaylightSavingsChangeYearAddress),
            m_RTC.Read_EEPROM(DaylightSavingsChangeMonthAddress),
            m_RTC.Read_EEPROM(DaylightSavingsChangeDateAddress),
            m_RTC.Read_EEPROM(DaylightSavingsChangeHourAddress),
            m_RTC.Read_EEPROM(DaylightSavingsChangeMinuteAddress),
            m_RTC.Read_EEPROM(DaylightSavingsChangeSecondAddress)
        };
    }

    bool RV3028_RTC::IsDaylightSaving() const
    {
        return static_cast<bool>(m_RTC.Read_EEPROM(DaylightSavingsAddress));
    }

    byte RV3028_RTC::GetDaylightSavingOffset() const
    {
        return m_RTC.Read_EEPROM(DaylightSavingsOffsetAddress);
    }

    time_t RV3028_RTC::GetUnixTime() const
    {
        return m_unixTime + static_cast<time_t>(m_RTC.Get_UNIX_Time());
    }

    void RV3028_RTC::SetUnixTime(time_t unixTime)
    {
        m_RTC.Clear_UNIX_Time();
        m_unixTime = unixTime;
    }

    String RV3028_RTC::GetTimeStamp() const
    {
        m_RTC.Update_Time_Stamp();

        return m_RTC.Time_Stamp;
    }

    void RV3028_RTC::SaveSettings(DateTime const& datetime, bool const daylightSaving,
                                  byte const dstOffsetMinutes, DateTime const& dstChangeDateTime)
    {
        m_RTC.Set_Time(datetime.Second, datetime.Minute, datetime.Hour,
                       datetime.Date, datetime.Month, datetime.Year);
        m_RTC.Update_Time_Stamp();

        m_RTC.Write_EEPROM(DaylightSavingsAddress, daylightSaving);
        m_RTC.Write_EEPROM(DaylightSavingsOffsetAddress, dstOffsetMinutes);
        m_RTC.Write_EEPROM(DaylightSavingsChangeYearAddress, dstChangeDateTime.Year);
        m_RTC.Write_EEPROM(DaylightSavingsChangeMonthAddress, dstChangeDateTime.Month);
        m_RTC.Write_EEPROM(DaylightSavingsChangeDateAddress, dstChangeDateTime.Date);
        m_RTC.Write_EEPROM(DaylightSavingsChangeHourAddress, dstChangeDateTime.Hour);
        m_RTC.Write_EEPROM(DaylightSavingsChangeMinuteAddress, dstChangeDateTime.Minute);
        m_RTC.Write_EEPROM(DaylightSavingsChangeSecondAddress, dstChangeDateTime.Second);
    }

    void RV3028_RTC::SetTime(DateTime const& datetime)
    {
        m_RTC.Set_Time(datetime.Second, datetime.Minute, datetime.Hour,
                       datetime.Date, datetime.Month, datetime.Year);
    }

    void RV3028_RTC::SetDaylightSavings(bool const daylightSaving)
    {
        m_RTC.Write_EEPROM(DaylightSavingsAddress, daylightSaving);
    }
} // namespace HardwareClock
