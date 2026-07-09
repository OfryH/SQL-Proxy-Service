#include "PolicyEngine.h"

#include <fstream>
#include <sstream>
#include "../logger/Logger.h"
#include "../utils/StringUtils.h"


PolicyEngine::PolicyEngine()
    : allowSelect(false),
      allowInsert(false),
      allowUpdate(false),
      allowDelete(false),
      allowDDL(false)
{
}

// Responsible for loading the policy file and defining the policy by it
bool PolicyEngine::loadPolicy(const std::string& path)
{
    
    std::ifstream file(path);

    if (!file.is_open())
    {
        Logger::error("Policy file can't be opened");
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
            trim(key);
            trim(value);
            
            bool enabled = (value == "true" || value == "1");

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

        case OperationType::DELETE_OP:
            return allowDelete;

        case OperationType::DDL:
            return allowDDL;

        default:
            return false;
    }
}