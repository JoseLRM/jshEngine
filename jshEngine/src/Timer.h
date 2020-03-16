#pragma once

#include "common.h"

namespace jsh {

	class Time {

		float m_Seconds;

	public:
		Time();
		Time(float sec);

		inline float GetSecondsf() const noexcept { return m_Seconds; }
		inline uint32 GetSecondsi() const noexcept { return m_Seconds; }
		inline float GetMilisf() const noexcept { return m_Seconds / 1000.f; }
		inline uint32 GetMilisi() const noexcept { return m_Seconds / 1000.f; }
		
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