#include <iostream>
#include "../src/executor/QueryExecutor.h"

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


bool connectExecutor(QueryExecutor& executor)
{
    return executor.connect(
        "localhost",
        3306,
        "proxy_user",
        "password",
        "sql_proxy_demo"
    );
}


void testConnection(QueryExecutor& executor)
{
    bool connected = connectExecutor(executor);

    check(connected,
          "Expected successful database connection");

    finishTest("Database connection");
}


void testSelectCustomers(QueryExecutor& executor)
{
    auto result = executor.execute(
        "SELECT id, name FROM customers"
    );

    check(result.success,
          "Expected query success");

    check(result.columnNames.size() == 2,
          "Expected 2 columns");

    check(result.rows.size() == 4,
          "Expected 4 customers");

    finishTest("SELECT customers");
}


void testSelectPIIColumns(QueryExecutor& executor)
{
    auto result = executor.execute(
        "SELECT email, phone, credit_card FROM customers"
    );

    check(result.success,
          "Expected query success");

    check(result.columnNames.size() == 3,
          "Expected 3 columns");

    check(result.rows.size() == 4,
          "Expected 4 rows");

    check(result.rows[0][0] == "alice@gmail.com",
          "Expected Alice email");

    check(result.rows[0][1] == "050-1234567",
          "Expected Alice phone");

    finishTest("SELECT PII columns");
}


void testSelectJoin(QueryExecutor& executor)
{
    auto result = executor.execute(
        "SELECT customers.name, orders.amount "
        "FROM customers "
        "JOIN orders ON customers.id = orders.customer_id"
    );

    check(result.success,
          "Expected join query success");

    check(result.rows.size() == 5,
          "Expected 5 orders");

    finishTest("SELECT JOIN");
}


void testInsert(QueryExecutor& executor)
{
    auto result = executor.execute(
        "INSERT INTO customers "
        "(name, email, phone, credit_card) "
        "VALUES "
        "('Test User', 'test@test.com', '0500000000', '123456789')"
    );

    check(result.success,
          "Expected insert success");

    finishTest("INSERT query");
}


void testUpdate(QueryExecutor& executor)
{
    auto result = executor.execute(
        "UPDATE customers "
        "SET phone='0509999999' "
        "WHERE email='test@test.com'"
    );

    check(result.success,
          "Expected update success");

    finishTest("UPDATE query");
}


void testDelete(QueryExecutor& executor)
{
    auto result = executor.execute(
        "DELETE FROM customers "
        "WHERE email='test@test.com'"
    );

    check(result.success,
          "Expected delete success");

    finishTest("DELETE query");
}


void testInvalidQuery(QueryExecutor& executor)
{
    auto result = executor.execute(
        "SELECT * FROM not_existing_table"
    );

    check(!result.success,
          "Expected query failure");

    check(!result.errorMessage.empty(),
          "Expected error message");

    finishTest("Invalid SQL query");
}


int main()
{
    QueryExecutor executor;

    testConnection(executor);

    if (failed > 0)
    {
        return 1;
    }

    testSelectCustomers(executor);
    testSelectPIIColumns(executor);
    testSelectJoin(executor);

    testInsert(executor);
    testUpdate(executor);
    testDelete(executor);

    testInvalidQuery(executor);

    executor.disconnect();

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