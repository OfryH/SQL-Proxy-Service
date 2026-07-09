#include "DatabaseConfig.h"
#include <fstream>
#include <sstream>
#include "../logger/Logger.h"
#include "../utils/StringUtils.h"

bool DatabaseConfig::load(const std::string &path)
{

    std::ifstream file(path);
    Logger::info("loading database configuration from " + path);

    if (!file.is_open())
    {
        Logger::error("Failed to open config file: " + path);
        return false;
    }
    Logger::info("database file was opened successfully");

    std::string line;

    while (std::getline(file, line))
    {
        auto pos = line.find('=');

        if (pos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        trim(key);
        trim(value);

        if (key == "host")
            host = value;
        else if (key == "port")
            port = std::stoi(value);
        else if (key == "user")
            user = value;
        else if (key == "password")
            password = value;
        else if (key == "database")
            database = value;
        else if (key == "LOG_FILE")
            logFile = value;

        else if (key == "POLICY_FILE")
            policyFile = value;
    }

    return true;
}
