/**
 * @file log.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Logger
 * @version 0.1
 * @date 2020-05-16
 */
#pragma once

#include <Rish/rlpch.h>

#include <spdlog/spdlog.h>

#include <Rish/Core/Core.h>

namespace rl {

// level, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##__VA_ARGS__
#define RL_TRACE(...) rl::Logger::GetClientLogger().trace(__VA_ARGS__)
#define RL_INFO(...) rl::Logger::GetClientLogger().info(__VA_ARGS__)
#define RL_WARN(...) rl::Logger::GetClientLogger().warn(__VA_ARGS__)
#define RL_ERROR(...) rl::Logger::GetClientLogger().error(__VA_ARGS__)
#define RL_CRITICAL(...) rl::Logger::GetClientLogger().critical(__VA_ARGS__)

#define RL_CORE_TRACE(...) rl::Logger::GetCoreLogger().trace(__VA_ARGS__)
#define RL_CORE_INFO(...) rl::Logger::GetCoreLogger().info(__VA_ARGS__)
#define RL_CORE_WARN(...) rl::Logger::GetCoreLogger().warn(__VA_ARGS__)
#define RL_CORE_ERROR(...) rl::Logger::GetCoreLogger().error(__VA_ARGS__)
#define RL_CORE_CRITICAL(...) rl::Logger::GetCoreLogger().critical(__VA_ARGS__)

/**
 * @brief Logger class
 */
class Logger
{
public:
    enum LoggerType
    {
        Normal,    //!< Normal Logger
        IgnoreDup  //!< Ignore Duplicate
    };

    /**
     * @brief Initialize the Logger
     * @param type Logger type
     */
    static void Init(LoggerType type);

    /**
     * @brief Get the Core Logger object
     * @return spdlog::logger& Core Logger
     */
    static spdlog::logger& GetCoreLogger() { return *CoreLogger; }

    /**
     * @brief Get the Client Logger object
     * @return spdlog::logger& Client Logger
     */
    static spdlog::logger& GetClientLogger() { return *ClientLogger; }

private:
    /// @brief static Core Logger object
    static std::shared_ptr<spdlog::logger> CoreLogger;
    /// @brief static Client Logger object
    static std::shared_ptr<spdlog::logger> ClientLogger;

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