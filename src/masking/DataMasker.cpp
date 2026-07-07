#include "DataMasker.h"


std::vector<std::vector<std::string>> DataMasker::mask(
    const QueryResult& result,
    const std::vector<ColumnClassification>& classifications)
{
    std::vector<std::vector<std::string>> maskedRows = result.rows;

    for (auto& row : maskedRows)
    {
        for (size_t i = 0; i < row.size(); i++)
        {
            
            if (i < classifications.size())
                {
                    row[i] = maskValue(
                        row[i],
                        classifications[i].type
                    );
                }       
        }
    }

    return maskedRows;
}


std::string DataMasker::maskValue(
    const std::string& value,
    PiiType type)
{
    switch (type)
    {
        case PiiType::EMAIL:
            return maskEmail(value);

        case PiiType::PHONE:
            return maskPhone(value);

        case PiiType::CREDIT_CARD:
            return maskCreditCard(value);

        case PiiType::NONE:
        default:
            return value;
    }
}


std::string DataMasker::maskEmail(const std::string& email)
{
    auto atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0)
    {
        return std::string(email.size(), '*');
    }

    return std::string(atPos, '*') + email.substr(atPos);
}


std::string DataMasker::maskPhone(const std::string& phone)
{
    if (phone.length() <= 2)
    {
        return std::string(phone.length(), '*');
    }

    return phone.substr(0, 2) +
           std::string(phone.length() - 2, '*');
}


std::string DataMasker::maskCreditCard(const std::string& card)
{
    if (card.length() <= 4)
    {
        return std::string(card.length(), '*');
    }

    return std::string(card.length() - 4, '*') +
           card.substr(card.length() - 4);
}