#include "AuditLogger.h"
#include <iostream>
#include <filesystem>

#include <fstream>

AuditLogger::AuditLogger(const std::string& logFile)
    : logFile_(logFile)
{
}

void AuditLogger::log(const AuditEntry& entry)
{
    std::ofstream out(logFile_, std::ios::app);

    if (!out.is_open())
    {
        std::cout << "Failed opening audit file\n";
        return;
    }

    out << "========================================\n";

    out << "Query: "
        << entry.query
        << "\n";

    out << "Type: "
        << entry.statementType
        << "\n";

    out << "Operation: "
        << entry.statementOperation
        << "\n";

    out << "Tables: "
        << vectorToString(entry.tables)
        << "\n";

    out << "Columns: "
        << vectorToString(entry.columns)
        << "\n";

    out << "PII Columns: "
        << vectorToString(entry.piiColumns)
        << "\n";

    out << "========================================\n\n";
}

std::string AuditLogger::vectorToString(
    const std::vector<std::string>& values)
{
    std::string result;

    for (size_t i = 0; i < values.size(); i++)
    {
        result += values[i];

        if (i + 1 < values.size())
        {
            result += ", ";
        }
    }

    return result;
}