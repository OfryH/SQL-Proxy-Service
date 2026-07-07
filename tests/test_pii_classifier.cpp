#include <iostream>
#include "../src/classifier/PiiClassifier.h"

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

void testEmailClassification(PiiClassifier& classifier)
{
    auto result = classifier.classify({"email"});

    check(result.size() == 1,
          "Expected one column");

    check(result[0].columnName == "email",
          "Expected column name 'email'");

    check(result[0].type == PiiType::EMAIL,
          "Expected EMAIL type");

    finishTest("Email classification");
}

void testPhoneClassification(PiiClassifier& classifier)
{
    auto result = classifier.classify({"phone"});

    check(result.size() == 1,
          "Expected one column");

    check(result[0].type == PiiType::PHONE,
          "Expected PHONE type");

    finishTest("Phone classification");
}

void testCreditCardClassification(PiiClassifier& classifier)
{
    auto result = classifier.classify({"credit_card"});

    check(result.size() == 1,
          "Expected one column");

    check(result[0].type == PiiType::CREDIT_CARD,
          "Expected CREDIT_CARD type");

    finishTest("Credit card classification");
}

void testNonPiiColumn(PiiClassifier& classifier)
{
    auto result = classifier.classify({"age"});

    check(result.size() == 1,
          "Expected one column");

    check(result[0].columnName == "age",
          "Expected column name 'age'");

    check(result[0].type == PiiType::NONE,
          "Expected NONE type");

    finishTest("Non PII column");
}

void testMultipleColumns(PiiClassifier& classifier)
{
    auto result = classifier.classify(
        {"id", "email", "phone", "name"}
    );

    check(result.size() == 4,
          "Expected 4 columns");

    check(result[0].type == PiiType::NONE,
          "id should not be PII");

    check(result[1].type == PiiType::EMAIL,
          "email should be PII");

    check(result[2].type == PiiType::PHONE,
          "phone should be PII");

    check(result[3].type == PiiType::NONE,
          "name should not be PII");

    finishTest("Multiple columns classification");
}

void testCaseInsensitive(PiiClassifier& classifier)
{
    auto result = classifier.classify({"EMAIL"});

    check(result[0].type == PiiType::EMAIL,
          "Expected case insensitive EMAIL detection");

    finishTest("Case insensitive classification");
}

int main()
{
    PiiClassifier classifier;

    testEmailClassification(classifier);
    testPhoneClassification(classifier);
    testCreditCardClassification(classifier);
    testNonPiiColumn(classifier);
    testMultipleColumns(classifier);
    testCaseInsensitive(classifier);

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