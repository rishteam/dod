/**
 * @file Time.h
 * @author roy4801 (me@roy4801.tw)
 * @brief Header of Time related classes
 * @date 2020-06-15
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

/**
 * @class rl::Time
 * @ingroip core
 * @brief Time Class
 */
class Time
{
public:
    /**
     * @brief Construct a new Time object
     * @param time Time in seconds.
     */
    Time(float time=0.0f)
        : m_time(time)
    {
    }

    /**
     * @brief The time from the application create until now
     * @return Time
     */
    static Time Now();

    /**
     * @brief Cast the Time to float
     * @return float Cast in to float
     */
    operator float() const { return m_time; }

    /**
     * @brief Time operator+
     * @code{.cpp}
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
     * @brief Equality operator
     * @param rhs other
     * @return *this == rhs
     */
    bool operator==(const Time &rhs) const
    {
        return m_time == rhs.m_time;
    }

    /**
     * @brief Inequality operator
     * @param rhs other
     * @return *this != rhs
     */
    bool operator!=(const Time &rhs) const
    {
        return !(rhs == *this);
    }

    /**
     * @brief Return the time as seconds.
     * @return float Time in seconds.
     */
    float asSeconds() const { return m_time; }

    /**
     * @brief Return the time as milliseconds.
     * @return float Time in milliseconds
     */
    float asMilliseconds() const { return m_time * 1000.0f; }

    /**
     * @brief Return the time as microseconds.
     * @return float Time in microseconds
     */
    float asMicroseconds() const { return m_time * 1000000.0f; }

private:
    /// The time value holds by rl::Time
    float m_time = 0.0f;
    /// Start time_point since the application starts
    static std::chrono::steady_clock::time_point m_clock_start;

public:
    friend std::ostream& operator<<(std::ostream& os, Time &t)
    {
        return os << std::fixed << std::setprecision(4) << t.m_time;
    }
};

/**
 * @class rl::Clock
 * @ingroup core
 * @brief Clock class
 * @details A class that measure the elapsed time
 * 
 * @code{.cpp}
 * rl::Clock clock
 * ...
 * Time t1 = clock.getElapsedTime();
 * ...
 * Time t2 = clock.restart();
 * 
 * @endcode
 */
class Clock
{
public:
    /**
     * @brief Ctor
     */
    Clock()
        : m_startTime(Time::Now())
    {
    }

    /**
     * @brief Get the elapsed time
     * @details This function returns the time elapsed since the last call to restart()
     * @return Time
     */
    Time getElapsedTime();

    /**
     * @brief Restart the clock
     * @details Puts the time counter to now. It also returns the time elapsed since the clock was started
     * @return Time
     */
    Time restart();

private:
    Time m_startTime;
};

/**
 * @class rl::Timer
 * @ingroup core
 * @brief Timer class
 * @details A timer that binds function to a specific time, the function will not run until the times up.<br/>
 * Timer have static timerList and static loopTimerList that stores the timer.
 *
 * <h4>Usage</h4>
 * * Create a Timer
 * @code{.cpp}
 * Timer test(1.f, [](){
 *      RL_TRACE("Task run");
 * });
 * @endcode
 * * Register Timer
 * @code{.cpp}
 * Timer::AddLoopTimer(test, 3); // run for 3 times
 * Timer::AddLoopTimer(test, Timer::TimerPermanent); // run for infinite times
 * @endcode
 *
 * * Add timer on the fly
 * @code{.cpp}
 * Timer::AddTimer(Timer{3.f, [](){
 *     static int a = 0;
 *     RL_TRACE("aaaa = {}", a);
 * }});
 * @endcode
 */
class Timer
{
public:
    using TimerCallback = std::function<void()>;

    enum TimerType
    {
        TimerNone=0, ///< None
        TimerOnce=1, ///< Once
        TimerTwice=2, ///< Twice
        TimerPermanent=-1 ///< Infinite times
    };

    /**
     * @brief Ctor
     * @param t Time in seconds
     * @param callback callback function that will be called when the times up
     */
    Timer(Time t, TimerCallback callback);

    /**
     * @brief restart the timer
     */
    void restart();

    /**
     * @brief Add the timer to the list
     * @details The timer in the timer list only execute once. When the timer is added to the list, it will start timeing.
     * @param t Timer that binds the time and function
     */
    static void AddTimer(Timer t);

    /**
     * @brief Add the timer to the loop list
     * @details The timer in the timer list execute quantity of times. When the timer is added to the list, it will start timing.
     * 
     * @param t Timer that binds the time and function
     * @param times Times that the timer will execute
     */
    static void AddLoopTimer(Timer t, int32_t times=TimerPermanent);

    /**
     * @brief Update method
     * @details Update all the timers in the list
     * 
     */
    static void Update();

    /**
     * @brief Clear all the timer in the timer list
     */
    static void Clear();

private:

    Time m_t;
    Time m_start;
    TimerCallback m_timerCallback;
    int m_loopTimes = 0;

    static std::list<Timer> m_timerList;
    static std::list<Timer> m_loopTimerList;
};

}

