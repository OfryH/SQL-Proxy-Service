#include "../src/executor/QueryExecutor.h"
#include <iostream>

int main()
{
    QueryExecutor executor;

    bool connected = executor.connect(
            "localhost",
            3306,
            "proxy_user",
            "password",
            "sql_proxy_demo"
        );

    if (connected)
    {
        std::cout << "TEST PASSED! SUCCESSFULLY CONNECTED\n";
    }
    else
    {
        std::cout << "TEST FAILED :( CONNECTION FAILED\n";
    }

    return 0;
}