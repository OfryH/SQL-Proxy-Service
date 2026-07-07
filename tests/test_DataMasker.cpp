#include <iostream>
#include "../src/masking/DataMasker.h"

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

QueryResult createTestResult()
{
    QueryResult result;

    result.success = true;

    result.columnNames = {
        "email",
        "phone",
        "credit_card",
        "name"
    };

    result.rows = {
        {
            "john@gmail.com",
            "0521234567",
            "1234567890123456",
            "John"
        }
    };

    return result;
}

std::vector<ColumnClassification> createClassifications()
{
    return {
        {"email", PiiType::EMAIL},
        {"phone", PiiType::PHONE},
        {"credit_card", PiiType::CREDIT_CARD},
        {"name", PiiType::NONE}
    };
}


void testEmailMask(DataMasker& masker)
{
    auto result = createTestResult();

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked[0][0] == "****@gmail.com",
          "Expected email to be masked before @");

    finishTest("Email masking");
}


void testPhoneMask(DataMasker& masker)
{
    auto result = createTestResult();

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked[0][1] == "05********",
          "Expected only first 2 phone digits");

    finishTest("Phone masking");
}


void testCreditCardMask(DataMasker& masker)
{
    auto result = createTestResult();

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked[0][2] == "************3456",
          "Expected only last 4 credit card digits");

    finishTest("Credit card masking");
}


void testNonPiiNotMasked(DataMasker& masker)
{
    auto result = createTestResult();

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked[0][3] == "John",
          "Expected non PII value to stay unchanged");

    finishTest("Non PII value");
}


void testMultipleRows(DataMasker& masker)
{
    QueryResult result;

    result.rows = {
        {
            "alice@gmail.com",
            "0541111111",
            "1111222233334444",
            "Alice"
        },
        {
            "bob@gmail.com",
            "0532222222",
            "5555666677778888",
            "Bob"
        }
    };

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked.size() == 2,
          "Expected two rows");

    check(masked[1][0] == "***@gmail.com",
          "Expected second email masked");

    finishTest("Multiple rows masking");
}


void testEmptyResult(DataMasker& masker)
{
    QueryResult result;

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked.size() == 0,
          "Expected empty result to remain empty");

    finishTest("Empty result masking");
}

void testFourthRow(DataMasker& masker)
{
    QueryResult result;

    result.rows = {
        {
            "alice@gmail.com",
            "0541111111",
            "1111222233334444",
            "Alice"
        },
        {
            "bob@gmail.com",
            "0532222222",
            "5555666677778888",
            "Bob"
        },
        {
            "charlie@gmail.com",
            "0523333333",
            "9999000011112222",
            "Charlie"
        },
        {
            "diana@gmail.com",
            "0514444444",
            "3333444455556666",
            "Diana"
        }
    };

    auto masked = masker.mask(
        result,
        createClassifications()
    );

    check(masked.size() == 4,
          "Expected four rows");

    check(masked[3][0] == "*****@gmail.com",
          "Expected fourth email masked");

    finishTest("Fourth row masking");
}


int main()
{
    DataMasker masker;

    testEmailMask(masker);
    testPhoneMask(masker);
    testCreditCardMask(masker);
    testNonPiiNotMasked(masker);
    testMultipleRows(masker);
    testEmptyResult(masker);
    testFourthRow(masker);

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