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


bool PolicyEngine::loadPolicy(const std::string& path)
{
    Logger::info("Loading policy from [" + path + "]");
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
            Logger::info("Key=[" + key + "] Value=[" + value + "]");
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
    Logger::info("ALLOW_SELECT: " + std::string(allowSelect ? "true" : "false"));
    Logger::info("ALLOW_INSERT: " + std::string(allowInsert ? "true" : "false"));
    Logger::info("ALLOW_UPDATE: " + std::string(allowUpdate ? "true" : "false"));
    Logger::info("ALLOW_DELETE: " + std::string(allowDelete ? "true" : "false"));
    Logger::info("ALLOW_DDL: " + std::string(allowDDL ? "true" : "false"));
        
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