#include "RealTimeClock.h"

#include <unity.h>
#include <single_header/standalone/fakeit.hpp>

using namespace fakeit;

using InternetClock::IInternetClock;
using HardwareClock::IRTC;

namespace
{
DateTime initalDateTime(24, 12, 19, 11, 29, 13);
time_t initalUnixTime = 1734607840;
DateTime daylightSavingsChangeForward(25, 3, 30, 1, 0, 0);
DateTime afterDaylightSavingsChangeForward(25, 3, 30, 1, 0, 10);
DateTime daylightSavingsChangeBackward(25, 10, 26, 2, 0, 0);
DateTime afterDaylightSavingsChangeBackward(25, 10, 26, 2, 0, 10);
}

void setup()
{
}

void teardown()
{
}

void test_RealTimeClock_Initialise()
{
	Mock<IInternetClock> internetClockMock;
	Mock<IRTC> rtcMock;
	RealTimeClock::RealTimeClock rtc(rtcMock.get(), internetClockMock.get());
    When(Method(rtcMock, Begin)).Return(true);
    When(Method(rtcMock, GetDateTime)).Return(initalDateTime);
    When(Method(rtcMock, GetUnixTime)).Return(initalUnixTime);
    When(Method(rtcMock, IsDaylightSaving)).Return(false);
    When(Method(rtcMock, GetDaylightSavingOffset)).Return(60);
    When(Method(rtcMock, GetDaylightSavingsChangeDateTime)).Return(daylightSavingsChangeForward);

    When(Method(internetClockMock, GetInternetDateTime)).Return(false);

    rtc.Initialise();

    Verify(Method(rtcMock, GetDateTime)).Once();
    Verify(Method(internetClockMock, GetInternetDateTime)).Once();
}

void test_RealTimeClock_CheckDaylightSavingsTime_FromInternet()
{
	Mock<IInternetClock> internetClockMock;
	Mock<IRTC> rtcMock;
	RealTimeClock::RealTimeClock rtc(rtcMock.get(), internetClockMock.get());

	When(Method(internetClockMock, GetInternetDateTime)).Return(true);

	When(Method(internetClockMock, GetDateTime)).Return(initalDateTime);
	When(Method(internetClockMock, IsDaylightSavings)).Return(false);
	When(Method(internetClockMock, GetDaylightSavingsOffset)).Return(60);
	When(Method(internetClockMock, GetDaylightSavingsChangeDateTime)).Return(daylightSavingsChangeForward);
	When(Method(internetClockMock, GetUnixTime)).Return(initalUnixTime);

	When(Method(rtcMock, SaveSettings)).AlwaysReturn();

	rtc.CheckDaylightSavingsTime();

	Verify(Method(rtcMock, SaveSettings).Using(_, false, 60, _));
}

void test_RealTimeClock_CheckDaylightSavingsTime_FromRTC_Forward()
{
	Mock<IInternetClock> internetClockMock;
	Mock<IRTC> rtcMock;
	RealTimeClock::RealTimeClock rtc(rtcMock.get(), internetClockMock.get());

    When(Method(rtcMock, Begin)).Return(true);
    When(Method(rtcMock, GetDateTime)).Return(afterDaylightSavingsChangeForward);
    When(Method(rtcMock, GetUnixTime)).Return(initalUnixTime);
    When(Method(rtcMock, IsDaylightSaving)).Return(false);
    When(Method(rtcMock, GetDaylightSavingOffset)).Return(60);
    When(Method(rtcMock, GetDaylightSavingsChangeDateTime)).Return(daylightSavingsChangeForward);

	When(Method(internetClockMock, GetInternetDateTime)).AlwaysReturn(false);

	When(Method(rtcMock, SetTime)).Do([](DateTime const& datetime)
		{
			DateTime expectedDateTime(25, 3, 30, 2, 0, 10);
			TEST_ASSERT_EQUAL(expectedDateTime.Year, datetime.Year);
			TEST_ASSERT_EQUAL(expectedDateTime.Month, datetime.Month);
			TEST_ASSERT_EQUAL(expectedDateTime.Date, datetime.Date);
			TEST_ASSERT_EQUAL(expectedDateTime.Hour, datetime.Hour);
			TEST_ASSERT_EQUAL(expectedDateTime.Minute, datetime.Minute);
			TEST_ASSERT_EQUAL(expectedDateTime.Second, datetime.Second);
		});

	When(Method(rtcMock, SetDaylightSavings)).AlwaysReturn();

	rtc.Initialise();
	rtc.CheckDaylightSavingsTime();

	Verify(Method(rtcMock, SetDaylightSavings).Using(true));
}

void test_RealTimeClock_CheckDaylightSavingsTime_FromRTC_Backward()
{
	Mock<IInternetClock> internetClockMock;
	Mock<IRTC> rtcMock;
	RealTimeClock::RealTimeClock rtc(rtcMock.get(), internetClockMock.get());

    When(Method(rtcMock, Begin)).Return(true);
    When(Method(rtcMock, GetDateTime)).Return(afterDaylightSavingsChangeBackward);
    When(Method(rtcMock, GetUnixTime)).Return(initalUnixTime);
    When(Method(rtcMock, IsDaylightSaving)).Return(true);
    When(Method(rtcMock, GetDaylightSavingOffset)).Return(60);
    When(Method(rtcMock, GetDaylightSavingsChangeDateTime)).Return(daylightSavingsChangeBackward);

	When(Method(internetClockMock, GetInternetDateTime)).AlwaysReturn(false);

	When(Method(rtcMock, SetTime)).Do([](DateTime const& datetime)
		{
			DateTime expectedDateTime(25, 10, 26, 1, 0, 10);
			TEST_ASSERT_EQUAL(expectedDateTime.Year, datetime.Year);
			TEST_ASSERT_EQUAL(expectedDateTime.Month, datetime.Month);
			TEST_ASSERT_EQUAL(expectedDateTime.Date, datetime.Date);
			TEST_ASSERT_EQUAL(expectedDateTime.Hour, datetime.Hour);
			TEST_ASSERT_EQUAL(expectedDateTime.Minute, datetime.Minute);
			TEST_ASSERT_EQUAL(expectedDateTime.Second, datetime.Second);
		});

	When(Method(rtcMock, SetDaylightSavings)).AlwaysReturn();

	rtc.Initialise();
	rtc.CheckDaylightSavingsTime();

	Verify(Method(rtcMock, SetDaylightSavings).Using(false));
}

int main()
{
 	UNITY_BEGIN();
  	RUN_TEST(test_RealTimeClock_Initialise);
	RUN_TEST(test_RealTimeClock_CheckDaylightSavingsTime_FromInternet);
	RUN_TEST(test_RealTimeClock_CheckDaylightSavingsTime_FromRTC_Forward);
	RUN_TEST(test_RealTimeClock_CheckDaylightSavingsTime_FromRTC_Backward);
  	return UNITY_END();
}
