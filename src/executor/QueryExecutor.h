#pragma once

#include <string>
#include <mysql.h>

class QueryExecutor
{
public:
    QueryExecutor();
    ~QueryExecutor();

    bool connect(
        const std::string& host,
        int port,
        const std::string& user,
        const std::string& password,
        const std::string& database
    );

    void disconnect();

private:
    MYSQL* connection_;
};