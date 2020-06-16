#include "Rish/Core/Time.h"

namespace rl {

std::chrono::steady_clock::time_point Time::m_clock_start = std::chrono::steady_clock::now();

Time Time::now()
{
	auto clock_end = std::chrono::steady_clock::now();
	std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(clock_end - m_clock_start);
	return Time(time_span.count());
}

Time Clock::getElapsedTime()
{
	return Time::now() - m_startTime;
}

Time Clock::restart()
{
	Time now = Time::now();
	Time elapsed = now - m_startTime;
	m_startTime = now;
	return elapsed;
}

}