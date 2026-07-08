#include <iostream>
#include <fstream>
#include <sstream>

#include "../src/proxy/SqlProxyService.h"


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


void testSelect(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT name, email FROM customers"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows.size() == 4,
          "Expected 4 rows");

    check(result.rows[0][1] != "alice@gmail.com",
          "Expected email to be masked");

    finishTest("SELECT with masking");
}


void testInsert(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "INSERT INTO customers "
        "(name, email, phone, credit_card) "
        "VALUES "
        "('Proxy Test', 'proxy@test.com', '0501111111', '123456789')"
    );

    check(result.success,
          "Expected INSERT success");

    finishTest("INSERT execution");
}


void testInvalidQuery(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT * FROM unknown_table"
    );

    check(!result.success,
          "Expected query failure");

    finishTest("Invalid SQL");
}


void testDelete(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "DELETE FROM customers "
        "WHERE email='proxy@test.com'"
    );

    check(result.success,
          "Expected DELETE success");

    finishTest("DELETE execution");
}

void testSelectWithoutPii(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT id, name FROM customers"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows.size() == 4,
          "Expected 4 rows");

    check(result.rows[0][1] == "Alice Johnson",
          "Name should not be masked");

    finishTest("SELECT without PII");
}

void testEmailMasking(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT email FROM customers"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows[0][0] == "*****@gmail.com",
          "Email should be masked");

    finishTest("Email masking");
}

void testPhoneMasking(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT phone FROM customers"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows[0][0].substr(0,2) == "05",
          "Phone should keep first two digits");

    check(result.rows[0][0] != "050-1234567",
          "Phone should be masked");

    finishTest("Phone masking");
}

void testCreditCardMasking(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT credit_card FROM customers"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows[0][0].find("1111") != std::string::npos,
          "Last four digits should remain");

    check(result.rows[0][0] != "4111111111111111",
          "Credit card should be masked");

    finishTest("Credit card masking");
}

void testSelectNoRows(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELECT * FROM customers WHERE id = -1"
    );

    check(result.success,
          "Expected successful query");

    check(result.rows.empty(),
          "Expected no rows");

    finishTest("SELECT no rows");
}

void testSyntaxError(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "SELEC * FROM customers"
    );

    check(!result.success,
          "Expected syntax error");

    check(!result.errorMessage.empty(),
          "Expected error message");

    finishTest("Syntax error");
}

void testPolicyDenied(SqlProxyService& proxy)
{
    auto result = proxy.execute(
        "DROP TABLE customers"
    );

    check(!result.success,
          "Expected policy denial");

    finishTest("Policy denied");
}

void testInsertThenRead(SqlProxyService& proxy)
{
    proxy.execute(
        "INSERT INTO customers "
        "(name,email) VALUES "
        "('Proxy User','proxy2@test.com')"
    );

    auto result = proxy.execute(
        "SELECT name FROM customers "
        "WHERE email='proxy2@test.com'"
    );

    check(result.success,
          "Expected SELECT success");

    check(result.rows.size() == 1,
          "Expected one inserted row");

    proxy.execute(
        "DELETE FROM customers "
        "WHERE email='proxy2@test.com'"
    );

    finishTest("Insert then read");
}

void testAuditCreated(SqlProxyService& proxy)
{
    proxy.execute(
        "SELECT id, name FROM customers"
    );

    std::ifstream file("../../logs/audit.log");

    check(file.is_open(),
          "Audit log file should exist");

    std::string line;
    bool found = false;
    bool foundOperation = false;

    while (std::getline(file, line))
    {
        if (line.find("SELECT id, name FROM customers") != std::string::npos)
        {
            found = true;
            break;
        }
        if (line.find("Operation: SELECT") != std::string::npos)
        {
            foundOperation = true;
        }
    }

    check(found,
          "Expected query to appear in audit log");
    check(foundOperation,
          "Expected operation to appear in audit log");

    finishTest("Audit log created");
}

void testAuditContainsPii(SqlProxyService& proxy)
{
    proxy.execute(
        "SELECT email, phone FROM customers"
    );

    std::ifstream file("../../logs/audit.log");

    std::string line;
    bool foundEmail = false;
    bool foundPhone = false;

    while (std::getline(file, line))
    {
        if (line.find("email") != std::string::npos)
            foundEmail = true;

        if (line.find("phone") != std::string::npos)
            foundPhone = true;
    }

    check(foundEmail,
          "Expected email column in audit");

    check(foundPhone,
          "Expected phone column in audit");

    finishTest("Audit PII columns");
}


int main()
{
    SqlProxyService proxy(
        "../../config/policy.conf",
        "../../logs/audit.log"
    );


    testSelect(proxy);
    testInsert(proxy);
    testInvalidQuery(proxy);
    testDelete(proxy);
    testSelectWithoutPii(proxy);
    testEmailMasking(proxy);
    testPolicyDenied(proxy);
    testInsertThenRead(proxy);
    testAuditCreated(proxy);
    testAuditContainsPii(proxy);


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