cmake_minimum_required(VERSION 3.10)
project(sql_proxy)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    src
    "C:/Program Files/mysql-connector-c++-9.7.0-winx64/include"
)

link_directories(
    "C:/Program Files/mysql-connector-c++-9.7.0-winx64/lib64/vs14"
)

add_executable(analyzer_tests
    tests/test_analyzer.cpp
    src/analyzer/SqlAnalyzer.cpp
)

add_executable(executor_tests
    tests/test_QueryExecutor.cpp
    src/executor/QueryExecutor.cpp
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

target_link_libraries(executor_tests mysqlcppconn)