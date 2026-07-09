#include "SqlProxyService.h"
#include <iostream>
#include "../logger/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>


std::string currentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(
        std::localtime(&time),
        "%Y-%m-%d %H:%M:%S"
    );

    return ss.str();
}

SqlProxyService::SqlProxyService(const DatabaseConfig& config)
    : logger_(config.logFile)
{
    // Loading the policy
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

// Fill the audit entry fields
AuditEntry SqlProxyService::createAuditEntry(const AnalysisResult& analysis)
{
    AuditEntry entry;

    entry.timestamp = currentTimestamp();
    entry.query = analysis.rawQuery;

    entry.statementType =
        statementTypeToString(analysis.type);

    entry.statementOperation =
        operationTypeToString(analysis.operation);

    entry.tables = analysis.tables;
    entry.columns = analysis.columns;

    return entry;
}

// Executing the proxy service
QueryResult SqlProxyService::execute(const std::string& sql)
{
    QueryResult result;

    // Analyze
    AnalysisResult analysis = analyzer_.analyze(sql);

    if (analysis.operation == OperationType::UNKNOWN)
    {
        result.success = false;
        result.errorMessage = "Invalid SQL query";

        AuditEntry entry = createAuditEntry(analysis);
        entry.status = "INVALID_SQL";
        entry.errorMessage = result.errorMessage;

        logger_.log(entry);
        return result;
    }
    // Policy
    if (!policy_.isAllowed(analysis.operation))
    {
        result.success = false;
        result.errorMessage = "Query rejected by policy";

        AuditEntry entry = createAuditEntry(analysis);
        entry.status = "DENIED_BY_POLICY";
        entry.errorMessage = result.errorMessage;

        logger_.log(entry);

        return result;
    }

    // Execute
    result = executor_.execute(sql);

    if (!result.success)
    {
        AuditEntry entry = createAuditEntry(analysis);
        entry.status = "EXECUTION_FAILED";
        entry.errorMessage = result.errorMessage;
       
        logger_.log(entry);

        return result;
    }

    // Mask SELECT results
    if (analysis.operation == OperationType::SELECT)
    {
        // Classifying which columns should be masked
        auto classifications =
            classifier_.classify(result.columnNames);
        // Masking the result
        result.rows =
            masker_.mask(result, classifications);
    }

    // Audit success
    AuditEntry entry = createAuditEntry(analysis);

    entry.status = "SUCCESS";
    
    if (analysis.columns.size() == 1 &&
        analysis.columns[0] == "*")
    {
        entry.columns = result.columnNames;
    }
    else
    {
        entry.columns = analysis.columns;
    }

    std::vector<std::string> columnsToClassify;
    // For select queries PII return in the result, for DML/DDL the PII is in the request
    if (analysis.operation == OperationType::SELECT){
        columnsToClassify = result.columnNames;
    } else {
        columnsToClassify = analysis.columns;
    }

    for (const auto& c : classifier_.classify(columnsToClassify))
    {
        if (c.type != PiiType::NONE)
        {
            entry.piiColumns[c.columnName] = piiTypeToString(c.type);
        }
    }

    if (analysis.operation == OperationType::SELECT)
    {
        entry.rowsAffected = result.rows.size();
    }
    else
    {
        entry.rowsAffected = result.rowsAffected;
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

std::string SqlProxyService::piiTypeToString(PiiType type)
{
    switch(type)
    {
        case PiiType::EMAIL:
            return "EMAIL";

        case PiiType::PHONE:
            return "PHONE";

        case PiiType::CREDIT_CARD:
            return "CREDIT_CARD";

        default:
            return "UNKNOWN";
    }
}