**Introduction to SQL**

1. DDL (Data Definition Language)
2. DML (Data Manipulation Language)

**Basic Types**

1. char(n): fixed-length string
2. varchar(n): variable-length string
3. int: integer
4. smallint: small integer
5. numeric(p, d): fixed-point number. The number consists of p digits, d of which are after the decimal point.
6. real: single precision floating point number
7. double precision: double precision floating point number
8. float(n): floating point number with n digits of precision

Each type may include a special value, NULL, which means "unknown" or "not applicable".

### Notes
- Char stores fixed-length strings. Consider an attribute A of type char(10). If we store a string "abc" in A, it will be stored as "abc       " (with 7 spaces at the end).
- In contrast, varchar stores variable-length strings. If we store a string "abc" in an attribute B of type varchar(10), it will be stored as "abc".
- When comapring two values of type char, if they are of different lengths, the shorter one is padded with spaces to make it the same length as the longer one. For example, 'abc' = 'abc       '.
- When comparing a char type with a varchar type, one may expect that the shorter one is padded with spaces to make it the same length as the longer one. However, this is not the case. For example, 'abc' = 'abc'.

**Basic Schema Definition**

```sql
CREATE TABLE department (
    dept_name varchar(20),
    building varchar(15),
    budget numeric(12, 2),
    primary key (dept_name)
);
```

### Notes
1. primary key: nonnull and unique
2. foreign key: references a primary key in another table
**foreign key** (dept_name) **references** department(dept_name)
or 
**foreign key** (dept_name) **references** department
3. not null: spcifies that the attribute cannot be null
4. To remove a table from the database, use the DROP TABLE statement.
```sql
DROP TABLE department;
```
This command deletes all information about the table from the database.

Alternatively, the DELETE statement can be used to remove all tuples from a table, but the table itself remains in the database.
```sql
DELETE FROM department;
```
6. **alter table** command add attributes to a table, delete attributes from a table, or modify the type of an attribute.
```sql
ALTER TABLE department ADD capacity int;
ALTER TABLE department DROP capacity;
```

**Basic structure of SQL queries**
```sql
SELECT name FROM instructor;
-- Eliminate duplicates
SELECT DISTINCT name FROM instructor;
-- Explicitly specify not to eliminate duplicates
SELECT ALL name FROM instructor;
```
- **select** clause may also contain arithmetic expressions.
```sql
SELECT ID,name,detp_name,salary*1.1 FROM instructor;
```
- **where** clause
```sql
SELECT name FROM instructor WHERE dept_name = 'Comp. Sci.' AND salary > 70000;
```
- SQL allows the use of the logical connectives **AND**, **OR**, and **NOT**.

**Queries on multiple tables**
```sql
SELECT name,instructor.dept_name,building FROM instructor,department WHERE instructor.dept_name = department.dept_name;
```
- Note that the attribute dept_name appears in both tables. To avoid ambiguity, we can use the dot notation to specify the table to which the attribute belongs.

**Additional Basic Operations**

#### The rename operation
- The **AS** clause is used to rename the tables and attributes in the result of a query.
- Rename attributes
```sql
SELECT name AS instructor_name,course_id FROM instructor,teaches WHERE instructor.ID = teaches.ID;
```
- Rename tables
```sql
SELECT T.name, S.course_id FROM instructor AS T, teaches AS S WHERE T.ID = S.ID;
```
- Compare tuples in the same table
```sql
SELECT DISTINCT T.name FROM instructor AS T, instructor AS S WHERE T.salary > S.salary and S.dept_name = 'Biology';
```
- T and S are declared as aliases for the instructor table. An identifier such as T and S, that is used to rename a relation is referred to as a **correlation name** in the SQL standard. But it is also commonly known as a **table alias**, or a **correlation variable**, or a **tuple variable**.

#### String Operations
- SQL specifies strings by enclosing them in single quotes. For example, 'abc' is a string of length 3. A single quote within a string is represented by two consecutive single quotes. For example, "It's right" is represented as 'It''s right'.
- Equality operation on strings is case-sensitive.
- SQL permits a variety of string operations, including concatenating ( || ), extracting substrings, finding the length of a string, and converting strings to uppercase ( using the function **upper(s)** ) or lowercase ( using the function **lower(s)** ), removing spaces at the end of the string ( using the function **trim(s)** ), and so on.
- **Pattern Matching**:
- **LIKE** opertor is used to match a string value against a pattern. The pattern may include two special characters:
- **%**: matches any substring of zero or more characters.
- **_**: matches any single character.
- Patterns are case-sensitive.
```sql
SELECT dept_name FROM department WHERE building LIKE '%WATSON%';
```
- For patterns to include the special characters % and _, we can use the **ESCAPE** clause.
```sql
LIKE 'ab\%cd%' ESCAPE '\' -- matches strings that start with 'ab%cd'
```
- SQL allows us to search for mismatched patterns using the **NOT LIKE** operator.
#### Attribute specification in  the select clause
- The asterisk (*) is used to specify that all attributes of the relation are to be included in the result.
```sql
SELECT instructor.* FROM instructor, teaches WHERE instructor.ID = teaches.ID;
-- It indicates that all attributes of the instructor relation are to be included in the result.
```

```sql
SELECT * FROM instructor, teaches WHERE instructor.ID = teaches.ID;
-- It indicates that all attributes of the instructor and teaches relations are to be included in the result.
```

#### Ordering the Display of Tuples
- The **ORDER BY** clause is used to sort the result of a query in ascending or descending order.
```sql
SELECT name FROM instructor WHERE dept_name = 'Comp. Sci.' ORDER BY name;
```
- By default, the **ORDER BY** clause sorts the result in ascending order. To sort the result in descending order, we use the **DESC** keyword and to sort the result in ascending order, we use the **ASC** keyword.
```sql
SELECT * FROM instructor ORDER BY salary DESCm name ASC;
```
#### Where-Clause Predicates
- SQL allows the use of the **BETWEEN** predicate to specify a range of values.
```sql
SELECT name FROM instructor WHERE salary BETWEEN 70000 AND 90000;
-- instead of
SELECT name FROM instructor WHERE salary >= 70000 AND salary <= 90000;
```
- Similarly, we can use the **NOT BETWEEN** predicate to specify a range of values that are not to be included in the result.
- SQL permits us to use the notation $(v1, v2, ..., vn)$ to denote a tuple of arity n containing the values $v1, v2, ..., vn$; the notation is called a row constructor.
```sql
SELECT name,course_id FROM instructor, teaches WHERE (instructor.ID,detp_name) = (teaches.ID,'BIOLOGY');
```
#### Set Operations
- SQL supports the set operations **UNION**, **INTERSECT**, and **EXCEPT**.
- **UNION**
```sql
(SELECT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017)
UNION
(SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
- Parentheses are optional but used for clarity.
- <font color="red">The **UNION** operator eliminates duplicates from the result, whereas the **UNION ALL** operator does not. The number of duplicate tuples in the results is equal to the total number of duplicate tuples in the input relations.</font>
```sql
(SELECT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017)
UNION ALL
(SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
- **INTERSECT**
```sql
(SELECT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017)
INTERSECT
(SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
- <font color="red">The **INTERSECT** operator eliminates duplicates from the result, whereas the **INTERSECT ALL** operator does not. The number of duplicate tuples in the results is equal to the minimum number of duplicate tuples in the input relations.</font>
- **EXCEPT**
```sql
(SELECT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017)
EXCEPT
(SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
- <font color="red">The **EXCEPT** operator outputs the tuples that are in the first input relation but not in the second input relation that is it performs set difference. The operation automatically eliminates duplicates from the result. If we want to keep duplicates, we can use the **EXCEPT ALL** operator. The number of duplicate tuples in the results is equal to the number of duplicate tuples in the first input relation minus the number of duplicate tuples in the second input relation.</font>

**Null Values**
- The result of any arithmetic operation with a null operand is null.
- For comparison operations, SQL uses a three-valued logic: true, false, and **unknown**. The result of a comparison operation is unknown if one or both of the operands are null.
- **and** : True and Unknown is Unknown, False and Unknown is False, Unknown and Unknown is Unknown.
- **or** : True or Unknown is True, False or Unknown is Unknown, Unknown or Unknown is Unknown.
- **not** : not Unknown is Unknown.
- <font color="magenta"> If the where clause predicate evaluates to either false or unknown, the tuple is not included in the result.</font>
- SQL uses the special keyword **NULL** in a predicate to test for null values.
```sql
SELECT name FROM instructor WHERE dept_name = 'Comp. Sci.' AND salary IS NULL;
```
- The predicate **IS NULL** is used to test for null values, and the predicate **IS NOT NULL** is used to test for nonnull values.
- SQL allows us to test whether the result of a comparison is unknown using the **IS UNKNOWN** predicate.
```sql
SELECT name FROM instructor WHERE dept_name = 'Comp. Sci.' AND salary = 70000 IS UNKNOWN;
```
- <font color="red"> **SELECT DISTINCT** clause for $\{('\text{A}',\text{NULL}),('\text{A}',\text{NULL})\}$ will return $\{('\text{A}',\text{NULL})\}$ only once.
- But Note that the comparison NULL=NULL is unknown, not true. 
- This approach of treating tuples as identical if they have the same values for all attributes, even if some of the values are null, is also used for the set operations **UNION**, **INTERSECT**, and **EXCEPT**.</font>

**Aggregate Functions**
- **avg**,**min**,**max**,**sum**,**count**
- The input to **sum** and **avg** must be a collection of numbers, but the other operations can be applied to nonnumeric data as well.
```sql
SELECT avg(salary) FROM instructor WHERE dept_name = 'Comp. Sci.';

SELECT avg(salary) AS avg_salary FROM instructor WHERE dept_name = 'Comp. Sci.';
```
- <font color="red">Retaining duplicates is important while computing the average of a set of values.
- In cases duplicates need to be removed, we can use the **DISTINCT** keyword.</font>
```sql
SELECT count(DISTINCT ID) FROM teaches WHERE semester = 'Fall' AND year = 2017;
```
- Count the number of tuples in a relation
```sql
SELECT count(*) FROM instructor WHERE dept_name = 'Comp. Sci.';
```
**Aggregation with Grouping**
- **group by** clause is used to group the tuples of a relation on the basis of the values of one or more attributes.
```sql
SELECT dept_name, avg(salary) FROM instructor GROUP BY dept_name;
```
- When an SQL query used grouping, it is important to ensure that the only attrubutes in the select statement without an aggregate function are those that appear in the group by clause. Otherwise the query is erroneous.
- **having** clause is used to eliminate groups that do not satisfy a certain condition.
```sql
SELECT dept_name, avg(salary) FROM instructor GROUP BY dept_name HAVING avg(salary) > 42000;
```
- Any attribute that appears in the having clause without an aggregate function must appear in the group by clause.
```sql
SELECT course_id, semester, year, sec_id, avg (tot_cred)
FROM student , takes
WHERE student.ID = takes.ID and year = 2018
GROUP BY course_id, semester, year, sec_id
HAVING count(ID) >= 2;
```
**Aggregation with Null and Boolean Values**
- All aggregate functions except **count(*)** ignore null values in their input
- As a result of null values, the result of an aggregate function may be null.

**Nested Subqueries**
- **Set Membership**
- **IN** operator is used to test for set membership.
```sql
SELECT DISTINCT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017 AND
course_id IN (SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
```sql
SELECT DISTINCT course_id
FROM section
WHERE semester = 'Fall' AND year = 2017 AND
course_id NOT IN (SELECT course_id
FROM section
WHERE semester = 'Spring' AND year = 2018);
```
- The **IN** and **NOT IN** operators can be used to test for set membership in a subquery.
```sql
SELECT DISTINCT name 
FROM instructor
WHERE name NOT IN ('Smith','Jones','Blake');
```
- **Set Comparison**
- **>some**
```sql
SELECT name
FROM instructor
WHERE salary > some (SELECT salary
FROM instructor
WHERE dept_name = 'Comp. Sci.');
```
- Also **<some**, **>=some**, **<=some**, **=some**, **<>some** can be used.
- **>all**
```sql
SELECT name
FROM instructor
WHERE salary > all (SELECT salary
FROM instructor
WHERE dept_name = 'Comp. Sci.');
```
- Also **<all**, **>=all**, **<=all**, **=all**, **<>all** can be used.
- **Test for Empty Relations**
- The **EXISTS** construct returns true if the result of the subquery is nonempty, and false otherwise.
```sql
SELECT course_id
FROM section AS S
WHERE semester = 'Fall' AND year = 2017 AND
EXISTS (SELECT *
FROM section AS T
WHERE semester = 'Spring' AND year = 2018 AND
S.course_id = T.course_id);
```
- The above query also illustrates a feature of SQL where a correlation name in the outer query can be used in the subquery in the WHERE clause. A subquery that uses a correlation name from the outer query is called a **correlated subquery**.
- We can also use the **NOT EXISTS** construct to test for an empty result.
- **Test for tha Absence of Duplicate Tuples**
- The **UNIQUE** construct returns true if the result of the subquery has no duplicate tuples, and false otherwise.
```sql
SELECT T.course_id
FROM course AS T
WHERE NOT UNIQUE (SELECT R.course_id
FROM section AS R
WHERE R.course_id = T.course_id AND
R.year = 2017);
```
-- The **NOT UNIQUE** construct returns true if the result of the subquery has duplicate tuples, and false otherwise.
-- It is possible for **UNIQUE** to be true even if there are multiple copies of a tuple, as long as at least one of the attributes of the tuples is null.
- **Nested Subqueries in the FROM Clause**
```sql
SELECT dept_name, avg_salary
FROM (SELECT dept_name, avg(salary) AS avg_salary
FROM instructor
GROUP BY dept_name)
WHERE avg_salary > 42000;

SELECT dept_name, avg_salary
FROM (SELECT dept_name, avg(salary) AS avg_salary
FROM instructor
GROUP BY dept_name)
AS dept_avg (dept_name, avg_salary)
WHERE avg_salary > 42000;
```
- ** The WITH Clause**
- The **WITH** clause is used to define a temporary relation whose definition is available only to the query in which the **WITH** clause occurs.
```sql
WITH dept_total (dept_name, value) AS
(SELECT dept_name, sum(salary)
FROM instructor
GROUP BY dept_name),
dept_avg (dept_name, value) AS
(SELECT avg(value)
FROM dept_total)
SELECT dept_name
FROM dept_total, dept_avg
WHERE dept_total.value > dept_avg.value;
```
-**Scalar Subqueries**
```sql
SELECT dept_name, (SELECT count(*)
FROM instructor
WHERE instructor.dept_name = department.dept_name)
AS num_instructors
FROM department;
```
-**Scalar without a FROM Clause**
```sql
(SELECT count(*) FROM teaches) / (SELECT count(*) FROM instructor);
-- This is illegal in some systems. Instead, we can use the following:
SELECT (SELECT count(*) FROM teaches) / (SELECT count(*) FROM instructor)
FROM dual;
```
### Deletion
- **DELETE FROM** r WHERE P;
- The **DELETE** request can contain a nested **SELECT** that references the relation being deleted.
```sql
DELETE FROM instructor 
WHERE dept_name < (SELECT avg(salary) FROM instructor);
```
### Insertion
```sql
-- Follow the order of the attributes in the schema
INSERT INTO course
VALUES ('CS-437', 'Database Systems', 'Comp. Sci.', 4);
-- Explicitly specify the attributes
INSERT INTO course (course_id, title, dept_name, credits)
VALUES ('CS-437', 'Database Systems', 'Comp. Sci.', 4);
```
- Inserting based on the result of a query
```sql
INSERT INTO instructor
SELECT ID,name,dept_name,18000
FROM student
WHERE dept_name = 'Music' AND tot_cred > 100;
```
### Updates
```sql
UPDATE instructor
SET salary = salary * 1.10
WHERE dept_name = 'Biology';
```
- **CASE** expression
```sql
UPDATE instructor
SET salary = CASE
WHEN salary < 100000 THEN salary * 1.10
ELSE salary * 1.05
END;
```