#pragma once

#include "../executor/QueryExecutor.h"
#include "../analyzer/SqlAnalyzer.h"
#include "../policy/PolicyEngine.h"
#include "../classifier/PiiClassifier.h"
#include "../masking/DataMasker.h"
#include "../audit/AuditLogger.h"
#include "../config/DatabaseConfig.h"

class SqlProxyService
{
public:
    SqlProxyService(const DatabaseConfig &config);
    QueryResult execute(const std::string &sql);

private:
    std::string statementTypeToString(StatementType type);
    std::string operationTypeToString(OperationType operation);
    SqlAnalyzer analyzer_;
    PolicyEngine policy_;
    QueryExecutor executor_;
    PiiClassifier classifier_;
    DataMasker masker_;
    AuditLogger logger_;
    AuditEntry createAuditEntry(const AnalysisResult &analysis);
    std::string piiTypeToString(PiiType type);
    std::string mapToString(const std::map<std::string, std::string> &values);
};