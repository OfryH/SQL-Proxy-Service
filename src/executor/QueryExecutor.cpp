#include "QueryExecutor.h"

#include <iostream>


QueryExecutor::QueryExecutor()
{
    connection_ = nullptr;
}


QueryExecutor::~QueryExecutor()
{
    disconnect();
}


bool QueryExecutor::connect(
    const std::string& host,
    int port,
    const std::string& user,
    const std::string& password,
    const std::string& database
)
{
    connection_ = mysql_init(nullptr);

    if (connection_ == nullptr)
    {
        std::cerr << "mysql_init failed\n";
        return false;
    }

    if (mysql_real_connect(
            connection_,
            host.c_str(),
            user.c_str(),
            password.c_str(),
            database.c_str(),
            port,
            nullptr,
            0) == nullptr)
    {
        std::cerr << "Connection failed: "
                  << mysql_error(connection_)
                  << std::endl;

        mysql_close(connection_);
        connection_ = nullptr;

        return false;
    }

    return true;
}


void QueryExecutor::disconnect()
{
    if (connection_)
    {
        mysql_close(connection_);
        connection_ = nullptr;
    }
}