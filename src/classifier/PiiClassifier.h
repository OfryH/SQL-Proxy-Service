#pragma once

#include <string>
#include <vector>

enum class PiiType
{
    NONE,
    EMAIL,
    PHONE,
    CREDIT_CARD
};

struct ColumnClassification
{
    std::string columnName;
    PiiType type;
};

class PiiClassifier
{
public:
    std::vector<ColumnClassification> classify(
        const std::vector<std::string>& columns
    );

private:
    PiiType classifyColumn(const std::string& column);
};