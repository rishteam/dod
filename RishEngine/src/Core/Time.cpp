#include "Rish/Core/Time.h"

namespace rl {

std::chrono::steady_clock::time_point Time::m_clock_start = std::chrono::steady_clock::now();

Time Time::Now()
{
	auto clock_end = std::chrono::steady_clock::now();
	std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(clock_end - m_clock_start);
	return Time(time_span.count());
}

Time Clock::getElapsedTime()
{
	return Time::Now() - m_startTime;
}

Time Clock::restart()
{
	Time now = Time::Now();
	Time elapsed = now - m_startTime;
	m_startTime = now;
	return elapsed;
}

Timer::Timer(Time &t, TimerCallback callback)
{
	m_t = t;
	m_timerCallback = callback;
}

void Timer::start()
{
	m_start = Time::Now();
}

void Timer::AddTimer(Timer &t)
{
	m_timerList.push_back(t);
}

void Timer::AddLoopTimer(Timer &t)
{
	m_loopTimerList.push_back(t);
}

void Timer::Update()
{
	for(auto &timer: m_timerList)
	{
		if(Time::Now() - timer.m_start >= timer.m_t)
		{
			
		}
	}

	for(auto &timer: m_loopTimerList)
	{
		if(Time::Now() - timer.m_start >= timer.m_t)
		{

		}
	}
}

}