#pragma once

#include "common.h"

namespace jsh {

	class Time {

		float m_Seconds;

	public:
		Time();
		Time(float sec);

		inline const float GetSecondsf() const noexcept { return m_Seconds; }
		inline const uint32 GetSecondsi() const noexcept { return (uint32) m_Seconds; }
		inline const float GetMilisf() const noexcept { return m_Seconds / 1000.f; }
		inline const uint32 GetMilisi() const noexcept { return (uint32)(m_Seconds / 1000.f); }

		inline void operator+=(const Time f) { m_Seconds += f.GetSecondsf(); }
		
		inline operator float() const noexcept { return m_Seconds; }

	};

	struct Date {
		uint32 year;
		uint8 month;
		uint8 day;
		uint8 hour;
		uint8 minute;
		uint8 second;

		std::string ToString() const noexcept;
	};

}

namespace jshTimer {

#ifdef JSH_ENGINE
	void Initialize();
#endif 

	jsh::Time Now();
	jsh::Time TimeFrom(jsh::Time& time);

	jsh::Date GetDate();

}

namespace jsh {
	class PerformanceTest {

		struct PerformanceData {
			Time beginTime = 0.f;
			Time currentTime = 0.f;
		};

		std::vector<PerformanceData> m_Data;

	public:
		PerformanceTest() {}

		inline void Begin(uint32 ID) noexcept
		{
			while (m_Data.size() <= ID) {
				m_Data.emplace_back();
			}
			m_Data[ID].beginTime = jshTimer::Now();
		}
		inline void End(uint32 ID) noexcept
		{
			PerformanceData& data = m_Data[ID];
			data.currentTime = jshTimer::TimeFrom(data.beginTime);
		}

		inline Time Get(uint32 ID) noexcept
		{
			return m_Data[ID].currentTime;
		}

	};
}