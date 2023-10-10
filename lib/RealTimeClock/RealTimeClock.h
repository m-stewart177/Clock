//
// Created by mike on 10/10/23.
//
#pragma once

#include <Arduino.h>
#include <RV3028.h>

namespace RealTimeClock
{
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
        {}

        byte Year;
        byte Month;
        byte Date;
        byte Hour;
        byte Minute;
        byte Second;

        bool operator>(const DateTime &other)
        {
            if (this->Year > other.Year)
            {
                return true;
            }

            if (this->Year == other.Year && this->Month > other.Month)
            {
                return true;
            }

            if (this->Year == other.Year && this->Month == other.Month && this->Date > other.Date)
            {
                return true;
            }

            if (this->Year == other.Year && this->Month == other.Month && this->Date == other.Date &&
                this->Hour > other.Hour)
            {
                return true;
            }

            if (this->Year == other.Year && this->Month == other.Month && this->Date == other.Date &&
                this->Hour == other.Hour && this->Minute > other.Minute)
            {
                return true;
            }

            if (this->Year == other.Year && this->Month == other.Month && this->Date == other.Date &&
                this->Hour == other.Hour && this->Minute == other.Minute && this->Second > other.Second)
            {
                return true;
            }

            return false;
        }

        bool operator==(DateTime const& other)
        {
            if (this->Year != other.Year)
            {
                return false;
            }

            if (this->Month != other.Month)
            {
                return false;
            }

            if (this->Date != other.Date)
            {
                return false;
            }

            if (this->Hour != other.Hour)
            {
                return false;
            }

            if (this->Minute != other.Minute)
            {
                return false;
            }

            if (this->Second != other.Second)
            {
                return false;
            }

            return true;
        }
    };

    class RealTimeClock
    {
    public:
        RealTimeClock();

    public:
        void Initialise();

        void CheckDaylightSavingsTime();

        DateTime &GetDateTime();

        time_t GetUnixTime();

    private:
        void UpdateToInternetTime();

    private:
        RV3028 m_RTC;
        DateTime m_dateTime;
        time_t m_unixTime;
        bool m_daylightSavingsTime;
        byte m_dstOffsetMinutes;
        DateTime m_dstChange;
    };

} // RealTimeClock
