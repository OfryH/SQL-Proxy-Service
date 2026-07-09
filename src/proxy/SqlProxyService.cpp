#include "SqlProxyService.h"
#include <iostream>
#include "../logger/Logger.h"


SqlProxyService::SqlProxyService(const DatabaseConfig& config)
    : logger_(config.logFile)
{

    if (!policy_.loadPolicy(config.policyFile))
    {
        Logger::error("Failed loading policy");
    }
    else
    {
        Logger::info("Policy loaded successfully");
    }

    Logger::info("Connecting to database host: " + config.host);

    bool connected = executor_.connect(
        config.host,
        config.port,
        config.user,
        config.password,
        config.database
    );

    if (connected)
    {
        Logger::info("Database connection successful");
    }
    else
    {
        Logger::error("Database connection failed");
    }
}

QueryResult SqlProxyService::execute(const std::string& sql)
{
    QueryResult result;

    // 1. Analyze
    AnalysisResult analysis = analyzer_.analyze(sql);

    // 2. Policy
    if (!policy_.isAllowed(analysis.operation))
    {
        result.success = false;
        result.errorMessage = "Query rejected by policy";

        AuditEntry entry;
        entry.query = sql;
        entry.statementType = statementTypeToString(analysis.type);
        entry.statementOperation = operationTypeToString(analysis.operation);
        entry.tables = analysis.tables;
        entry.columns = analysis.columns;

        logger_.log(entry);

        return result;
    }

    // 3. Execute
    result = executor_.execute(sql);

    if (!result.success)
    {
        AuditEntry entry;
        entry.query = sql;
        entry.statementType = "EXECUTION_FAILED";
        entry.statementOperation = "ERROR";
        entry.tables = analysis.tables;
        entry.columns = analysis.columns;

        logger_.log(entry);

        return result;
    }

    // 4. Mask SELECT results
    if (analysis.operation == OperationType::SELECT)
    {
        auto classifications =
            classifier_.classify(result.columnNames);

        result.rows =
            masker_.mask(result, classifications);
    }

    // 5. Audit success
    AuditEntry entry;

    entry.query = sql;
    entry.statementType = statementTypeToString(analysis.type);
    entry.statementOperation = operationTypeToString(analysis.operation);

    entry.tables = analysis.tables;
    entry.columns = analysis.columns;


    for (const auto& c : classifier_.classify(result.columnNames))
    {
        if (c.type != PiiType::NONE)
        {
            entry.piiColumns.push_back(c.columnName);
        }
    }

    logger_.log(entry);


    return result;

}

std::string SqlProxyService::statementTypeToString(StatementType type)
{
    switch(type)
    {
        case StatementType::SELECT:
            return "SELECT";

        case StatementType::DML:
            return "DML";

        case StatementType::DDL:
            return "DDL";
    }

    return "UNKNOWN";
}


std::string SqlProxyService::operationTypeToString(OperationType operation)
{
    switch(operation)
    {
        case OperationType::SELECT:
            return "SELECT";

        case OperationType::INSERT:
            return "INSERT";

        case OperationType::UPDATE:
            return "UPDATE";

        case OperationType::DELETE_OP:
            return "DELETE";

        case OperationType::DDL:
            return "DDL";
    }

    return "UNKNOWN";
}