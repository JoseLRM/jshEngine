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

}

namespace jshTimer {

#ifdef JSH_ENGINE
	void Initialize();
#endif 

	jsh::Time Now();

	jsh::Time Capture();

}