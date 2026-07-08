#pragma once

#include <string>
#include <vector>

struct AuditEntry
{
    std::string query;
    std::string statementType;
    std::string statementOperation;
    std::vector<std::string> tables;
    std::vector<std::string> columns;
    std::vector<std::string> piiColumns;
};

class AuditLogger
{
public:
    explicit AuditLogger(const std::string& logFile);

    void log(const AuditEntry& entry);

private:
    std::string logFile_;

    std::string vectorToString(
        const std::vector<std::string>& values
    );
};