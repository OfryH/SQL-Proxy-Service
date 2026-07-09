cmake_minimum_required(VERSION 3.10)

project(sql_proxy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

include_directories(
    src
    /usr/include/mysql
)

add_executable(sql_proxy
    src/main.cpp
    src/proxy/SqlProxyService.cpp
    src/executor/QueryExecutor.cpp
    src/analyzer/SqlAnalyzer.cpp
    src/classifier/PiiClassifier.cpp
    src/masking/DataMasker.cpp
    src/audit/AuditLogger.cpp
    src/policy/PolicyEngine.cpp
    src/config/DatabaseConfig.cpp
    src/logger/Logger.cpp
)

target_link_libraries(sql_proxy
    mysqlclient
)


if(BUILD_TESTS)

    add_executable(analyzer_tests
        tests/test_analyzer.cpp
        src/analyzer/SqlAnalyzer.cpp
        src/logger/Logger.cpp
    )

    add_executable(pii_tests
        tests/test_Pii_classifier.cpp
        src/classifier/PiiClassifier.cpp
        src/logger/Logger.cpp
    )

    add_executable(masker_tests
        tests/test_DataMasker.cpp
        src/masking/DataMasker.cpp
        src/logger/Logger.cpp
    )

    add_executable(audit_tests
        tests/test_AuditLogger.cpp
        src/audit/AuditLogger.cpp
        src/logger/Logger.cpp
    )

    add_executable(policy_tests
        tests/test_PolicyEngine.cpp
        src/policy/PolicyEngine.cpp
        src/logger/Logger.cpp
    )

    add_executable(executor_tests
        tests/test_QueryExecutor.cpp
        src/executor/QueryExecutor.cpp
        src/config/DatabaseConfig.cpp
        src/logger/Logger.cpp
    )

    add_executable(proxy_tests
        tests/test_SqlProxyService.cpp
        src/proxy/SqlProxyService.cpp
        src/executor/QueryExecutor.cpp
        src/analyzer/SqlAnalyzer.cpp
        src/classifier/PiiClassifier.cpp
        src/masking/DataMasker.cpp
        src/audit/AuditLogger.cpp
        src/policy/PolicyEngine.cpp
        src/config/DatabaseConfig.cpp
        src/logger/Logger.cpp
    )

    target_link_libraries(executor_tests
        mysqlclient
    )

    target_link_libraries(proxy_tests
        mysqlclient
    )

endif()