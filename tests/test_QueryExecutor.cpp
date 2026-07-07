#include "../src/executor/QueryExecutor.h"
#include <iostream>

int main()
{
    std::cout << "START TEST" << std::endl;
    QueryExecutor executor;

    bool connected = executor.connect(
        "localhost",
        "root",
        "11880055",
        "sql_proxy_demo"
    );

    if (connected)
    {
        std::cout << "Connection successful" << std::endl;
    }
    else
    {
        std::cout << "Connection failed" << std::endl;
    }

    return 0;
}