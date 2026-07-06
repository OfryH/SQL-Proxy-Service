#include <iostream>
#include "../src/analyzer/SqlAnalyzer.h"

void assertTrue(bool cond, const std::string& testName) {
    if (cond)
        std::cout << "[PASS] " << testName << "\n";
    else
        std::cout << "[FAIL] " << testName << "\n";
}

int main() {
    SqlAnalyzer analyzer;

    // 1. SELECT basic
    {
        auto r = analyzer.analyze("SELECT id, name FROM customers");
        assertTrue(r.type == StatementType::SELECT, "Select type");
        assertTrue(r.tables[0] == "customers", "Select table");
    }

    // 2. INSERT
    {
        auto r = analyzer.analyze("INSERT INTO customers (id) VALUES (1)");
        assertTrue(r.type == StatementType::DML, "Insert type");
        assertTrue(r.tables[0] == "customers", "Insert table");
    }

    // 3. UPDATE
    {
        auto r = analyzer.analyze("UPDATE customers SET name='x'");
        assertTrue(r.type == StatementType::DML, "Update type");
        assertTrue(r.tables[0] == "customers", "Update table");
    }

    // 4. DELETE
    {
        auto r = analyzer.analyze("DELETE FROM customers");
        assertTrue(r.type == StatementType::DML, "Delete type");
        assertTrue(r.tables[0] == "customers", "Delete table");
    }

    // 5. DDL
    {
        auto r = analyzer.analyze("CREATE TABLE customers (id INT)");
        assertTrue(r.type == StatementType::DDL, "DDL type");
    }

    // 6. Case insensitive
    {
        auto r = analyzer.analyze("select ID from Customers");
        assertTrue(r.type == StatementType::SELECT, "Case insensitive");
    }

    // 7. Columns parsing
    {
        auto r = analyzer.analyze("SELECT id, name, email FROM customers");
        assertTrue(r.columns.size() == 3, "Column count");  
    }

    // 8. Columns parsing with *
    {
        auto r = analyzer.analyze("SELECT * FROM customers");
        assertTrue(r.columns.size() == 1 && r.columns[0] == "*", "Column * parsing");  
    }

    // 9. Columns parsing with UPDATE
    {
        auto r = analyzer.analyze("UPDATE customers SET name='x', email='y'");
        assertTrue(r.columns.size() == 2, "Update columns count"); 
        assertTrue(r.columns[0] == "name" && r.columns[1] == "email", "Update columns names"); 
    }

    // 10. Columns parsing with INSERT
    {
        auto r = analyzer.analyze("INSERT INTO customers (id, name) VALUES (1, 'x')");
        assertTrue(r.columns.size() == 2, "Insert columns count");
        assertTrue(r.columns[0] == "id" && r.columns[1] == "name", "Insert columns names");
    }
    
    // 11. Columns parsing with DELETE (should be empty)
    {
        auto r = analyzer.analyze("DELETE FROM customers");
        assertTrue(r.columns.empty(), "Delete columns should be empty");
    }
    std::cout << "\nDone.\n";
}