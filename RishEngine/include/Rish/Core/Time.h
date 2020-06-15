/**
 * @file Time.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Time related classes
 * @date 2020-06-15
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

/**
 * @brief Time Class
 */
class Time
{
public:
    /**
     * @brief Construct a new Time object
     * @param time Time in seconds.
     */
    Time(float time=0.0f) : m_time(time)
    {
        m_clock_start = std::chrono::steady_clock::now();
    }

    /**
     * @brief The time from the application create until now
     * 
     * @return float 
     */
    static float now()
    {
        auto clock_end = std::chrono::steady_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(clock_end - m_clock_start);
        return time_span.count();
    }

    /**
     * @brief Cast the Time to float
     * @return float Cast in to float
     */
    operator float() const { return m_time; }

    /**
     * @brief Time operator+
     * @code
     * Time a = 3.f, b = 7.f;
     * Time tmp = a + b;
     * // tmp = 10.f
     * @endcode
     *
     * @param lhs Time of the left hand side
     * @param rhs Time of the right hand side
     * @return Time lhs + rhs
     */
    friend Time operator+(const Time &lhs, const Time &rhs)
    {
        Time tmp;
        tmp += lhs;
        tmp += rhs;
        return tmp;
    }

    /**
     * @brief Time operator+=
     * @code
     * Time a, b = 10.f;
     * a += b;
     * // a = 10.f
     * @endcode
     *
     * @param other Time of the right side
     * @return Time& *this after addition
     */
    Time& operator+=(const Time &other)
    {
        m_time += other.m_time;
        return *this;
    }

    /**
     * @brief Return the time as seconds.
     * @return float Time in seconds.
     */
    float asSeconds() const { return m_time; }

    /**
     * @brief Return the time as miliseconds.
     * @return float Time in miliseconds
     */
    float asMiliseconds() const { return m_time * 1000.0f; }

    /**
     * @brief Return the time as microseconds.
     * @return float Time in microseconds
     */
    float asMicroseconds() const { return m_time * 1000000.0f; }

private:
    float m_time = 0.0f;
    static std::chrono::steady_clock::time_point m_clock_start;
public:
    friend std::ostream& operator<<(std::ostream& os, Time &t)
    {
        return os << std::fixed << std::setprecision(4) << t.m_time;
    }
};

/**
 * @brief
 * @todo Implement
 */
class Clock
{
public:
    Clock();

    Time getElapsedTime()
    {
        // A function provide getCurrentTime that return Time
    }

    Time restart()
    {
        Time now; // A function provide getCurrentTime() that return Time
        Time elapsed = now - m_startTime;
        m_startTime = now;
        return now;
    }

private:
    Time m_startTime;
};

/**
 * @brief
 * @todo templace class for customizing callback function
 */
class Timer
{

};

}
