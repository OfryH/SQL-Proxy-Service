#include <iostream>
#include "../src/analyzer/SqlAnalyzer.h"

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

void testBasicSelect(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("SELECT id, name FROM customers");

    check(r.type == StatementType::SELECT,
          "Expected SELECT statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");

    finishTest("Basic SELECT");
}

void testInsert(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("INSERT INTO customers (id) VALUES (1)");

    check(r.type == StatementType::DML,
          "Expected DML statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");

    finishTest("Basic INSERT");
}

void testUpdate(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("UPDATE customers SET name='x'");

    check(r.type == StatementType::DML,
          "Expected DML statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");
    
    check(r.columns.size() == 1,
          "Expected exactly one column");

    check(r.columns[0] == "name",
          "Expected column 'name'");

    finishTest("Basic UPDATE");
}

void testDelete(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("DELETE FROM customers");

    check(r.type == StatementType::DML,
          "Expected DML statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");

    finishTest("Basic DELETE");
}

void testCreateTable(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("CREATE TABLE customers (id INT)");

    check(r.type == StatementType::DDL,
          "Expected DDL statement");

    finishTest("CREATE TABLE");
}

void testSelectColumns(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("SELECT id, name, email FROM customers");

    check(r.columns.size() == 3,
          "Expected 3 selected columns");
    
    check(r.columns[0] == "id",
          "First column should be 'id'");
    
    check(r.columns[1] == "name",
          "Second column should be 'name'");

    check(r.columns[2] == "email",
          "Third column should be 'email'");

    finishTest("SELECT columns");
}

void testSelectStar(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("SELECT * FROM customers");

    check(r.columns.size() == 1,
          "Expected one column");

    check(r.columns[0] == "*",
          "Expected '*'");

    finishTest("SELECT *");
}

void testUpdateColumns(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("UPDATE customers SET name='x', email='y'");

    check(r.columns.size() == 2,
          "Expected 2 updated columns");

    check(r.columns[0] == "name",
          "First column should be 'name'");

    check(r.columns[1] == "email",
          "Second column should be 'email'");

    finishTest("UPDATE columns");
}

void testInsertColumns(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze(
        "INSERT INTO customers (id, name) VALUES (1, 'x')");

    check(r.columns.size() == 2,
          "Expected 2 inserted columns");

    check(r.columns[0] == "id",
          "First column should be 'id'");

    check(r.columns[1] == "name",
          "Second column should be 'name'");

    finishTest("INSERT columns");
}

void testDeleteColumns(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("DELETE FROM customers WHERE id=1");

    check(r.columns.size() == 0,
          "Expected 0 columns for DELETE");

    finishTest("DELETE columns");
}

void testCreateTableTable(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("CREATE TABLE customers (id INT, name VARCHAR(100))");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");
}

void testAlterTable(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("ALTER TABLE customers ADD COLUMN email VARCHAR(100)");

    check(r.type == StatementType::DDL,
          "Expected DDL statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");

    finishTest("ALTER TABLE");
}

void testDropTable(SqlAnalyzer& analyzer)
{
    auto r = analyzer.analyze("DROP TABLE customers");

    check(r.type == StatementType::DDL,
          "Expected DDL statement");

    check(r.tables.size() == 1,
          "Expected exactly one table");

    check(r.tables[0] == "customers",
          "Expected table 'customers'");

    finishTest("DROP TABLE");
}

int main()
{
    SqlAnalyzer analyzer;

    testBasicSelect(analyzer);
    testInsert(analyzer);
    testUpdate(analyzer);
    testDelete(analyzer);
    testCreateTable(analyzer);
    testSelectColumns(analyzer);
    testSelectStar(analyzer);
    testUpdateColumns(analyzer);
    testInsertColumns(analyzer);
    testDeleteColumns(analyzer);
    testCreateTableTable(analyzer);
    testAlterTable(analyzer);
    testDropTable(analyzer);

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