#include <iostream>
#include <string>

#include "config/DatabaseConfig.h"
#include "proxy/SqlProxyService.h"
#include "logger/Logger.h"

int main()
{
    DatabaseConfig config;

    if (!config.load("config/database.conf"))
    {
        Logger::error("Failed loading database configuration");
        return 1;
    }

    SqlProxyService proxy(config);

    Logger::info("SQL Proxy Service started");
    std::cout << "Enter SQL queries (type exit to quit)\n\n";

    std::string query;

    while (true)
    {
        std::cout << "> ";

        std::getline(std::cin, query);

        if (query == "exit")
        {
            break;
        }

        QueryResult result = proxy.execute(query);

        if (!result.success)
        {
            std::cout << "ERROR: "
                      << result.errorMessage
                      << "\n";
            continue;
        }

        if (!result.rows.empty())
        {
            for (const auto &column : result.columnNames)
            {
                std::cout << column << "\t";
            }

            std::cout << "\n";

            for (const auto &row : result.rows)
            {
                for (const auto &value : row)
                {
                    std::cout << value << "\t";
                }

                std::cout << "\n";
            }
        }
        else
        {
            std::cout << "Query executed successfully. "
                      << "Rows affected: "
                      << result.rowsAffected
                      << "\n";
        }

        std::cout << "\n";
    }

    return 0;
}