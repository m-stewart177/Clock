#include "DataTypes.h"

bool DateTime::operator>(DateTime const& other) const
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

bool DateTime::operator==(DateTime const& other) const
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
