#pragma once

#include <string>
#include "../analyzer/SqlAnalyzer.h"

class PolicyEngine
{
public:
    PolicyEngine();
    bool loadPolicy(const std::string& path);

    bool isAllowed(
        OperationType operation
    );

private:
    bool allowSelect;
    bool allowInsert;
    bool allowUpdate;
    bool allowDelete;
    bool allowDDL;
};