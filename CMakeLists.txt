cmake_minimum_required(VERSION 3.10)
project(sql_proxy)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    src
)

add_executable(analyzer_tests
    tests/test_analyzer.cpp
    src/analyzer/SqlAnalyzer.cpp
)

add_executable(pii_tests
    tests/test_Pii_classifier.cpp
    src/classifier/PiiClassifier.cpp
)

add_executable(masker_tests
    tests/test_DataMasker.cpp
    src/masking/DataMasker.cpp
)

add_executable(audit_tests
    tests/test_AuditLogger.cpp
    src/audit/AuditLogger.cpp
)

add_executable(policy_tests
    tests/test_PolicyEngine.cpp
    src/policy/PolicyEngine.cpp
)

add_executable(executor_tests
    tests/test_QueryExecutor.cpp
    src/executor/QueryExecutor.cpp
    src/config/DatabaseConfig.cpp
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
)

target_include_directories(proxy_tests PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/include"
)

target_link_directories(proxy_tests PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/lib"
)

target_link_libraries(proxy_tests libmysql)

target_include_directories(executor_tests PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/include"
)

target_link_directories(executor_tests PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/lib"
)

target_link_libraries(executor_tests
    libmysql
)

target_include_directories(sql_proxy PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/include"
)

target_link_directories(sql_proxy PRIVATE
    "C:/Program Files/MySQL/MySQL Server 8.0/lib"
)

target_link_libraries(sql_proxy
    libmysql
)