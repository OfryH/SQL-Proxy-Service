#include "Logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>

void Logger::info(const std::string& message)
{
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message)
{
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message)
{
    log(LogLevel::ERROR, message);
}

void Logger::log(
    LogLevel level,
    const std::string& message)
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::string levelText;

    switch (level)
    {
    case LogLevel::INFO:
        levelText = "INFO";
        break;

    case LogLevel::WARNING:
        levelText = "WARNING";
        break;

    case LogLevel::ERROR:
        levelText = "ERROR";
        break;
    }

    std::cout
        << "[" << std::put_time(std::localtime(&time), "%F %T") << "] "
        << "[" << levelText << "] "
        << message
        << '\n';
}