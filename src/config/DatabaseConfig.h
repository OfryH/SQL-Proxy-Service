#pragma once

#include <string>

struct DatabaseConfig
{
    std::string host;
    int port;
    std::string user;
    std::string password;
    std::string database;

    std::string policyFile;
    std::string logFile;

    bool load(const std::string& path);
};