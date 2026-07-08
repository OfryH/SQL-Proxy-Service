#include <iostream>
#include <fstream>
#include "../src/policy/PolicyEngine.h"

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

void createTestConfig()
{
    std::ofstream file("test_policy.conf");

    file << "ALLOW_SELECT=true\n";
    file << "ALLOW_INSERT=true\n";
    file << "ALLOW_UPDATE=false\n";
    file << "ALLOW_DELETE=false\n";
    file << "ALLOW_DDL=false\n";
}

void testLoadPolicy()
{
    createTestConfig();

    PolicyEngine policy;

    bool loaded = policy.loadPolicy("test_policy.conf");

    check(loaded,
          "Policy file should load");

    finishTest("Load policy");
}

void testAllowedOperations()
{
    createTestConfig();

    PolicyEngine policy;
    policy.loadPolicy("test_policy.conf");

    check(policy.isAllowed(OperationType::SELECT),
          "SELECT should be allowed");

    check(policy.isAllowed(OperationType::INSERT),
          "INSERT should be allowed");

    finishTest("Allowed operations");
}

void testDeniedOperations()
{
    createTestConfig();

    PolicyEngine policy;
    policy.loadPolicy("test_policy.conf");

    check(!policy.isAllowed(OperationType::UPDATE),
          "UPDATE should be denied");

    check(!policy.isAllowed(OperationType::DELETE_OP),
          "DELETE should be denied");

    check(!policy.isAllowed(OperationType::DDL),
          "DDL should be denied");

    finishTest("Denied operations");
}

void testMissingConfig()
{
    PolicyEngine policy;

    bool loaded = policy.loadPolicy("missing.conf");

    check(!loaded,
          "Missing file should fail");

    finishTest("Missing config file");
}

void testRealConfig()
{
    PolicyEngine policy;

    bool loaded = policy.loadPolicy("../../config/policy.conf");

    check(loaded,
          "Real policy file should load");
    check(policy.isAllowed(OperationType::SELECT),
          "SELECT should be allowed in real policy");
    check(policy.isAllowed(OperationType::INSERT),
          "INSERT should be allowed in real policy");
    check(!policy.isAllowed(OperationType::DDL),
          "DDL shouldn't be allowed in real policy");

    finishTest("Load real policy");
}

int main()
{
    testLoadPolicy();
    testAllowedOperations();
    testDeniedOperations();
    testMissingConfig();
    testRealConfig();

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