#include "Timer.h"

#include <chrono>

namespace jsh {

	Time::Time() : m_Seconds(0.f) {}
	Time::Time(float sec) : m_Seconds(sec) {}

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

}