/**
 * @file log.h
 * @author roy4801 (me@roy4801.tw)
 * @brief Logger
 * @version 0.1
 * @date 2020-05-16
 */
#pragma once

#include <Rish/rlpch.h>
#include <spdlog/spdlog.h>
#include <Rish/Core/Core.h>

#define IS_BIT_SET(x, b) ((1<<(b)) & (x))

namespace rl {

// level, __FILE__, __LINE__, RL_PRETTY_FUNCTION, format, ##__VA_ARGS__
#define RL_TRACE(...) rl::Logger::ClientLog(rl::Logger::Trace, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_INFO(...) rl::Logger::ClientLog(rl::Logger::Info, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_WARN(...) rl::Logger::ClientLog(rl::Logger::Warn, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_ERROR(...) rl::Logger::ClientLog(rl::Logger::Error, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_CRITICAL(...) rl::Logger::ClientLog(rl::Logger::Critical, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)

#define RL_CORE_TRACE(...) rl::Logger::CoreLog(rl::Logger::Trace, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_CORE_INFO(...) rl::Logger::CoreLog(rl::Logger::Info, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_CORE_WARN(...) rl::Logger::CoreLog(rl::Logger::Warn, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_CORE_ERROR(...) rl::Logger::CoreLog(rl::Logger::Error, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)
#define RL_CORE_CRITICAL(...) rl::Logger::CoreLog(rl::Logger::Critical, __FILE__, __LINE__, RL_PRETTY_FUNCTION, __VA_ARGS__)

// TODO: Use regex to parse the function

/**
 * @brief Logger class
 */
class Logger
{
public:
    enum LoggerType
    {
        Default      = (1<<0), ///< Default
        IgnoreDup    = (1<<1), ///< Ignore Duplicate
        FileInfo     = (1<<2), ///< File
        FunctionInfo = (1<<3)  ///< Function
    };

    enum LoggerLevel
    {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Critical,
        Off
    };

    /**
     * @brief Initialize the Logger
     * @param type Logger type
     */
    static void Init(uint32_t type=Default);

    template<typename ... Args>
    static void CoreLog(LoggerLevel level, const char* file, int line, const char* function,
            const std::string &fmt, Args && ... args)
    {
        auto s = fmt::format(fmt, std::forward<Args>(args)...);
        if(s_type & FunctionInfo)
            s += fmt::format("\n`{}`", function);
        if(s_type & FileInfo)
            s += fmt::format("\n{}:{}\n", file, line);
        CoreLogger->log((spdlog::level::level_enum)level, s);
    }

    template<typename ... Args>
    static void ClientLog(LoggerLevel level, const char* file, int line, const char* function,
            const std::string &fmt, Args && ... args)
    {
        auto s = fmt::format(fmt, std::forward<Args>(args)...);
        if(s_type & FunctionInfo)
            s += fmt::format("\n`{}`", function);
        if(s_type & FileInfo)
            s += fmt::format("\n{}:{}\n", file, line);
        ClientLogger->log((spdlog::level::level_enum)level, s);
    }

private:
    /// @brief static Core Logger object
    static std::shared_ptr<spdlog::logger> CoreLogger;
    /// @brief static Client Logger object
    static std::shared_ptr<spdlog::logger> ClientLogger;

    static uint32_t s_type;
    static std::string s_pattern;

    Logger() = default;
};

/**
 * @brief Remove the project path the full path (Convert to relative path)
 *
 * @param path full path
 * @return const char* reloative path
 */
const char* GetSrcFileRelativePath(const char* path);

}

/**
 * @class rl::Logger
 * @ingroup core
 */
