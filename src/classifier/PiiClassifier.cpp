#include "PiiClassifier.h"
#include <algorithm>


std::vector<ColumnClassification> PiiClassifier::classify(
    const std::vector<std::string>& columns)
{
    std::vector<ColumnClassification> result;

    for (const auto& column : columns)
    {
        result.push_back({
            column,
            classifyColumn(column)
        });
    }

    return result;
}


PiiType PiiClassifier::classifyColumn(const std::string& column)
{
    std::string lower = column;

    std::transform(
        lower.begin(),
        lower.end(),
        lower.begin(),
        ::tolower
    );

    if (lower == "email")
    {
        return PiiType::EMAIL;
    }

    if (lower == "phone")
    {
        return PiiType::PHONE;
    }

    if (lower == "credit_card")
    {
        return PiiType::CREDIT_CARD;
    }

    return PiiType::NONE;
}