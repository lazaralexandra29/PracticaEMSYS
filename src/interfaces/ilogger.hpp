#ifndef INTERFACES_ILOGGER_HPP
#define INTERFACES_ILOGGER_HPP

#include <stdint.h>

enum class LogLevel : uint8_t
{
    INFO = 0,
    ERROR = 1,
    WARNING = 2,
    DEBUG = 3
};

class ILogger
{
public:
    virtual ~ILogger() = default;

    virtual void Log(LogLevel level, const char* message) = 0;
    virtual void LogInfo(const char* message) = 0;
    virtual void LogError(const char* message) = 0;
    virtual void LogWarning(const char* message) = 0;
    virtual void LogDebug(const char* message) = 0;
};

#endif





