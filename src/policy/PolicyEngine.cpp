#include "PolicyEngine.h"

#include <fstream>
#include <sstream>


PolicyEngine::PolicyEngine()
    : allowSelect(false),
      allowInsert(false),
      allowUpdate(false),
      allowDelete(false),
      allowDDL(false)
{
}


bool PolicyEngine::loadPolicy(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string key;
        std::string value;

        if (std::getline(ss, key, '=') &&
            std::getline(ss, value))
        {
            bool enabled = (value == "true");

            if (key == "ALLOW_SELECT")
                allowSelect = enabled;

            else if (key == "ALLOW_INSERT")
                allowInsert = enabled;

            else if (key == "ALLOW_UPDATE")
                allowUpdate = enabled;

            else if (key == "ALLOW_DELETE")
                allowDelete = enabled;

            else if (key == "ALLOW_DDL")
                allowDDL = enabled;
        }
    }

    return true;
}


bool PolicyEngine::isAllowed(OperationType operation)
{
    switch (operation)
    {
        case OperationType::SELECT:
            return allowSelect;

        case OperationType::INSERT:
            return allowInsert;

        case OperationType::UPDATE:
            return allowUpdate;

        case OperationType::DELETE:
            return allowDelete;

        case OperationType::DDL:
            return allowDDL;

        default:
            return false;
    }
}