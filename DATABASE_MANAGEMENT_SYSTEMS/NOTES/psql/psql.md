[Video](https://www.youtube.com/watch?v=qw--VYLpxG4&t=6840s)

1.  Create a Database

```sql
CREATE DATABASE test;
```

2.  Connect to the database

a.  
```sql
psql -h localhost -p 5432 -U 21CS10016 21CS10016
```
b. 
After logging in
```sql
\c 21CS10016
```

3. Delete the database

```sql
DROP DATABASE test;
```

4. Create a new table

#### Syntax:

```txt
CREATE TABLE table_name (
    column_name + datatype +constraints if any
)
  
EXAMPLE:

CREATE TABLE person (
    id int
    first_name VARCHAR(50),
    last_name VARCHAR(50),
    gender VARCHAR(6),
    date_of_birth TIMESTAMP
);
```

[Datatypes](https://www.postgresql.org/docs/16/datatype.html)

#### Delete a table

```sql
DROP TABLE person;
```

#### Table with constraints:

```sql
CREATE TABLE person (
    id BIGSERIAL NOT NULL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    gender VARCHAR(7) NOT NULL,
    date_of_birth DATE NOT NULL,
    email VARCHAR(50));
```

5. Insert records into the table.

```sql
INSERT INTO person (first_name,
last_name,
gender,
date_of_birth)
VALUES ('Anne','Smith','FEMALE',DATE '1988-01-09');
```

[Website](https://mockaroo.com/)

6. Select

```sql
SELECT email from person;
```

Ordering
```sql
SELECT * FROM person ORDER BY country_of_birth;
SELECT * FROM person ORDER BY country_of_birth ASC;
SELECT * FROM person ORDER BY country_of_birth DESC;
SELECT * FROM person ORDER BY id,email ASC;
```

7. Distinct

```sql
SELECT DISTINCT country_of_birth FROM person ORDER BY country_of_birth ASC;
```

8. Where clause and AND

```sql
SELECT * from person WHERE gender = 'Female';

SELECT * from person WHERE gender = 'Male' AND country_of_birth = 'Poland' ;

SELECT * from person WHERE gender = 'Male' AND (country_of_birth='Poland' OR country_of_birth = 'China') ;

SELECT * from person WHERE gender = 'Male' AND (country_of_birth='Poland' OR country_of_birth = 'China') AND last_name = 'Cuel';
```

9. Comparison

```sql
SELECT * FROM person LIMIT 10;

SELECT * FROM person OFFSET 5 LIMIT 5;

SELECT * FROM person OFFSET 5 FETCH FIRST 5 ROW ONLY;
```

10. IN

```sql
SELECT * FROM person WHERE country_of_birth IN ('China','Brazil','France');

SELECT * FROM person WHERE country_of_birth IN ('China','Brazil','France','Mexico','Portugal','Nigeria') ORDER BY country_of_birth;
```

11. Between

```sql
SELECT * FROM person WHERE date_of_birth BETWEEN DATE '2017-03-04' AND '2018-03-04';
```

12. Like and iLike

```sql
SELECT * FROM person WHERE email LIKE '%.com';

SELECT * FROM person WHERE email LIKE '%@google.%';

SELECT * FROM person WHERE email LIKE '________@%';
```
##### Like is case sensitive and iLike is case insensitive

13. Group by

```sql 
SELECT country_of_birth FROM person GROUP BY country_of_birth; 

SELECT country_of_birth,COUNT(*) FROM person GROUP BY country_of_birth;

SELECT country_of_birth,COUNT(*) FROM person GROUP BY country_of_birth ORDER BY country_of_birth;
```

14. Group by Having

```sql
SELECT country_of_birth,COUNT(*) FROM person GROUP BY country_of_birth HAVING COUNT(*)>5 ORDER BY country_of_birth;
```

[Aggregate Functions](https://www.postgresql.org/docs/16/functions-aggregate.html)

15. Min,Max and Average

```sql
SELECT MAX(price) FROM car;

SELECT MIN(price) FROM car;

SELECT AVG(price) FROM car;

SELECT ROUND(AVG(price)) FROM car;

SELECT make, model, MIN(price) FROM car GROUP BY make,model;

-- Minimum price for every make and mode
```

16. Sum

```sql
SELECT SUM(price) FROM car;

SELECT make,SUM(price) FROM car GROUP BY make;
```

17. Arithmetic Operations

```sql
SELECT id, make,model,price,price*.10 from car;

SELECT id, make,model,price,ROUND(price*.10,2) from car;

SELECT id, make,model,price,ROUND(price*.10,2),ROUND(price-(price*0.10),2) from car;
```

18. Alias

```sql
SELECT id,make,model,price AS original_price,ROUND(price*.10,2) AS ten_percent,ROUND(price-(price*0.10),2) as discount_after_10_percent from car;
```

19. Coalesce

```sql
21CS10016=> SELECT COALESCE(1);
--  coalesce 
-- ----------
--         1
-- (1 row)

21CS10016=> SELECT COALESCE(1) AS number;
--  number 
-- --------
--       1
-- (1 row)

21CS10016=> SELECT COALESCE(null,1) AS number;
--  number 
-- --------
--       1
-- (1 row)

21CS10016=> SELECT COALESCE(null,null,1) AS number;
--  number 
-- --------
--       1
-- (1 row)

21CS10016=> SELECT COALESCE(null,null,1,10) AS number;
--  number 
-- --------
--       1
-- (1 row)


SELECT COALESCE(email,'Email not provided') from person;
```

20. NULLIF

```sql
21CS10016=> SELECT 10/0;
-- ERROR:  division by zero

21CS10016=> SELECT NULLIF(10,10);
--  nullif 
-- --------
       
-- (1 row)

21CS10016=> SELECT NULLIF(10,1);
--  nullif 
-- --------
--      10
-- (1 row)

21CS10016=> SELECT NULLIF(10,19);
--  nullif 
-- --------
--      10
-- (1 row)

21CS10016=> SELECT NULLIF(100,19);
--  nullif 
-- --------
--     100
-- (1 row)

21CS10016=> SELECT NULLIF(100,100);
--  nullif 
-- --------
       
-- (1 row)

21CS10016=> SELECT 10/NULL;
--  ?column? 
-- ----------
         
-- (1 row)

21CS10016=> SELECT 10/NULLIF(2,9);
--  ?column? 
-- ----------
--         5
-- (1 row)

21CS10016=> SELECT 10/NULLIF(0,0);
--  ?column? 
-- ----------
         
-- (1 row)

21CS10016=> SELECT COALESCE(10/NULLIF(0,0),0);
--  coalesce 
-- ----------
--         0
-- (1 row)
```

21. Timestampes and Dates

```sql
21CS10016=> SELECT NOW();
--                now                
-- ----------------------------------
--  2024-01-23 23:43:55.222328+05:30
-- (1 row)

21CS10016=> SELECT NOW()::DATE;
--     now     
-- ------------
--  2024-01-23
-- (1 row)

21CS10016=> SELECT NOW()::TIME;
--        now       
-- -----------------
--  23:44:05.237252
-- (1 row)
```

[Date/Time](https://www.postgresql.org/docs/16/datatype-datetime.html)

22. Adding and Substracting on Dates

```sql
21CS10016=> SELECT NOW() - INTERVAL '1 YEAR';
--              ?column?             
-- ----------------------------------
--  2023-01-23 23:46:34.145377+05:30
-- (1 row)

21CS10016=> SELECT NOW() - INTERVAL '10 MONTHS';
--              ?column?             
-- ----------------------------------
--  2023-03-23 23:47:36.421619+05:30
-- (1 row)

21CS10016=> SELECT NOW()::DATE + INTERVAL '10 MONTHS';
--       ?column?       
-- ---------------------
--  2024-11-23 00:00:00
-- (1 row)
```

23. Extracting Fields

```sql
21CS10016=> SELECT EXTRACT(YEAR FROM NOW());
--  extract 
-- ---------
--     2024
-- (1 row)

21CS10016=> SELECT EXTRACT(MONTH FROM NOW());
--  extract 
-- ---------
--        1
-- (1 row)

21CS10016=> SELECT EXTRACT(DAY FROM NOW());
--  extract 
-- ---------
--       23
-- (1 row)

21CS10016=> SELECT EXTRACT(DOW FROM NOW());
--  extract 
-- ---------
--        2
-- (1 row)

21CS10016=> SELECT EXTRACT(CENTURY FROM NOW());
--  extract 
-- ---------
--       21
-- (1 row)
```

24. Age Function

```sql
ELECT first_name,last_name,gender,country_of_birth,date_of_birth,age(NOW(),date_of_birth) FROM person;
```

25. Primary Keys
##### Importance
```sql
21CS10016=> \d person   
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (1,'Spence', 'Lovett', 'slovett0@i2i.jp', 'Male', '2018-03-04', 'Indonesia');
-- ERROR:  duplicate key value violates unique constraint "person_pkey"
-- DETAIL:  Key (id)=(1) already exists.
21CS10016=> ALTER TABLE person DROP CONSTRAINT person_pkey;
-- ALTER TABLE
21CS10016=>  \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (1,'Spence', 'Lovett', 'slovett0@i2i.jp', 'Male', '2018-03-04', 'Indonesia');
-- INSERT 0 1
21CS10016=> SELECT * FROM person WHERE id = 1;
--  id | first_name | last_name |      email      | gender | date_of_birth | country
-- _of_birth 
-- ----+------------+-----------+-----------------+--------+---------------+--------
-- ----------
--   1 | Spence     | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indones
-- ia
--   1 | Spence     | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indones
-- ia
-- (2 rows)
```

##### Adding Primary Key

```sql
21CS10016=> ALTER TABLE person ADD PRIMARY KEY (id);
-- ERROR:  could not create unique index "person_pkey"
-- DETAIL:  Key (id)=(1) is duplicated.
-- 21CS10016=> DELETE FROM person WHERE id = 1;
-- DELETE 2
-- 21CS10016=> SELECT * FROM person WHERE id = 1;
--  id | first_name | last_name | email | gender | date_of_birth | country_of_birth 
-- ----+------------+-----------+-------+--------+---------------+------------------
-- (0 rows)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (1,'Spence', 'Lovett', 'slovett0@i2i.jp', 'Male', '2018-03-04', 'Indonesia');
-- INSERT 0 1
21CS10016=> SELECT * FROM person WHERE id = 1;
--  id | first_name | last_name |      email      | gender | date_of_birth | country
-- _of_birth 
-- ----+------------+-----------+-----------------+--------+---------------+--------
-- ----------
--   1 | Spence     | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indones
-- ia
-- (1 row)

21CS10016=> ALTER TABLE person ADD PRIMARY KEY (id);
-- ALTER TABLE
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
```
26. Unique Constraints

```sql
21CS10016=> SELECT email,count(*) FROM person GROUP BY email HAVING COUNT(*)>1;
--  email | count 
-- -------+-------
--        |   312
-- (1 row)

21CS10016=> insert into person (first_name, last_name, email, gender, date_of_birth, country_of_birth) values ('Fernanda', 'Lovett', 'slovett0@i2i.jp', 'Male', '2018-03-04', 'Indonesia');
-- INSERT 0 1
21CS10016=> SELECT email,count(*) FROM person GROUP BY email HAVING COUNT(*)>1;
--       email      | count 
-- -----------------+-------
--                  |   312
--  slovett0@i2i.jp |     2
-- (2 rows)

21CS10016=> SELECT * FROM person WHERE email = 'slovett0@i2i.jp';
--   id  | first_name | last_name |      email      | gender | date_of_birth | count
-- ry_of_birth 
-- ------+------------+-----------+-----------------+--------+---------------+------
-- ------------
--     1 | Spence     | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indon
-- esia
--  1001 | Fernanda   | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indon
-- esia
-- (2 rows)

21CS10016=> ALTER TABLE person ADD CONSTRAINT unique_email_address UNIQUE (email);
-- ERROR:  could not create unique index "unique_email_address"
-- DETAIL:  Key (email)=(slovett0@i2i.jp) is duplicated.
21CS10016=> DELETE FROM person WHERE id = 1001;                
-- DELETE 1
21CS10016=> SELECT * FROM person WHERE email = 'slovett0@i2i.jp';
--  id | first_name | last_name |      email      | gender | date_of_birth | country
-- _of_birth 
-- ----+------------+-----------+-----------------+--------+---------------+--------
-- ----------
--   1 | Spence     | Lovett    | slovett0@i2i.jp | Male   | 2018-03-04    | Indones
-- ia
-- (1 row)

21CS10016=> ALTER TABLE person ADD CONSTRAINT unique_email_address UNIQUE (email);
-- ALTER TABLE       
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
--     "unique_email_address" UNIQUE CONSTRAINT, btree (email)

21CS10016=> insert into person (first_name, last_name, email, gender, date_of_birth, country_of_birth) values ('Fernanda', 'Lovett', 'slovett0@i2i.jp', 'Male', '2018-03-04', 'Indonesia');
-- ERROR:  duplicate key value violates unique constraint "unique_email_address"
-- DETAIL:  Key (email)=(slovett0@i2i.jp) already exists.
21CS10016=> ALTER TABLE person DROP CONSTRAINT unique_email_address;
-- ALTER TABLE
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)

21CS10016=> ALTER TABLE person ADD UNIQUE (email);
-- ALTER TABLE
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
--     "person_email_key" UNIQUE CONSTRAINT, btree (email)
```

27. Check Constraints

```sql
21CS10016=> insert into person (first_name, last_name, email, gender, date_of_birth, country_of_birth) values ('Fernanda', 'Lovett', 'hello@i2i.jp', 'Hello', '2018-03-04', 'Indonesia');
-- INSERT 0 1
21CS10016=> SELECT DISTINCT gender FROM person;
--    gender    
-- -------------
--  Agender
--  Hello
--  Genderqueer
--  Bigender
--  Genderfluid
--  Male
--  Polygender
--  Non-binary
--  Female
-- (9 rows)

21CS10016=> ALTER TABLE person ADD CONSTRAINT gender_constraint CHECK (gender = 'Agender' OR gender = 'Genderqueer' OR gender = 'Bigender' OR gender = 'Genderfluid' OR gender = 'Male' OR gender = 'Polygender' OR gender = 'Non-binary' OR gender = 'Female');
-- ERROR:  check constraint "gender_constraint" of relation "person" is violated by some row
21CS10016=> DELETE FROM person WHERE gender = 'Hello';
-- DELETE 1
21CS10016=> ALTER TABLE person ADD CONSTRAINT gender_constraint CHECK (gender = 'Agender' OR gender = 'Genderqueer' OR gender = 'Bigender' OR gender = 'Genderfluid' OR gender = 'Male' OR gender = 'Polygender' OR gender = 'Non-binary' OR gender = 'Female');
-- ALTER TABLE
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
--     "person_email_key" UNIQUE CONSTRAINT, btree (email)
-- Check constraints:
--     "gender_constraint" CHECK (gender::text = 'Agender'::text OR gender::text = '
-- Genderqueer'::text OR gender::text = 'Bigender'::text OR gender::text = 'Genderfl
-- uid'::text OR gender::text = 'Male'::text OR gender::text = 'Polygender'::text OR
--  gender::text = 'Non-binary'::text OR gender::text = 'Female'::text)

21CS10016=> insert into person (first_name, last_name, email, gender, date_of_birth, country_of_birth) values ('Fernanda', 'Lovett', 'hello@i2i.jp', 'Hello', '2018-03-04', 'Indonesia');
-- ERROR:  new row for relation "person" violates check constraint "gender_constraint"
-- DETAIL:  Failing row contains (1004, Fernanda, Lovett, hello@i2i.jp, Hello, 2018-03-04, Indonesia).
21CS10016=> insert into person (first_name, last_name, email, gender, date_of_birth, country_of_birth) values ('Fernanda', 'Lovett', 'hello@i2i.jp', 'Lol', '2018-03-04', 'Indonesia');
-- ERROR:  new row for relation "person" violates check constraint "gender_constraint"
-- DETAIL:  Failing row contains (1005, Fernanda, Lovett, hello@i2i.jp, Lol, 2018-03-04, Indonesia).
```

28. Delete Records

```sql
21CS10016=> DELETE FROM person;
-- DELETE 1000
21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | email | gender | date_of_birth | country_of_birth 
-- ----+------------+-----------+-------+--------+---------------+------------------
-- (0 rows)

21CS10016=> \i /home/bt1/21CS10016/person.sql
--
-- (Insertion Happens Here)
--

21CS10016=> SELECT * FROM person;
--   id  |  first_name  |    last_name     |                 email                 |
--    gender    | date_of_birth |     country_of_birth     
-- ------+--------------+------------------+---------------------------------------+
-- -------------+---------------+--------------------------
--  1006 | Spence       | Lovett           | slovett0@i2i.jp                       |
--  Male        | 2018-03-04    | Indonesia
--  1007 | Diana        | Slocumb          | dslocumb1@ibm.com                     |
--  Female      | 2018-08-11    | China
--  1008 | Vanni        | Couch            | vcouch2@hibu.com                      |
--  Female      | 2018-08-16    | Indonesia
--  1009 | Nerta        | Revel            |                                       |
--  Female      | 2018-05-29    | Indonesia
--  1010 | Jae          | Attoe            | jattoe4@wikimedia.org                 |
--  Male        | 2018-07-28    | China
--  1011 | Mickey       | Piperley         | mpiperley5@woothemes.com              |
--  Male        | 2018-08-20    | Philippines
--  1012 | Bobina       | Dubose           | bdubose6@un.org                       |
--  Female      | 2018-03-07    | Poland
--  1013 | Jayne        | Ucceli           |                                       |
--  Female      | 2018-05-30    | Luxembourg


21CS10016=> DELETE FROM person where id = 1006;
-- DELETE 1

21CS10016=> DELETE FROM person where gender = 'Female' and country_of_birth = 'Nigeria';
-- DELETE 3

21CS10016=> SELECT * FROM person where gender = 'Female' and country_of_birth = 'Nigeria';
--  id | first_name | last_name | email | gender | date_of_birth | country_of_birth 
-- ----+------------+-----------+-------+--------+---------------+------------------
-- (0 rows)

21CS10016=> SELECT * FROM person where gender = 'Male' and country_of_birth = 'Nigeria';
--   id  | first_name | last_name | email | gender | date_of_birth | country_of_birt
-- h 
-- ------+------------+-----------+-------+--------+---------------+----------------
-- --
--  1427 | Norbert    | Redparth  |       | Male   | 2018-10-17    | Nigeria
-- (1 row)

21CS10016=> DELETE FROM person WHERE gender = 'Male';
-- DELETE 444

21CS10016=> DELETE FROM person;
-- DELETE 552
```

29. Update Records

```sql
21CS10016=> SELECT * FROM PERSON LIMIT 5;
--   id  | first_name | last_name |          email           | gender | date_of_birt
-- h | country_of_birth 
-- ------+------------+-----------+--------------------------+--------+-------------
-- --+------------------
--  2008 | Vanni      | Couch     | vcouch2@hibu.com         | Female | 2018-08-16  
--   | Indonesia
--  2009 | Nerta      | Revel     |                          | Female | 2018-05-29  
--   | Indonesia
--  2010 | Jae        | Attoe     | jattoe4@wikimedia.org    | Male   | 2018-07-28  
--   | China
--  2011 | Mickey     | Piperley  | mpiperley5@woothemes.com | Male   | 2018-08-20  
--   | Philippines
--  2012 | Bobina     | Dubose    | bdubose6@un.org          | Female | 2018-03-07  
--   | Poland
-- (5 rows)

21CS10016=> UPDATE person SET email = 'omar@gmail.com' WHERE id = 2008;
-- UPDATE 1
21CS10016=> SELECT * FROM PERSON WHERE id = 2008;
--   id  | first_name | last_name |     email      | gender | date_of_birth | countr
-- y_of_birth 
-- ------+------------+-----------+----------------+--------+---------------+-------
-- -----------
--  2008 | Vanni      | Couch     | omar@gmail.com | Female | 2018-08-16    | Indone
-- sia
-- (1 row)
```

30. On conflict Do nothing

```sql
21CS10016=> SELECT * FROM PERSON WHERE id = 2017;
--   id  | first_name | last_name |            email            | gender | date_of_b
-- irth | country_of_birth 
-- ------+------------+-----------+-----------------------------+--------+----------
-- -----+------------------
--  2017 | Xylia      | Grimsdyke | xgrimsdykeb@list-manage.com | Female | 2018-01-2
-- 5    | Moldova
-- (1 row)

-- 21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova');
-- ERROR:  duplicate key value violates unique constraint "person_pkey"
-- DETAIL:  Key (id)=(2017) already exists.
21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (id) DO NOTHING;
-- INSERT 0 0
21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              
-- Default               
-- ------------------+------------------------+-----------+----------+--------------
-- ----------------------
--  id               | bigint                 |           | not null | nextval('pers
-- on_id_seq'::regclass)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  email            | character varying(150) |           |          | 
--  gender           | character varying(50)  |           | not null | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
--     "person_email_key" UNIQUE CONSTRAINT, btree (email)
-- Check constraints:
--     "gender_constraint" CHECK (gender::text = 'Agender'::text OR gender::text = '
-- Genderqueer'::text OR gender::text = 'Bigender'::text OR gender::text = 'Genderfl
-- uid'::text OR gender::text = 'Male'::text OR gender::text = 'Polygender'::text OR
--  gender::text = 'Non-binary'::text OR gender::text = 'Female'::text)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (email) DO NOTHING; 
-- INSERT 0 0
21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (first_name) DO NOTHING;
-- ERROR:  there is no unique or exclusion constraint matching the ON CONFLICT specification
```

31. Upsert

```sql
21CS10016=> SELECT * FROM PERSON WHERE id = 2017;
--   id  | first_name | last_name |            email            | gender | date_of_birth | country_of_b
-- irth 
-- ------+------------+-----------+-----------------------------+--------+---------------+-------------
-- -----
--  2017 | Xylia      | Grimsdyke | xgrimsdykeb@list-manage.com | Female | 2018-01-25    | Moldova
-- (1 row)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (id) DO NOTHING;
-- INSERT 0 0
21CS10016=> SELECT * FROM PERSON WHERE id = 2017;
--   id  | first_name | last_name |            email            | gender | date_of_birth | country_of_b
-- irth 
-- ------+------------+-----------+-----------------------------+--------+---------------+-------------
-- -----
--  2017 | Xylia      | Grimsdyke | xgrimsdykeb@list-manage.com | Female | 2018-01-25    | Moldova
-- (1 row)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (id) DO UPDATE SET email  = EXCLUDED.email;
-- INSERT 0 1
21CS10016=> SELECT * FROM PERSON WHERE id = 2017;
--   id  | first_name | last_name |            email            | gender | date_of_birth | country_of_b
-- irth 
-- ------+------------+-----------+-----------------------------+--------+---------------+-------------
-- -----
--  2017 | Xylia      | Grimsdyke | xgrimsdykeb@list-manage.com | Female | 2018-01-25    | Moldova
-- (1 row)

21CS10016=> insert into person (id,first_name, last_name, email, gender, date_of_birth, country_of_birth) values (2017,'Xylia', 'Grimsdyke', 'xgrimsdykeb@list-manage.com.uk', 'Female', DATE '2018-01-25', 'Moldova') ON CONFLICT (id) DO UPDATE SET email  = EXCLUDED.email;
-- INSERT 0 1
21CS10016=> SELECT * FROM PERSON WHERE id = 2017;
--   id  | first_name | last_name |             email              | gender | date_of_birth | country_o
-- f_birth 
-- ------+------------+-----------+--------------------------------+--------+---------------+----------
-- --------
--  2017 | Xylia      | Grimsdyke | xgrimsdykeb@list-manage.com.uk | Female | 2018-01-25    | Moldova
-- (1 row)
```

32. Foreign Keys, Joins and Relationships

##### Updating Foreign Key Column
```sql
21CS10016=> UPDATE person SET car_id = 2 WHERE id = 1;
-- UPDATE 1
21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
   
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
-- (3 rows)

21CS10016=> UPDATE person SET car_id = 2 WHERE id = 2;
-- ERROR:  duplicate key value violates unique constraint "person_car_id_key"
-- DETAIL:  Key (car_id)=(2) already exists.
21CS10016=> UPDATE person SET car_id = 1 WHERE id = 2;
-- UPDATE 1
21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
-- (3 rows)

21CS10016=> SELECT * FROM car;   
--  id |    make    |  model   |  price   
-- ----+------------+----------+----------
--   1 | Land Rover | Sterling | 87665.38
--   2 | GMC        | Acadia   | 17662.69
-- (2 rows)

21CS10016=> UPDATE person SET car_id = 3 WHERE id = 1;
-- ERROR:  insert or update on table "person" violates foreign key constraint "person_car_id_fkey"
-- DETAIL:  Key (car_id)=(3) is not present in table "car".
```
##### Inner Join

```sql
21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
-- (3 rows)

21CS10016=> SELECT * FROM car;
--  id |    make    |  model   |  price   
-- ----+------------+----------+----------
--   1 | Land Rover | Sterling | 87665.38
--   2 | GMC        | Acadia   | 17662.69
-- (2 rows)

21CS10016=> SELECT * FROM person JOIN car ON person.car_id = car.id;
--  id | first_name | last_name | gender |      email      | date_of_birth | country_of_birth | car_id 
-- | id |    make    |  model   |  price   
-- ----+------------+-----------+--------+-----------------+---------------+------------------+--------
-- +----+------------+----------+----------
--   2 | Omar       | Colmore   | Male   |                 | 1921-04-03    | Finland          |      1 
-- |  1 | Land Rover | Sterling | 87665.38
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd | 1953-10-28    | Comoros          |      2 
-- |  2 | GMC        | Acadia   | 17662.69
-- (2 rows)

21CS10016=> \x
-- Expanded display is on.
21CS10016=> SELECT * FROM person JOIN car ON person.car_id = car.id;
-- -[ RECORD 1 ]----+----------------
-- id               | 2
-- first_name       | Omar
-- last_name        | Colmore
-- gender           | Male
-- email            | 
-- date_of_birth    | 1921-04-03
-- country_of_birth | Finland
-- car_id           | 1
-- id               | 1
-- make             | Land Rover
-- model            | Sterling
-- price            | 87665.38
-- -[ RECORD 2 ]----+----------------
-- id               | 1
-- first_name       | Fernanda
-- last_name        | Beardon
-- gender           | Female
-- email            | fernandab@is.gd
-- date_of_birth    | 1953-10-28
-- country_of_birth | Comoros
-- car_id           | 2
-- id               | 2
-- make             | GMC
-- model            | Acadia
-- price            | 17662.69

21CS10016=> SELECT person.first_name, car.make, car.make, car.model, car.price FROM person JOIN car ON person.car_id = car.id;
-- -[ RECORD 1 ]----------
-- first_name | Omar
-- make       | Land Rover
-- make       | Land Rover
-- model      | Sterling
-- price      | 87665.38
-- -[ RECORD 2 ]----------
-- first_name | Fernanda
-- make       | GMC
-- make       | GMC
-- model      | Acadia
-- price      | 17662.69
```

##### Left Joins

```sql
21CS10016=> SELECT * FROM person LEFT JOIN car ON person.car_id = car.id;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id | id |    make    |  model   |  price   
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----+----+------------+----------+----------
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1 |  1 | Land Rover | Sterling | 87665.38
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2 |  2 | GMC        | Acadia   | 17662.69
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
--     |    |            |          |         
-- (3 rows)

21CS10016=> SELECT * FROM person LEFT JOIN car ON person.car_id = car.id WHERE car.* IS NULL;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id | id | make | model | price 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----+----+------+-------+-------
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
--     |    |      |       |      
-- (1 row)
```

##### Deleting Records with Foreign Keys

```sql
21CS10016=> insert into car (id, make, model, price) values (13, 'Mazda', 'RX-8', '51272.48');

-- INSERT 0 1

21CS10016=> insert into person (id, first_name, last_name, gender, email, date_of_birth, country_of_birth) values (9000, 'John', 'Smith', 'Male', null, '1921-04-03', 'Finland');
-- INSERT 0 1

21CS10016=> SELECT * FROM person where id = 9000;
--   id  | first_name | last_name | gender | email | date_of_birth | country_of_birth | car_id 
-- ------+------------+-----------+--------+-------+---------------+------------------+--------
--  9000 | John       | Smith     | Male   |       | 1921-04-03    | Finland          |       
-- (1 row)

21CS10016=> SELECT * FROM car where id = 13;
--  id | make  | model |  price   
-- ----+-------+-------+----------
--  13 | Mazda | RX-8  | 51272.48
-- (1 row)

21CS10016=> UPDATE person SET car_id = 13  WHERE id = 9000;
-- UPDATE 1
21CS10016=> SELECT * FROM person where id = 9000;
--   id  | first_name | last_name | gender | email | date_of_birth | country_of_birth | car_id 
-- ------+------------+-----------+--------+-------+---------------+------------------+--------
--  9000 | John       | Smith     | Male   |       | 1921-04-03    | Finland          |     13
-- (1 row)

21CS10016=> SELECT * FROM car where id = 13;
--  id | make  | model |  price   
-- ----+-------+-------+----------
--  13 | Mazda | RX-8  | 51272.48
-- (1 row)

21CS10016=> DELETE FROM car WHERE id = 13;
-- ERROR:  update or delete on table "car" violates foreign key constraint "person_car_id_fkey" on table "person"
-- DETAIL:  Key (id)=(13) is still referenced from table "person".
21CS10016=> DELETE FROM person WHERE id = 9000;
-- DELETE 1
21CS10016=> SELECT * FROM person where id = 9000;
--  id | first_name | last_name | gender | email | date_of_birth | country_of_birth | car_id 
-- ----+------------+-----------+--------+-------+---------------+------------------+--------
-- (0 rows)

21CS10016=> DELETE FROM car WHERE id = 13;
-- DELETE 1
21CS10016=> SELECT * FROM car where id = 13;
--  id | make | model | price 
-- ----+------+-------+-------
-- (0 rows)
```

33. Exporting Query Results to CSV

```sql
21CS10016=> \copy (SELECT * FROM person LEFT JOIN car ON car.id = person.car_id) TO '/home/bt1/21CS10016/results.csv' DELIMITER ',' CSV HEADER;
-- COPY 3
```

34. Serial & Sequences

```sql
21CS10016=> SELECT * FROM person_id_seq;
--  last_value | log_cnt | is_called 
-- ------------+---------+-----------
--           3 |      30 | t
-- (1 row)

21CS10016=> SELECT * FROM person;       
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
-- (3 rows)

21CS10016=> \d person
--                                          Table "public.person"
--       Column      |          Type          | Collation | Nullable |              Default            
   
-- ------------------+------------------------+-----------+----------+---------------------------------
-- ---
--  id               | bigint                 |           | not null | nextval('person_id_seq'::regclas
-- s)
--  first_name       | character varying(50)  |           | not null | 
--  last_name        | character varying(50)  |           | not null | 
--  gender           | character varying(7)   |           | not null | 
--  email            | character varying(100) |           |          | 
--  date_of_birth    | date                   |           | not null | 
--  country_of_birth | character varying(50)  |           | not null | 
--  car_id           | bigint                 |           |          | 
-- Indexes:
--     "person_pkey" PRIMARY KEY, btree (id)
--     "person_car_id_key" UNIQUE CONSTRAINT, btree (car_id)
-- Foreign-key constraints:
--     "person_car_id_fkey" FOREIGN KEY (car_id) REFERENCES car(id)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        4
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        5
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        6
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        7
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        8
-- (1 row)

21CS10016=> SELECT * FROM person_id_seq;
--  last_value | log_cnt | is_called 
-- ------------+---------+-----------
--           8 |      28 | t
-- (1 row)

21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
-- (3 rows)

21CS10016=> insert into person (first_name, last_name, gender, email, date_of_birth, country_of_birth) values ('John', 'Matuschek', 'Male', 'john@feedburner.com', '1965-02-28', 'England');
-- INSERT 0 1
21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
--   9 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
-- (4 rows)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       10
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       11
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       12
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       13
-- (1 row)

21CS10016=> SELECT * FROM person_id_seq;
--  last_value | log_cnt | is_called 
-- ------------+---------+-----------
--          13 |      28 | t
-- (1 row)

21CS10016=> SELECT * FROM person;
--  id | first_name | last_name | gender |        email        | date_of_birth | country_of_birth | car
-- _id 
-- ----+------------+-----------+--------+---------------------+---------------+------------------+----
-- ----
--   3 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
--   1 | Fernanda   | Beardon   | Female | fernandab@is.gd     | 1953-10-28    | Comoros          |    
--   2
--   2 | Omar       | Colmore   | Male   |                     | 1921-04-03    | Finland          |    
--   1
--   9 | John       | Matuschek | Male   | john@feedburner.com | 1965-02-28    | England          |    
   
-- (4 rows)

21CS10016=> ALTER SEQUENCE person_id_seq RESTART WITH 9; 
-- ALTER SEQUENCE
21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--        9
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       10
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       11
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       12
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       13
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       14
-- (1 row)

21CS10016=> SELECT nextval('person_id_seq'::regclass);
--  nextval 
-- ---------
--       15
-- (1 row)
```

35. Extensions

```sql
SELECT * FROM pg_available_extensions;
```

36. UUID Data Type

[Wikipedia](https://en.wikipedia.org/wiki/Universally_unique_identifier)

```sql
21CS10016=> CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
-- CREATE EXTENSION

21CS10016=> \df
--                                  List of functions
--  Schema |        Name        | Result data type |    Argument data types    | Type 
-- --------+--------------------+------------------+---------------------------+------
--  public | uuid_generate_v1   | uuid             |                           | func
--  public | uuid_generate_v1mc | uuid             |                           | func
--  public | uuid_generate_v3   | uuid             | namespace uuid, name text | func
--  public | uuid_generate_v4   | uuid             |                           | func
--  public | uuid_generate_v5   | uuid             | namespace uuid, name text | func
--  public | uuid_nil           | uuid             |                           | func
--  public | uuid_ns_dns        | uuid             |                           | func
--  public | uuid_ns_oid        | uuid             |                           | func
--  public | uuid_ns_url        | uuid             |                           | func
--  public | uuid_ns_x500       | uuid             |                           | func
-- (10 rows)

21CS10016=> SELECT uuid_generate_v4();
           uuid_generate_v4           
-- --------------------------------------
--  0626972a-f88d-42b4-9d6e-7209a75fa83d
-- (1 row)
```

37. UUID as Primary Keys

```sql
SELECT * FROM person JOIN car USING (car_uid);
```