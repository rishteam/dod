#include <log.h>

namespace rl {

// static
Logger Logger::logger;
LogLevel Logger::nowLevel = LOG_DEBUG;

const char * Logger::LogName[LogLevel::LOG_LEVEL_NUM] = {
    "NONE", "INFO", "WARN", "ERROR", "DEBUG"
};

}