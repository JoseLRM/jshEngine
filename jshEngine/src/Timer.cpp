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

	std::chrono::steady_clock::time_point initTime;
	std::chrono::steady_clock::time_point capturedTime;

	void Initialize()
	{
		initTime = NowFromChrono();
		capturedTime = NowFromChrono();
	}

	Time Now() {
		return Time(std::chrono::duration<float>(NowFromChrono() - initTime).count());
	}

	Time Capture()
	{
		auto lastCapture = capturedTime;
		capturedTime = NowFromChrono();
		return Time(std::chrono::duration<float>(NowFromChrono() - lastCapture).count());
	}

}