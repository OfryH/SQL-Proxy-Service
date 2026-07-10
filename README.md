# SQL Proxy Service

## Overview

SQL Proxy Service is a CLI-based proxy layer between users and a MySQL database.

The service receives SQL statements, analyzes them, applies policy rules, classifies sensitive data exposure, masks PII fields when required, writes audit logs, and executes allowed queries against the database.

The goal of this project is to simulate the behavior of a SQL proxy service that controls and monitors database access.

---

# Features

The proxy supports:

- Basic SQL statement analysis:
  - SELECT
  - DML:
    - INSERT
    - UPDATE
    - DELETE
  - DDL:
    - CREATE
    - ALTER
    - DROP

  The analyzer is designed for basic SQL statements and does not support advanced SQL features such as:
  - JOINs
  - Nested queries
  - Subqueries
  - Complex expressions
  - Aliases

- Policy-based query authorization
- PII classification
- Result masking
- Audit logging
- MySQL query execution

---

# Setup

## Requirements

- Docker
- Docker Compose

---
Clone the repository and navigate to the project directory

## Running the service

Start containers in detached mode:

```bash
docker compose up --build -d
```
The SQL Proxy CLI can then be accessed with:

```bash
docker attach sql_proxy
````

Application logs can be viewed with:

```bash
docker logs -f sql_proxy
```

The service starts a MySQL container and the SQL proxy container.


## Configuration

The service uses configuration files located under:

config/  
├── database.conf  
└── policy.conf  

### database.conf

Contains the database connection configuration and runtime paths.

Example:  
host=mysql  
port=3306
user=proxy_user  
password=password  
database=sql_proxy_demo  
LOG_FILE=logs/audit.log  
POLICY_FILE=config/policy.conf  

### policy.conf

Defines which SQL operations are allowed by the proxy.

# Architecture

The system is divided into several components:

## SqlProxyService

The main orchestration layer.

Responsibilities:
- Receives SQL queries
- Coordinates analysis, policy validation, execution, masking and auditing

## SqlAnalyzer

Responsible for analyzing SQL statements.

Extracts:
- Statement type
- Operation
- Referenced tables
- Columns involved

The analyzer uses a lightweight parser based on SQL keywords and patterns.

## PolicyEngine

Controls which operations are allowed.

Policies are loaded from:
config/policy.conf


Example:
ALLOW_SELECT=true
ALLOW_INSERT=true
ALLOW_UPDATE=true
ALLOW_DELETE=false
ALLOW_DDL=false


The policy configuration is designed to control potentially destructive database operations.

By default, read operations and controlled data modifications can be enabled, while destructive operations such as DELETE and DDL statements are disabled to reduce the risk of accidental data loss or unintended schema changes.

The policy is configurable, allowing administrators to enable or disable operations according to the required environment and security constraints.

## QueryExecutor

Responsible for communicating with MySQL and executing approved statements.

## PiiClassifier

Classifies sensitive columns:

Supported classifications:

| Column | Classification |
|---|---|
| email | PII.Email |
| phone | PII.Phone |
| credit_card | PII.CreditCard |

The classifier uses an exact column name match approach. Only columns with these exact names are classified as PII. 
Other columns containing similar information but with different names are not automatically classified.

## DataMasker

Masks sensitive values before returning SELECT results.

Examples: 

email:  
alice@gmail.com -> *****@gmail.com

phone: 
0501234567 -> 05********

credit_card: 
1234567890123456 -> ************3456

The masking rules are applied only to columns classified as PII by the PiiClassifier.

## AuditLogger

Creates an audit record for every processed query.

Audit records are stored in:

`logs/audit.log`

Each audit record contains:



| Field | Why it is logged |
|-------|-------------------|
| Timestamp | Records when the query was processed, making it possible to reconstruct the sequence of events. |
| Status | Indicates whether the query succeeded, was rejected by policy, was invalid, or failed during execution, allowing quick identification of the outcome. |
| Query | Preserves the original SQL statement for auditing and troubleshooting. |
| Type | Distinguishes between SELECT, DML, and DDL statements for high-level analysis. |
| Operation | Records the exact SQL operation (SELECT, INSERT, UPDATE, DELETE, CREATE, ALTER, DROP) to provide more detailed audit information. |
| Tables | Identifies which database tables were accessed or modified. |
| Columns | Shows which columns were read or updated, helping identify access to sensitive data. |
| PII Columns | Indicates which sensitive columns were detected together with their classification, demonstrating which data required masking. |
| Rows returned/affected | Shows the impact of the query by reporting the number of rows returned for SELECT statements or affected by DML statements. |
| Error | Recorded only when a query fails or is rejected, providing the reason for the failure to simplify troubleshooting. |

---

The `logs` directory is mounted as a Docker volume, so audit records are available on the host machine under the project directory.

## Database Setup

Database initialization scripts are located under:

sql/  
├── schema.sql  
└── seed.sql

The MySQL container automatically executes these scripts when initializing a new database instance.

The database state is persisted between container restarts. Running `docker compose up` again does not recreate the database or reload the seed data unless the database volume is removed.

To recreate the database from scratch, remove the existing volume and start the services again.

```bash
docker compose down -v
docker compose up --build
```

`schema.sql` creates the database schema, and `seed.sql` populates the database with demo data.

# Usage

After startup, SQL queries can be entered through the CLI:

```bash
docker attach sql_proxy
```

Example:

> SELECT * FROM customers;

SELECT results are returned with required PII masking.

Example:

```text
email             phone
*****@gmail.com  05********
```

For DML/DDL operations the service returns an execution status.

Example:

Query executed successfully. Rows affected: 1

# Policy Enforcement

Before execution every query is analyzed and checked against the configured policy.

Examples:

Allowed:

SELECT * FROM customers;

Rejected:

DROP TABLE customers;

when:

ALLOW_DDL=false

The policy configuration is loaded at startup and remains unchanged during runtime. 
A service restart is required after modifying the policy file.

# SQL Analysis Approach

The implementation uses a lightweight SQL analyzer instead of a full SQL parser.

Supported assumptions:

SQL statements begin with the relevant keyword.
Queries are simple and do not contain nested queries.
Table and column extraction is based on common SQL syntax patterns.

Examples supported:

SELECT name, email FROM customers;

UPDATE customers SET phone='0501234567';

DELETE FROM customers WHERE id=1;

# Design Decisions
## Why CLI instead of REST API?

A CLI interface was selected because the goal of the exercise is to demonstrate proxy behavior rather than API design.

The CLI provides a simple simulation of users submitting SQL statements and receiving responses.

## Why lightweight SQL parsing?

A full SQL parser adds significant complexity.

For this exercise, a best-effort analyzer is sufficient and keeps the implementation focused on proxy responsibilities:

policy
auditing
classification
masking

The current implementation has several limitations:

SQL parsing is not a complete SQL grammar parser.

Complex queries (joins, subqueries, aliases) are not fully supported. 

PII classification is based on column names. 

Only predefined PII column names are detected: 
  email 
  phone 
  credit_card 
  
DDL statements are analyzed and authorized, but the proxy does not perform PII detection on schema changes. For example, adding a new sensitive column through ALTER TABLE is not automatically classified. 

The proxy currently supports MySQL only and is not database-engine agnostic. 
