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


QueryResult QueryExecutor::execute(const std::string& sql)
{
    QueryResult result;

    if (connection_ == nullptr)
    {
        result.success = false;
        result.errorMessage = "No database connection";
        return result;
    }

    if (mysql_query(connection_, sql.c_str()) != 0)
    {
        result.success = false;
        result.errorMessage = mysql_error(connection_);
        return result;
    }

    MYSQL_RES* res = mysql_store_result(connection_);

    // Query that does not return rows (INSERT/UPDATE/DELETE/DDL)
    if (res == nullptr)
    {
        result.success = true;
        return result;
    }

    int columns = mysql_num_fields(res);

    MYSQL_FIELD* fields = mysql_fetch_fields(res);

    for (int i = 0; i < columns; i++)
    {
        result.columnNames.push_back(fields[i].name);
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)))
    {
        std::vector<std::string> currentRow;

        for (int i = 0; i < columns; i++)
        {
            currentRow.push_back(
                row[i] ? row[i] : "NULL"
            );
        }

        result.rows.push_back(currentRow);
    }

    mysql_free_result(res);

    result.success = true;

    return result;
}


void QueryExecutor::disconnect()
{
    if (connection_)
    {
        mysql_close(connection_);
        connection_ = nullptr;
    }
}