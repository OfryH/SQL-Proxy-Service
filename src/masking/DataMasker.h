#pragma once

#include <string>
#include <vector>

#include "../executor/QueryResult.h"
#include "../classifier/PiiClassifier.h"

class DataMasker
{
public:
    std::vector<std::vector<std::string>> mask(
        const QueryResult &result,
        const std::vector<ColumnClassification> &classifications);

private:
    std::string maskValue(
        const std::string &value,
        PiiType type);

    std::string maskEmail(const std::string &email);
    std::string maskPhone(const std::string &phone);
    std::string maskCreditCard(const std::string &card);
};