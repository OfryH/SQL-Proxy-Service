#pragma once

#include <string>
#include <vector>

struct AuditEntry
{
    std::string timestamp;

    std::string query;
    std::string statementType;
    std::string statementOperation;

    std::vector<std::string> tables;
    std::vector<std::string> columns;

    std::vector<std::string> piiColumns;

    std::string status;
    std::string errorMessage;

    int rowsAffected = 0;
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