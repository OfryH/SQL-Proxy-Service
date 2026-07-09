#pragma once

#include <string>
#include <mysql.h>
#include "QueryResult.h"

class QueryExecutor
{
public:
    QueryExecutor();
    ~QueryExecutor();

    bool connect(
        const std::string &host,
        int port,
        const std::string &user,
        const std::string &password,
        const std::string &database);

    void disconnect();

    QueryResult execute(const std::string &sql);

private:
    MYSQL *connection_;
};