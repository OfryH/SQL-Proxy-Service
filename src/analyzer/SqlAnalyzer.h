#pragma once
#include <string>
#include <vector>

// Represents the type of SQL statement we support
enum class StatementType {
    SELECT,
    DML,
    DDL
};

// This struct is the output of the analyzer.
struct AnalysisResult {
    StatementType type;

    std::string rawQuery;

    // Tables referenced in the query
    std::vector<std::string> tables;

    // Columns involved
    std::vector<std::string> columns;
};

class SqlAnalyzer {
public:
    AnalysisResult analyze(const std::string& sql);

private:
    // Utility: convert SQL to uppercase for case-insensitive parsing
    std::string toUpper(const std::string& s);

    StatementType detectType(const std::string& sql);

    // Separate extractors per SQL type
    std::vector<std::string> extractTablesSelect(const std::string& sql);
    std::vector<std::string> extractColumnsSelect(const std::string& sql);

    std::vector<std::string> extractTablesDml(const std::string& sql);
    std::vector<std::string> extractColumnsDml(const std::string& sql);

    std::vector<std::string> extractTablesDdl(const std::string& sql);
};