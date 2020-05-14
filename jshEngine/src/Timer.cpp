#include "common.h"

#include "Timer.h"

#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <ctime>
#include <time.h>

namespace jsh {

	Time::Time() : m_Seconds(0.f) {}
	Time::Time(float sec) : m_Seconds(sec) {}

	std::string Date::ToString() const noexcept
	{
		std::string s;
		s = std::to_string(day) + '-' + std::to_string(month) + '-' + std::to_string(year) + "(" + std::to_string(hour) + '-' + std::to_string(minute) + '-' + std::to_string(second) + ')';
		return s;
	}

}

using namespace jsh;

static inline std::chrono::steady_clock::time_point NowFromChrono()
{
	return std::chrono::steady_clock::now();
}

namespace jshTimer {

	std::chrono::steady_clock::time_point g_InitTime;

	void Initialize()
	{
		g_InitTime = NowFromChrono();
	}

	Time Now() {
		return Time(std::chrono::duration<float>(NowFromChrono() - g_InitTime).count());
	}

	Time TimeFrom(jsh::Time& time)
	{
		return Now() - time;
	}

	jsh::Date GetDate()
	{
		__time64_t t;
		_time64(&t);
		tm time;
		_localtime64_s(&time, &t);
		
		Date date;
		date.year = time.tm_year + 1900;
		date.month = time.tm_mon + 1;
		date.day = time.tm_mday;
		date.hour = time.tm_hour;
		date.minute = time.tm_min;
		date.second = time.tm_sec;

		return date;
	}

}