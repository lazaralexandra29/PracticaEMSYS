#include "app/logger.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h>
#include <string.h>

Logger* Logger::instance_ = nullptr;

Logger* Logger::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Logger();
    }
    return instance_;
}

void Logger::SetInstance(Logger* instance)
{
    instance_ = instance;
}

void Logger::Log(LogLevel level, const char* message)
{
    if (message == nullptr)
    {
        return;
    }

    const char* prefix = GetLogLevelPrefix(level);
    char log_buffer[128];

    snprintf(log_buffer, sizeof(log_buffer), "[%s] %s\r\n", prefix, message);
    UsartDriver::send(log_buffer);
}

void Logger::LogInfo(const char* message)
{
    Log(LogLevel::INFO, message);
}

void Logger::LogError(const char* message)
{
    Log(LogLevel::ERROR, message);
}

void Logger::LogWarning(const char* message)
{
    Log(LogLevel::WARNING, message);
}

void Logger::LogDebug(const char* message)
{
    Log(LogLevel::DEBUG, message);
}

const char* Logger::GetLogLevelPrefix(LogLevel level) const
{
    switch (level)
    {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "LOG";
    }
}

