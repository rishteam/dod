#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

struct ProfileResult
{
    std::string Name;
    long long Start, End;
    std::thread::id ThreadID;
};

struct InstrumentationSession
{
    std::string Name;
};

using InstrumentorClock=std::chrono::steady_clock;

class Instrumentor
{
private:
    std::mutex m_Mutex;
    InstrumentationSession *m_CurrentSession;
    std::ofstream m_OutputStream;
public:
    Instrumentor()
        : m_CurrentSession(nullptr)
    {
    }

    void BeginSession(const std::string &name, const std::string &filepath="results.json")
    {
        std::lock_guard lock(m_Mutex);

        RL_CORE_ASSERT(m_CurrentSession == nullptr, "Instrumentor::BeginSession(): already has a session.");

        m_OutputStream.open(filepath);
        WriteHeader();
        m_CurrentSession = new InstrumentationSession{name};
    }

    void EndSession()
    {
        std::lock_guard lock(m_Mutex);
        //
        WriteFooter();
        m_OutputStream.close();
        // delete session
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
    }

    void WriteProfile(const ProfileResult &result)
    {
        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');
        std::stringstream profileRec;

        profileRec << ",{";
        profileRec << "\"cat\":\"function\",";
        profileRec << "\"dur\":" << (result.End - result.Start) << ',';
        profileRec << "\"name\":\"" << name << "\",";
        profileRec << "\"ph\":\"X\",";
        profileRec << "\"pid\":0,";
        profileRec << "\"tid\":" << result.ThreadID << ",";
        profileRec << "\"ts\":" << result.Start;
        profileRec << "}";

        std::lock_guard lock(m_Mutex);
        if(m_CurrentSession)
        {
            m_OutputStream << profileRec.str();
            m_OutputStream.flush();
        }
    }

    void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
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

//        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().WriteProfile({m_Name, start, end, std::this_thread::get_id()});

        m_Stopped = true;
    }

private:
    const char *m_Name;
    std::chrono::time_point<InstrumentorClock> m_StartTimepoint;
    bool m_Stopped;
};

}

// TODO: support multithreading

// https://stackoverflow.com/questions/1597007/creating-c-macro-with-and-line-token-concatenation-with-positioning-macr
// https://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#ifdef RL_PROFILE
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define RL_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define RL_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
		#define RL_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define RL_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define RL_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define RL_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define RL_FUNC_SIG __func__
	#else
		#define RL_FUNC_SIG "RL_FUNC_SIG unknown!"
	#endif

    #define RL_PROFILE_BEGIN_SESSION(name, filepath) rl::Instrumentor::Get().BeginSession(name, filepath)
    #define RL_PROFILE_END_SESSION() rl::Instrumentor::Get().EndSession()
    #define RL_PROFILE_SCOPE(name) rl::InstrumentationTimer TOKENPASTE2(time, __LINE__)(name)
    #define RL_PROFILE_FUNCTION() RL_PROFILE_SCOPE(RL_FUNC_SIG)

    #if RL_PROFILE_RENDERER
        #define RL_PROFILE_RENDERER_FUNCTION() RL_PROFILE_FUNCTION()
        #define RL_PROFILE_RENDERER_SCOPE(name) RL_PROFILE_SCOPE(name)
    #else
        #define RL_PROFILE_RENDERER_FUNCTION()
        #define RL_PROFILE_RENDERER_SCOPE(name)
    #endif
#else
    #define RL_PROFILE_BEGIN_SESSION(name, filepath)
    #define RL_PROFILE_END_SESSION()
    #define RL_PROFILE_SCOPE(name)
    #define RL_PROFILE_FUNCTION()
#endif
