#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

struct ProfileResult
{
    std::string Name;
    long long Start, End;
    uint32_t ThreadID;
};

struct InstrumentationSession
{
    std::string Name;
};

using InstrumentorClock=std::chrono::steady_clock;

class Instrumentor
{
private:
    InstrumentationSession *m_CurrentSession;
    std::ofstream m_OutputStream;
    int m_ProfileCount;
public:
    Instrumentor()
        : m_CurrentSession(nullptr), m_ProfileCount(0)
    {
    }

    void BeginSession(const std::string &name, const std::string &filepath="results.json")
    {
        m_OutputStream.open(filepath);
        WriteHeader();
        m_CurrentSession = new InstrumentationSession{name};
    }

    void EndSession()
    {
        WriteFooter();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount = 0;
    }

    void WriteProfile(const ProfileResult &result)
    {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";

        m_OutputStream.flush();
    }

    void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_OutputStream.flush();
    }

    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    static Instrumentor& Get()
    {
        static Instrumentor instance;
        return instance;
    }
};

class InstrumentationTimer
{
public:
    InstrumentationTimer(const char *name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = InstrumentorClock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = InstrumentorClock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(
                m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(
                endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

        m_Stopped = true;
    }

private:
    const char *m_Name;
    std::chrono::time_point<InstrumentorClock> m_StartTimepoint;
    bool m_Stopped;
};

}

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#ifdef RL_PROFILE
    #define RL_PROFILE_BEGIN_SESSION(name, filepath) rl::Instrumentor::Get().BeginSession(name, filepath)
    #define RL_PROFILE_END_SESSION() rl::Instrumentor::Get().EndSession()
    #define RL_PROFILE_SCOPE(name) rl::InstrumentationTimer TOKENPASTE2(time, __LINE__)(name)
    #define RL_PROFILE_FUNCTION() RL_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
    #define RL_PROFILE_BEGIN_SESSION(x)
    #define RL_PROFILE_END_SESSION()
    #define RL_PROFILE_SCOPE()
    #define RL_PROFILE_FUNCTION()
#endif
