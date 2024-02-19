# Functions and Procedures
# Functions
```sql
create function dept_count(dept_name varchar(20))
returns integer
begin
declare d_count integer;
select count(*) into d_count
from instructor
where instructor.dept_name = dept_name;
return d_count;
end
--
select dept_name,budget
from department
where dept_count(dept_name) >12;
```
```sql
create function instructor_of(dept_name varchar(20))
returns table(
    ID varchar(5),
    name varchar(20),
    dept_name varchar(20),
    salary numeric(8,2))
    return table
    (select ID, name, dept_name, salary
    from instructor
    where instructor.dept_name = dept_name);
--
select * 
from table(instructor_of('Physics'));
```
## Procedures
```sql
create procedure dept_count_proc(in dept_name varchar(20), out d_count integer)
begin
select count(*) into d_count
from instructor
where instructor.dept_name = dept_count_proc.dept_name;
end
--
declare d_count integer;
call dept_count_proc('Physics', d_count);
```

### Language Constructs
```sql
create function registerStudent(
    in s_id varchar(5),
    in s_course_id varchar(8),
    in s_sec_id varchar(8),
    in s_semester varchar(6),
    in s_year numeric(4,0),
    out erroMsg varchar(100)
returns integer
begin
    declare currEnrol int;
    select count(*) into currEnrol
        from takes
        where course_id = s_course_id and sec_id = s_sec_id and semester = s_semester and year = s_year;
    declare limit int;
    select capacity into limit
        from classroom natural join section
        where course_id = s_course_id and sec_id = s_sec_id and semester = s_semester and year = s_year;
    if (curEnrol < limit)
        begin
            insert into takes values (s_id, s_course_id, s_sec_id, s_semester, s_year, null);
            return (0);
        end
    set errorMsg = 'Enrollment limit reached for course ' || s_course_id || ' section ' || s_sec_id;
    return (-1);
end;
```