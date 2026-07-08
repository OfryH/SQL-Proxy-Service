#include <iostream>
#include <fstream>
#include <cstdio>

#include "../src/audit/AuditLogger.h"

const std::string RED   = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string RESET = "\033[0m";

int passed = 0;
int failed = 0;

bool currentTestPassed = true;

void check(bool condition, const std::string& message)
{
    if (!condition)
    {
        currentTestPassed = false;
        std::cout << "    " << message << '\n';
    }
}

void finishTest(const std::string& testName)
{
    if (currentTestPassed)
    {
        ++passed;
        std::cout << GREEN << "[PASS] " << RESET << testName << '\n';
    }
    else
    {
        ++failed;
        std::cout << RED << "[FAIL] " << RESET << testName << '\n';
    }

    currentTestPassed = true;
}

std::string readFile(const std::string& filename)
{
    std::ifstream in(filename);

    return std::string(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>()
    );
}

void testSingleLogEntry()
{
    const std::string file = "test_audit.log";

    std::remove(file.c_str());

    AuditLogger logger(file);

    AuditEntry entry;
    entry.query = "SELECT email FROM customers";
    entry.statementType = "SELECT";
    entry.statementOperation = "SELECT";
    entry.tables = {"customers"};
    entry.columns = {"email"};
    entry.piiColumns = {"email"};

    logger.log(entry);

    std::string content = readFile(file);

    check(content.find("SELECT email FROM customers") != std::string::npos,
          "Query not found");

    check(content.find("Type: SELECT") != std::string::npos,
          "Statement type not found");
    check(content.find("Operation: SELECT") != std::string::npos,
          "Statement operation not found");

    check(content.find("Tables: customers") != std::string::npos,
          "Tables not found");

    check(content.find("Columns: email") != std::string::npos,
          "Columns not found");

    check(content.find("PII Columns: email") != std::string::npos,
          "PII columns not found");

    finishTest("Single audit entry");

    std::remove(file.c_str());
}

void testMultipleColumns()
{
    const std::string file = "test_audit.log";

    std::remove(file.c_str());

    AuditLogger logger(file);

    AuditEntry entry;
    entry.query = "SELECT email, id FROM customers";
    entry.statementType = "SELECT";
    entry.statementOperation = "SELECT";
    entry.tables = {"customers"};
    entry.columns = {"email", "id"};
    entry.piiColumns = {"email"};

    logger.log(entry);

    std::string content = readFile(file);

    check(content.find("customers") != std::string::npos,
          "Tables missing");

    check(content.find("email, id") != std::string::npos,
          "Multiple columns missing");
    
    check(content.find("email") != std::string::npos,
          "PII columns missing");

    finishTest("Multiple columns");

    std::remove(file.c_str());
}

void testAppendLogEntries()
{
    const std::string file = "test_audit.log";

    std::remove(file.c_str());

    AuditLogger logger(file);

    AuditEntry entry;
    entry.query = "SELECT * FROM customers";
    entry.statementType = "SELECT";

    logger.log(entry);
    logger.log(entry);

    std::string content = readFile(file);

    size_t first = content.find("Query:");
    size_t second = content.find("Query:", first + 1);

    check(first != std::string::npos,
          "First log entry missing");

    check(second != std::string::npos,
          "Second log entry missing");

    finishTest("Append log entries");

    std::remove(file.c_str());
}

void testDmlLogging()
{
    const std::string file = "test_audit.log";

    std::remove(file.c_str());

    AuditLogger logger(file);

    AuditEntry entry;
    entry.query = "UPDATE customers SET email = 'newemail@example.com' WHERE id = 1";
    entry.statementType = "DML";
    entry.statementOperation = "UPDATE";
    entry.tables = {"customers"};
    entry.columns = {"email"};
    entry.piiColumns = {"email"};

    logger.log(entry);

    std::string content = readFile(file);

    check(content.find("UPDATE customers SET email = 'newemail@example.com' WHERE id = 1") != std::string::npos,
          "DML query not found");

    check(content.find("Type: DML") != std::string::npos,
          "Statement type not found");
    check(content.find("Operation: UPDATE") != std::string::npos,
          "Statement operation not found");

    check(content.find("Tables: customers") != std::string::npos,
          "Tables not found");

    check(content.find("Columns: email") != std::string::npos,
          "Columns not found");

    check(content.find("PII Columns: email") != std::string::npos,
          "PII columns not found");

    finishTest("DML logging");

    std::remove(file.c_str());
}

void testDdlLogging()
{
    const std::string file = "test_audit.log";

    std::remove(file.c_str());

    AuditLogger logger(file);

    AuditEntry entry;
    entry.query = "CREATE TABLE customers (id INT, email VARCHAR(255))";
    entry.statementType = "DDL";
    entry.statementOperation = "CREATE";
    entry.tables = {"customers"};
    entry.columns = {};
    entry.piiColumns = {};

    logger.log(entry);

    std::string content = readFile(file);

    check(content.find("CREATE TABLE customers (id INT, email VARCHAR(255))") != std::string::npos,
          "DDL query not found");

    check(content.find("Type: DDL") != std::string::npos,
          "Statement type not found");
    check(content.find("Operation: CREATE") != std::string::npos,
          "Statement operation not found");

    check(content.find("Tables: customers") != std::string::npos,
          "Tables not found");

    check(content.find("Columns: ") != std::string::npos,
          "Columns should be empty for DDL");

    check(content.find("PII Columns: ") != std::string::npos,
          "PII columns should be empty for DDL");

    finishTest("DDL logging");

    std::remove(file.c_str());
}

int main()
{
    testSingleLogEntry();
    testMultipleColumns();
    testDmlLogging();
    testDdlLogging();
    testAppendLogEntries();

    std::cout << "\n=========================\n";

    if (failed == 0)
    {
        std::cout << GREEN << "ALL TESTS PASSED" << RESET << '\n';
    }
    else
    {
        std::cout << RED << "SOME TESTS FAILED" << RESET << '\n';
    }

    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "=========================\n";

    return failed == 0 ? 0 : 1;
}