#pragma once

#include <string>
#include <vector>

struct QueryResult
{
    bool success = false;
    std::string errorMessage;

    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> rows;
};