#pragma once

#include <string>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:

    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static void log(
        LogLevel level,
        const std::string& message
    );
};