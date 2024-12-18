#pragma once

namespace LedClock
{
    class ILedClock
    {
    public:
        virtual ~ILedClock() = default;
        virtual void InitialiseDisplay() = 0;
        virtual void UpdateTime(int hours, int minutes) = 0;
    };
} // namespace LedClock
