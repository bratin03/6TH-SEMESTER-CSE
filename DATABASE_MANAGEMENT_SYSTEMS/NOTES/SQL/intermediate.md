# JOIN
**Natural Join**
```sql
SELECT name,course_id
FROM student NATURAL JOIN takes;
```
Consider the two queries:
```sql
SELECT name,title
FROM student NATURAL JOIN takes, course
WHERE takes.course_id = course.course_id;
--
SELECT name,title
FROM student NATURAL JOIN takes NATURAL JOIN course;
```
The first query lists the names of students and the titles of the courses that they are taking.
But in the second query the second natural join imposes an additional condition that the student department must be the same as the course department. So the second query lists the names of students and the titles of the courses that they are taking in their own department.

The second query does not give the desired result. In order to get the correct result we should write as follows using **JOIN...USING** clause:
```sql
SELECT name,title
FROM (student NATURAL JOIN takes) JOIN course USING (course_id);
```

**ON** clause can also be used to specify the join condition.
```sql
SELECT *
FROM student JOIN takes ON student.ID = takes.ID;
```
**Outer Join**
- The **left outer join** returns all the rows from the left table, and the matched rows from the right table. The result is NULL from the right side if there is no match.
- The **right outer join** returns all the rows from the right table, and the matched rows from the left table. The result is NULL from the left side if there is no match.
- The **full outer join** returns all the rows when there is a match in one of the tables. The result is NULL from the left side if there is no match.

```sql
  SELECT *
  FROM student NATURAL LEFT OUTER JOIN takes;
```
```sql
  SELECT *
  FROM student NATURAL RIGHT OUTER JOIN takes;
```
```sql
  SELECT *
  FROM student NATURAL FULL OUTER JOIN takes;
```
- We can also sepcify the **ON** condition in the outer join.
```sql
  SELECT *
  FROM student LEFT OUTER JOIN takes ON student.ID = takes.ID;
```
**Join Types and Conditions**
- Keyword **INNER** is optional in the **INNER JOIN** clause. It is **INNER JOIN** by default. Similarly **NATURAL JOIN** is equivalent to **NAUTRAL INNER JOIN**.
## Summary
### Join Types
1. inner join
2. left outer join
3. right outer join
4. full outer join
### Join Conditions
1. natural
2. on predicate
3. using \(A_1,A_2,...,A_n\)

# VIEWS
```sql
CREATE VIEW v AS <query expression>
```
```sql
CREATE VIEW faculty as 
SELECT ID,name,dept_name
FROM instructor;
```
```sql
CREATE VIEW departments_total_salary(dept_name,total_salary) AS
SELECT dept_name, SUM(salary)
FROM instructor
GROUP BY dept_name;
```
**Materialized View**
Some database systems allow views to be stored physically.
This is called a materialized view. The materialized view is updated periodically or lazily to keep the view consistent with the base tables. 
This process is called maintenance of the materialized view.

**Update of a View**
- The modification to the database expressed in terms of a view must be translated into a modification to the actual relations in the logical model of the database.
- Two reasonable approaches
- 1. Reject the insertion and send an error message to the user.
- 2. Insert with a null value for the attribute that is not present in the view.
- Consider the following view:
```sql
CREATE VIEW instructor_info AS
SELECT ID,name,building
FROM instructor,department
WHERE instructor.dept_name = department.dept_name;
-- 
INSERT INTO instructor_info VALUES('10101','Srinivasan','Watson');
-- For this insertion to be valid, We will need to insert ('10101,'Srinivasan',null,null) into the instructor table and (null,'Watson',null) into the department table.
-- But even after this the view instructor_info will not contain the entry ('10101','Srinivasan','Watson') because the dept_name field is null.
```
**Updatable Views**
- The **FROM** clause has only one database relation.
- The **SELECT** clause has only attribute names of the relation and does not have any expressions, aggregate functions, or distinct specification.
- Any attribute that is not listed in the **SELECT** clause can be set to null; that is, it does not have a **NOT NULL** constraint and is not part of a primary key.
- The query does not have a **GROUP BY** or **HAVING** clause.
```sql
CREATE VIEW history_instructors AS
SELECT *
FROM instructor
WHERE dept_name = 'History';
-- Trying to insert ('25566','Brown',Biology'100000') into the view will be allowed by SQL by default. 
-- But it will not appear in the history_instructors view.
```
- Views can be defined wtih the **WITH CHECK OPTION** clause. This clause prevents the insertion of a tuple that cannot appear in the view.
```sql
CREATE VIEW history_instructors AS
SELECT *
FROM instructor
WHERE dept_name = 'History'
WITH CHECK OPTION;
```
# TRANSACTIONS
- **COMMIT WORK** makes the changes to the database permanent.
- **ROLLBACK WORK** undoes the changes to the database.
- The keyword **WORK** is optional.
- The database provides an abstraction of a transaction as being atomic.
```sql
SET AUTOCOMMIT ON;
--
SET AUTOCOMMIT OFF;
```
- **BEGIN ATOMIC ... END**

# INTEGRITY CONSTRAINTS
- Can be done while creating database using **CREATE TABLE** statement or after creating the table using **ALTER TABLE** table-name **ADD** constraint
## Constraints on a single relation
- **NOT NULL**: The **NOT NULL** constraint specifies that the attribute cannot have a null value and is an example of domain constraint.
```sql
name varchar(20) NOT NULL
budget numeric(12,2) NOT NULL
```
- **UNIQUE**: The **UNIQUE** constraint specifies that the attributes form a superkey. 
- Attribute defined as **UNIQUE** can have multiple null values.
```sql
UNIQUE (ID,dept_name)
```
- **CHECK**: The **CHECK** constraint specifies a condition that must be satisfied by the attribute values of each tuple.
```sql
CHECK (budget > 0)
```
- <font color='red'>A check clause is satisfied if it is not false, so clauses that evaluate to true or unknown are allowed. If null values are not desired, a separate **NOT NULL** constraint should be used.</font>
## Referential Integrity
```sql
FOREIGN KEY (dept_name) REFERENCES department
--
FOREIGN KEY (dept_name) REFERENCES department(dept_name)
```
- The specified attribute or attributes must be declared as a superkey of the referenced relation, using either a **UNIQUE** constraint or a **PRIMARY KEY** constraint.
- Foreign key must reference a compatible set of attributes, that is number of attributes must be the same and the data types of corresponding attributes must be compatible.
- **CASCADE**:
```sql
CREATE TABLE course
(...
FOREIGN KEY (dept_name) REFERENCES department
ON DELETE CASCADE
ON UPDATE CASCADE
);
```
- If a department is deleted, then all the courses offered by that department are also deleted.
- If a department name is changed, then all the courses offered by that department are also updated.
- **SET NULL**:
```sql
CREATE TABLE course
(...
FOREIGN KEY (dept_name) REFERENCES department
ON DELETE SET NULL
ON UPDATE SET NULL
);
```
- If a department is deleted, then the department name of all the courses offered by that department is set to null.
- If a department name is changed, then the department name of all the courses offered by that department is set to null.
- <font color='red'>If there is a chain of foreign-key depenedencies across multiple relations, a deletion or update at one end of the chain can propagate across the entire chain.
- If a cascading update or delete causes a constraint violation that cannot be handled by a further cascading operation, the system aborts the transaction. As a result, all the changes made by the transaction are undone.</font>
- **NULL** values satisfy the foreign-key constraints.
- **Assigning name to constraints**:
```sql
salary numeric(8,2) CONSTRAINT min_salary CHECK (salary > 29000)
--
ALTER TABLE instructor DROP CONSTRAINT min_salary;
```
- **Deferrable Constraints**:
```sql
SET CONSTRAINTS constraint_name DEFERRED;
SET CONSTRAINTS constraint_name IMMEDIATE;
```
- **Complex Check Conditions and Assertions**:
```sql
CHECK (time_slot_id IN (SELECT time_slot_id FROM time_slot))
--
CREATE ASSERTION credits_earned_constraint CHECK
(NOT EXISTS (SELECT ID 
FROM student
WHERE tot_cred <> (SELECT COALESCE(SUM(credits),0) FROM takes NATURAL JOIN course
WHERE student.ID = takes.ID AND grade IS NOT NULL AND grade <> 'F')));
```
# SQL Data Types and Schemas
## Date and Time Types in SQL
- **date**: A calendar date in the form **YYYY-MM-DD**.
- **time**: A time of day in the form **HH:MM:SS**. A variant of this type is **time(n)**, where **n** is the number of decimal places in the seconds. It is also possible to specify a time zone by adding **WITH TIME ZONE**.
- **timestamp**: A date and time of day in the form **YYYY-MM-DD HH:MM:SS**. A variant of this type is **timestamp(n)**, where **n** is the number of decimal places in the seconds. It is also possible to specify a time zone by adding **WITH TIME ZONE**.
- To extract individual fields from a date or time value, we can use the **EXTRACT** function.
```sql
EXTRACT (YEAR FROM date)
EXTRACT (MONTH FROM date)
EXTRACT (DAY FROM date)
EXTRACT (HOUR FROM time)
EXTRACT (MINUTE FROM time)
EXTRACT (SECOND FROM time)
EXTRACT (YEAR FROM timestamp)
```
SQL defines several functions to get the current date and time.
- **CURRENT_DATE**: Returns the current date.
- **CURRENT_TIME**: Returns the current time.
- **CURRENT_TIMESTAMP**: Returns the current date and time.
- **LOCALTIME**: Returns the current time.
- **LOCALTIMESTAMP**: Returns the current date and time.
- SQL provides a data type **INTERVAL** for representing a period of time.
```sql
INTERVAL '1' YEAR
```
## Typc Conversion and Formatting Functions
```sql
SELECT CAST(ID AS numeric(5)) AS inst_id
FROM instructor
ORDER BY inst_id;
```
**COALESCE** function returns the first non-null argument.
```sql
SELECT ID, COALESCE(salary,0) AS salary
FROM instructor;
```
- A limitation of **COALESCE** is that all the arguments must be of the same type.
## Default Values
```sql
CREATE TABLE student
(...
tot_cred numeric(3,0) DEFAULT 0
...);
```
### Large-Object Types
- **BLOB**: Binary large object
- **CLOB**: Character large object
```sql
book_review CLOB(10KB)
image BLOB(10MB)
```
### User-Defined Types
#### distinct types
```sql
CREATE TYPE Dollars AS numeric(12,2) FINAL;
CREATE TYPE Pounds AS numeric(12,2) FINAL;
-- Attempting to assign a value of type Dollars to a variable of type Pounds will result in an error.
```
- We can also use **CREATE DOMAIN** to define a distinct type.
```sql
CREATE DOMAIN DDollars AS numeric(12,2) NOT NULL;
```
#### Differnece between Type and Domain
1. Domains can have constraints, such as **NOT NULL** and can have **DEFAULT** values. but user-defined types cannot.
2. Domains are not strongly typed. As a result, values of one domain type can be assigned to values of another domain as long as the underlying types are compatible.
```sql
CREATE DOMAIN YearlySalary numeric(8,2) constraint salary_value_test CHECK (value > 29000.00);
```
### Generating Unique Key Values
```sql
ID number(5) generated always as identity
-- In this case the user cannot specify a value for the ID attribute. The system will generate a unique value for the ID attribute.
ID number(5) generated by default as identity
-- In this case the user can specify a value for the ID attribute. If the user does not specify a value, the system will generate a unique value for the ID attribute.
```
### Create Table Extensions
```sql
CREATE TABLE temp_instructor LIKE instructor;
--
CREATE TABLE t1 AS
(SELECT *
FROM instructor
WHERE dept_name = 'Music')
WITH DATA;
```
### Schemas, Catalogs, and Environments
- **Schema**: A collection of database objects, such as tables, views, and constraints, that are owned by a single user or organization.
- **Catalog**: A collection of schemas.
- **Environment**: A collection of catalogs.
# Index Defintion in SQL
- An index is a data structure that can be used to locate the rows of a relation quickly.
```sql
CREATE INDEX <index-name> ON <table-name> (<attribute-list>);
--
CREATE INDEX dept_index ON instructor (dept_name);
-- If we want to declare search key as candidate key, we can use UNIQUE keyword.
CREATE UNIQUE INDEX dept_index ON instructor (dept_name);
-- If index creation using UNIQUE keyword succeds, then any subsequent attempt to insert a tuple with a duplicate value for the indexed attribute will fail.
```
# Authorization in SQL
- The SQL standard includes the privileges **SELECT**, **INSERT**, **DELETE**, and **UPDATE**. The privilege **ALL PRIVILEGES** can be used as a short form for all the privileges. A user who creates a new relation is automatically given all privileges on that relation.
### GRANT
```sql
GRANT <privilege-list> ON <relation-name or view-name> TO <user-list>;
--
GRANT SELECT ON department TO Amit, Satoshi;
```
**update authorization**
- The **update** authorization on a relation allows a user to update any tuple in the relation. The **update** authorization may be given either on all attributes of the relation or on only some. If the list of attributes is empty, then the user is allowed to update any attribute of the relation.
```sql
GRANT UPDATE (name,dept_name) ON instructor TO Amit;
```
**insert authorization**
- The **insert** authorization on a relation allows a user to insert new tuples into the relation. The **insert** authorization may be given either on all attributes of the relation or on only some. The system either gives each of the remaining attributes a default value or sets them to null.
```sql
GRANT INSERT (ID,name,dept_name) ON instructor TO Amit;
```
**delete authorization**
- The **delete** authorization on a relation allows a user to delete any tuple from the relation.

**public**
- The user name **public** refers to all current and future users of the database. Thus privileges granted to **public** are granted to all users.

### REVOKE
```sql
revoke <privilege-list> ON <relation-name or view-name> FROM <user-list>;
-- 
REVOKE SELECT ON department FROM Amit, Satoshi;
```

## Roles
```sql
CREATE ROLE instructor;
--
GRANT SELECT ON takes TO instructor;
--
CREATE ROLE dean;
GRANT isntructor TO dean;
GRANT dean TO santoshi;
```
- The privileges of a user or a role consist of
- 1. All privileges granted to the user or role directly.
- 2. All privileges granted to roles that have been granted to the user or role.
## Authorization on Views
- A user who creates a view does not necessarily receive all pribileges on the view. It receive only the privilges that provide no additional authorization beyond those that the user already has on the base relations.
- If a user creates a view on which no authorization can be granted, then the system will deny creation of that view.
## Authorization on Schema
```sql
GRANT REFERENCES (dept_name) ON department TO Mariano;
```
## Transfer of Privileges
```sql
GRANT SELECT ON department TO Amit WITH GRANT OPTION;
```
- **AUTHORIZATION GRAPH** is a directed graph in which the nodes are users and the edges are the privileges. The graph is constructed by starting with a node for each user and then adding an edge from user A to user B if A has granted a privilege to B.
## Revoking Privileges
- If a user revokes a privilege from another user, the system must check whether the user who is revoking the privilege has the privilege with the **GRANT OPTION**. If the user has the **GRANT OPTION**, then the system must also revoke the privilege from all users to whom the user has granted the privilege. In case a user has been granted a privilege through more than one path in the authorization graph, the system must revoke the privilege from the user only if the user has no remaining path to the privilege. It is called  **cascading revocation**.
- We can change this default behavior by using the **RESTRICT** keyword.
```sql
REVOKE SELECT ON department FROM Amit, Satoshi RESTRICT;
-- In this case, the system returns an error if there are any cascading revocations and does not carry out the revocation.
```
- The keyword **CASCADE** can be used to force the system to carry out the revocation even if there are cascading revocations.
```sql
REVOKE SELECT ON department FROM Amit, Satoshi CASCADE;
```
- <font color='red'>**GRANTED BY CURRENT_ROLE** allows a user to grant a privilege from the current role rather than from the current user.</font>
```sql
GRANT SELECT ON department TO Amit GRANTED BY CURRENT_ROLE;
-- Suppose santoshi who is the current dean (role) has granted the select privilege on department to Amit using the above command.
-- Now even if santoshi is not the current dean, Amit can still use the select privilege on department.
-- If Santoshi didn't use the option GRANTED BY CURRENT_ROLE, then Amit would not have been able to use the select privilege on department if Santoshi is not the current dean.
```
## Row Level Authorization
- Allow access to tuples