#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "interfaces/ilogger.hpp"

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