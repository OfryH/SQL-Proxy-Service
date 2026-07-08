#include "SqlAnalyzer.h"
#include <algorithm>
#include <sstream>

std::string SqlAnalyzer::toUpper(const std::string& s) {
    std::string res = s;
    std::transform(
    res.begin(),
    res.end(),
    res.begin(),
    [](unsigned char c){ return std::toupper(c); }
);
    return res;
}

AnalysisResult SqlAnalyzer::analyze(const std::string& sql) {
    AnalysisResult result;

    std::string upper = toUpper(sql);
    result.operation = detectOperation(upper);
    result.type = detectType(result.operation);

    switch (result.type) {
        case StatementType::SELECT:
            result.tables = extractTablesSelect(sql);
            result.columns = extractColumnsSelect(sql);
            break;

        case StatementType::DML:
            result.tables = extractTablesDml(sql);
            result.columns = extractColumnsDml(sql);
            break;

        case StatementType::DDL:
            result.tables = extractTablesDdl(sql);
            result.columns = {}; // DDL has no meaningful columns
            break;
    }
    result.rawQuery = sql;
    return result;
}

// Determine the type of SQL statement
OperationType SqlAnalyzer::detectOperation(const std::string& upperQuery) {
    if (upperQuery.find("SELECT") == 0) {
        return OperationType::SELECT;
    } else if (upperQuery.find("INSERT") == 0) {
        return OperationType::INSERT;
    } else if (upperQuery.find("UPDATE") == 0) {
        return OperationType::UPDATE;
    } else if (upperQuery.find("DELETE") == 0) {
        return OperationType::DELETE;
    } else if (upperQuery.find("CREATE") == 0 || upperQuery.find("ALTER") == 0 || upperQuery.find("DROP") == 0) {
        return OperationType::DDL;
    } else {
        return OperationType::UNKNOWN;
    }
}
StatementType SqlAnalyzer::detectType(OperationType upperQuery) {
    switch (upperQuery) {
        case OperationType::SELECT:
            return StatementType::SELECT;
        case OperationType::INSERT:
        case OperationType::UPDATE:
        case OperationType::DELETE:
            return StatementType::DML;
        case OperationType::DDL:
            return StatementType::DDL;
        default:
            return StatementType::UNKNOWN;
    }
}

std::vector<std::string> SqlAnalyzer::extractTablesSelect(const std::string& sql){
    std::string upperQuery = toUpper(sql);

    std::vector<std::string> tables;
    // Find the FROM keyword
    size_t fromPos = upperQuery.find("FROM");
    if (fromPos == std::string::npos)
    {
        return tables;
    }

    // Start right after "FROM"
    size_t startPos = fromPos + 4;

    // Skip whitespace
    while (startPos < sql.size() && std::isspace(sql[startPos]))
    {
        startPos++;
    }

    // Find where the table name ends
    size_t endPos = startPos;
    while (endPos < sql.size())
    {
        char c = sql[endPos];

        if (std::isspace(c) || c == ';' || c == ',')
        {
            break;
        }

        endPos++;
    }

    tables.push_back(sql.substr(startPos, endPos - startPos));

    return tables;
}

std::vector<std::string> SqlAnalyzer::extractColumnsSelect(const std::string& sql){
    std::string upperQuery = toUpper(sql);

    std::vector<std::string> columns;
    size_t selectPos = upperQuery.find("SELECT");
    size_t fromPos = upperQuery.find("FROM");

    if (selectPos == std::string::npos || fromPos == std::string::npos)
        return columns;

    // extract original substring)
    std::string rawColumns = sql.substr(selectPos + 6, fromPos - (selectPos + 6));

    std::stringstream ss(rawColumns);
    std::string col;

    while (std::getline(ss, col, ','))
    {
        // trim spaces
        size_t start = col.find_first_not_of(" \t\n\r");
        size_t end = col.find_last_not_of(" \t\n\r");

        if (start == std::string::npos)
            continue;

        std::string cleaned = col.substr(start, end - start + 1);

        // handle *
        if (cleaned == "*")
        {
            columns.clear();
            columns.push_back("*");
            return columns;
        }

        columns.push_back(cleaned);
    }


    return columns;

}

std::vector<std::string> SqlAnalyzer::extractTablesDml(const std::string& sql)
{
    std::vector<std::string> tables;

    std::string upper = toUpper(sql);

    size_t startPos = std::string::npos;

    if (upper.find("INSERT") == 0)
    {
        startPos = upper.find("INTO");
        if (startPos != std::string::npos)
            startPos += 4; // skip "INTO"
    }
    else if (upper.find("UPDATE") == 0)
    {
        startPos = 6; // skip "UPDATE"
    }
    else if (upper.find("DELETE") == 0)
    {
        startPos = upper.find("FROM");
        if (startPos != std::string::npos)
            startPos += 4; // skip "FROM"
    }

    if (startPos == std::string::npos)
        return tables;

    // Skip whitespace
    while (startPos < sql.size() && std::isspace(static_cast<unsigned char>(sql[startPos])))
        ++startPos;

    // Read table name
    size_t endPos = startPos;
    while (endPos < sql.size() &&
           !std::isspace(static_cast<unsigned char>(sql[endPos])) &&
           sql[endPos] != '(' &&
           sql[endPos] != ';')
    {
        ++endPos;
    }

    if (endPos > startPos)
    {
        tables.push_back(sql.substr(startPos, endPos - startPos));
    }

    return tables;
}

std::vector<std::string> SqlAnalyzer::extractColumnsDml(const std::string& sql){
    std::vector<std::string> columns;
    std::string upper = toUpper(sql);

    // ======================
    // INSERT INTO table (a,b,c)
    // ======================
    size_t insertPos = upper.find("INSERT");
    if (insertPos == 0) {
        size_t openParen = upper.find('(');
        size_t closeParen = upper.find(')', openParen);

        if (openParen != std::string::npos && closeParen != std::string::npos) {
            std::string inside = sql.substr(openParen + 1, closeParen - openParen - 1);

            std::istringstream ss(inside);
            std::string col;

            while (std::getline(ss, col, ',')) {
                col.erase(0, col.find_first_not_of(" \t"));
                col.erase(col.find_last_not_of(" \t") + 1);
                columns.push_back(col);
            }
        }

        return columns;
    }

    // ======================
    // UPDATE table SET a=1, b=2
    // ======================
    size_t updatePos = upper.find("UPDATE");
    if (updatePos == 0) {
        size_t setPos = upper.find("SET");
        if (setPos == std::string::npos) return columns;

        std::string afterSet = sql.substr(setPos + 3);

        size_t wherePos = upper.find("WHERE");
        if (wherePos != std::string::npos) {
            afterSet = sql.substr(setPos + 3, wherePos - (setPos + 3));
        }

        std::istringstream ss(afterSet);
        std::string part;

        while (std::getline(ss, part, ',')) {
            size_t eq = part.find('=');
            if (eq == std::string::npos) continue;

            std::string col = part.substr(0, eq);

            col.erase(0, col.find_first_not_of(" \t"));
            col.erase(col.find_last_not_of(" \t") + 1);

            columns.push_back(col);
        }

        return columns;
    }

    // Didn't extract columns for DELETE because PII doesn't require column names in the DELETE statement.
    return columns;
}

std::vector<std::string> SqlAnalyzer::extractTablesDdl(const std::string& sql) {
    std::vector<std::string> tables;

    std::string upper = toUpper(sql);

    // Find the TABLE keyword.
    // CREATE TABLE, ALTER TABLE, and DROP TABLE all contain it.
    size_t tablePos = upper.find("TABLE");

    if (tablePos == std::string::npos) {
        return tables;
    }

    // The table name appears immediately after TABLE.
    std::string afterTable = sql.substr(tablePos + 5);

    std::istringstream ss(afterTable);

    std::string tableName;
    ss >> tableName;

    // Remove possible trailing characters like '(' or ';' from the table name
    tableName.erase(
        std::remove_if(tableName.begin(), tableName.end(),
            [](char c) {
                return c == '(' || c == ';';
            }),
        tableName.end()
    );

    if (!tableName.empty()) {
        tables.push_back(tableName);
    }

    return tables;
}


