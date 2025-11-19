#ifndef LOGGER_HPP
#define LOGGER_HPP

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

class Logger : public ILogger
{
public:
    
    static Logger* GetInstance();

    static void SetInstance(Logger* instance);

    void Log(LogLevel level, const char* message) override;
    void LogInfo(const char* message) override;
    void LogError(const char* message) override;
    void LogWarning(const char* message) override;
    void LogDebug(const char* message) override;

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* instance_;
    const char* GetLogLevelPrefix(LogLevel level) const;
};

#endif


