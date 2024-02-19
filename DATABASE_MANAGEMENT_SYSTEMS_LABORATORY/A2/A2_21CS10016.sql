/**********************************************************
 Student Information:  
 -----------------------------------------------------------
 Name: Bratin Mondal
 Student ID: 21CS10016
 -----------------------------------------------------------
 Department of Computer Science and Engineering,
 Indian Institute of Technology Kharagpur.
 ***********************************************************/
/* Create the tables */
/* Role table */
CREATE TABLE Role (
    RID VARCHAR(10) PRIMARY KEY NOT NULL UNIQUE,
    Rname VARCHAR(50) NOT NULL,
    Description VARCHAR(255)
);

/* Event table */
CREATE TABLE Event (
    EID VARCHAR(10) PRIMARY KEY NOT NULL UNIQUE,
    Date DATE NOT NULL,
    Ename VARCHAR(100) NOT NULL,
    Type VARCHAR(50) NOT NULL
);

/* College table */
CREATE TABLE College (
    Name VARCHAR(100) PRIMARY KEY NOT NULL UNIQUE,
    Location VARCHAR(50) NOT NULL
);

/* Student table */
CREATE TABLE Student (
    Name VARCHAR(50) NOT NULL,
    Roll CHAR(9) PRIMARY KEY NOT NULL UNIQUE,
    Dept VARCHAR(50) NOT NULL,
    RoleID VARCHAR(10),
    FOREIGN KEY (RoleID) REFERENCES Role(RID)
);

/* Volunteer table */
CREATE TABLE Volunteer (
    Roll CHAR(9) NOT NULL,
    EventID VARCHAR(10) NOT NULL,
    PRIMARY KEY (Roll, EventID),
    FOREIGN KEY (EventID) REFERENCES Event(EID) ON DELETE CASCADE,
    FOREIGN KEY (Roll) REFERENCES Student(Roll) ON DELETE CASCADE
);

/* Participant table */
CREATE TABLE Participant (
    PID BIGSERIAL PRIMARY KEY NOT NULL UNIQUE,
    Name VARCHAR(50) NOT NULL,
    College_Name VARCHAR(100) NOT NULL,
    EventID VARCHAR(10) NOT NULL,
    FOREIGN KEY (College_Name) REFERENCES College(Name) ON DELETE CASCADE,
    FOREIGN KEY (EventID) REFERENCES Event(EID) ON DELETE CASCADE
);

/* Manages table */
CREATE TABLE Manages (
    Roll CHAR(9) NOT NULL,
    EventID VARCHAR(10) NOT NULL,
    PRIMARY KEY (Roll, EventID),
    FOREIGN KEY (Roll) REFERENCES Student(Roll) ON DELETE CASCADE,
    FOREIGN KEY (EventID) REFERENCES Event(EID) ON DELETE CASCADE
);

/* Inserting Roles */
INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'SEC001',
        'Secretary',
        'A student, as secretary, coordinates event logistics, communicates with participants, and ensures smooth execution by overseeing administrative details and schedules.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'SEC003',
        'Sub_Secretary',
        'A sub-secretary assists the main secretary, handling specific tasks, communication, and logistics to support the seamless management of an event.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'TR001',
        'Treasurer',
        'Manages financial aspects and budgeting for events.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'PRO001',
        'Public Relations Officer',
        'Handles communication and publicity for events.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'LC001',
        'Logistics Coordinator',
        'Organizes and manages logistical aspects of events.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'TC001',
        'Technical Coordinator',
        'Manages technical setup and requirements for events.'
    );

INSERT INTO
    Role (RID, Rname, Description)
VALUES
    (
        'LC002',
        'Operations Coordinator',
        'Manages overall operational aspects and ensures smooth execution of events.'
    );

/* Inserting Events */
INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'ME001',
        DATE '2024-01-25',
        'Megaevent',
        'Special'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'DJ001',
        '2024-01-28',
        'DJ Night',
        'Entertainment'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'ME003',
        '2024-02-01',
        'Super Event',
        'Special'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'CN001',
        '2024-03-15',
        'Cultural Night',
        'Cultural'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'SG001',
        '2024-03-20',
        'Sports Gala',
        'Sports'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'TE001',
        '2024-03-25',
        'Tech Expo',
        'Technical'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'FS001',
        '2024-04-01',
        'Fashion Show',
        'Cultural'
    );

INSERT INTO
    Event (EID, Date, Ename, Type)
VALUES
    (
        'LF001',
        '2024-04-10',
        'Literary Fest',
        'Literary'
    );

/* Inserting Colleges */
INSERT INTO
    College (Name, Location)
VALUES
    ('IITB', 'Mumbai');

INSERT INTO
    College (Name, Location)
VALUES
    ('IITD', 'Delhi');

INSERT INTO
    College (Name, Location)
VALUES
    ('IITK', 'Kanpur');

INSERT INTO
    College (Name, Location)
VALUES
    ('IITM', 'Madras');

INSERT INTO
    College (Name, Location)
VALUES
    ('IITKGP', 'Kharagpur');

INSERT INTO
    College (Name, Location)
VALUES
    ('IITR', 'Roorkee');

INSERT INTO
    College (Name, Location)
VALUES
    ('Jadavpur University', 'Kolkata');

INSERT INTO
    College (Name, Location)
VALUES
    ('Calcutta University', 'Kolkata');

INSERT INTO
    College (Name, Location)
VALUES
    ('National Medical College', 'Kolkata');

INSERT INTO
    College (Name, Location)
VALUES
    ('Presidency University', 'Kolkata');

/* Inserting Students and Manages */
INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Abhishek Kumar',
        '20CS10064',
        'CSE',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('20CS10064', 'ME001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Bhaskar Kumar',
        '20CS10065',
        'CSE',
        'SEC003'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('20CS10065', 'ME001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Bratin Ghosh',
        '20CS10063',
        'CSE',
        'TR001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('20CS10063', 'ME001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Somya Gupta',
        '21ME10034',
        'ME',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('21ME10034', 'ME001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Dhruv Lal',
        '21ME10035',
        'ME',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('21ME10035', 'ME003');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Tanya Mondal',
        '21CE10036',
        'CE',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('21CE10036', 'CN001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Rajesh Prasad',
        '21EE10036',
        'EE',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('21EE10036', 'SG001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Eshaan Gupta',
        '19ME10036',
        'ME',
        'PRO001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('19ME10036', 'DJ001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Rahul Sharma',
        '22CS10036',
        'CSE',
        'LC002'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('22CS10036', 'TE001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Vishal Agarwal',
        '22CS30021',
        'CSE',
        'PRO001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('22CS30021', 'LF001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Sakshi Dubey',
        '20CS10066',
        'CSE',
        'SEC003'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('20CS10066', 'DJ001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Amit Patel',
        '21EE10037',
        'EE',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('21EE10037', 'ME003');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Rahul Singh',
        '22CS10038',
        'CSE',
        'SEC003'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('22CS10038', 'CN001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Neha Sharma',
        '19ME10039',
        'ME',
        'SEC003'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('19ME10039', 'SG001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Pooja Mishra',
        '22CS30040',
        'CSE',
        'SEC001'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('22CS30040', 'TE001');

INSERT INTO
    Student (Name, Roll, Dept, RoleID)
VALUES
    (
        'Catholine Marie',
        '20CS10067',
        'CSE',
        'LC002'
    );

INSERT INTO
    Manages (Roll, EventID)
VALUES
    ('20CS10067', 'TE001');

/* Inserting Participants */
INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Faisal Khan',
        'IITB',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Rajesh Prasad',
        'IITB',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Sophie Martin',
        'IITB',
        'DJ001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Lucas Dubois',
        'IITB',
        'ME003'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Eva Rousseau',
        'IITB',
        'CN001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Antoine Bernard',
        'IITB',
        'SG001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Isabelle Leroux',
        'IITB',
        'TE001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Claude Laurent',
        'IITB',
        'FS001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Amélie Dubois',
        'IITB',
        'LF001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Rahul Sharma',
        'IITB',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Manish Das',
        'IITD',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Nitish Kumar',
        'IITD',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Chirag Paswan',
        'IITK',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Gaurav Lal',
        'Jadavpur University',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Amrita Dubey',
        'Calcutta University',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Monika Roy',
        'National Medical College',
        'ME001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Antoine Martin',
        'IITD',
        'DJ001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Camille Dubois',
        'IITK',
        'ME003'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Elise Rousseau',
        'IITM',
        'CN001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Lucas Lambert',
        'IITKGP',
        'SG001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Isabelle Lefevre',
        'IITR',
        'TE001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Maximilian Becker',
        'Jadavpur University',
        'FS001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Hannah Müller',
        'Calcutta University',
        'LF001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Lukas Schmidt',
        'National Medical College',
        'CN001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Sophie Wagner',
        'Presidency University',
        'TE001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Felix Weber',
        'IITR',
        'DJ001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Ivan Ivanov',
        'IITM',
        'SG001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Anastasia Sokolova',
        'IITB',
        'LF001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Dmitri Petrov',
        'IITK',
        'FS001'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Ekaterina Ivanova',
        'IITKGP',
        'ME003'
    );

INSERT INTO
    Participant (Name, College_Name, EventID)
VALUES
    (
        'Natalia Volkova',
        'IITD',
        'CN001'
    );

/* Inserting Volunteers */
INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('20CS10064', 'ME001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('20CS10065', 'ME001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('20CS10063', 'ME001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('22CS10036', 'ME003');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('22CS30021', 'SG001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('21ME10034', 'CN001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('21ME10035', 'FS001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('20CS10066', 'DJ001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('21EE10037', 'ME003');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('22CS10038', 'CN001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('19ME10039', 'SG001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('22CS30040', 'TE001');

INSERT INTO
    Volunteer (Roll, EventID)
VALUES
    ('20CS10067', 'ME001');

/* Queries */
/* 
 Q1.
 Roll number and name of all the students who are managing the “Megaevent”
 */
-- Approach: Use INNER JOIN to link student and event tables based on the event ID and filter by event name.
SELECT
    student.roll,
    student.name
FROM
    student
    JOIN manages ON student.roll = manages.roll
    JOIN event ON manages.eventid = event.eid
WHERE
    event.ename = 'Megaevent';

/*
 Q2.
 Roll number and name of all the students who are managing “Megaevent” as a “Secretary”
 */
-- Approach: Use INNER JOIN to connect student and role tables on the role ID, filter for 'Secretary', and then join with the event table based on the event ID.
SELECT
    secretary_student.roll,
    secretary_student.name
FROM
    (
        SELECT
            name,
            roll
        FROM
            student
            JOIN role ON student.roleid = role.rid
        WHERE
            role.rname = 'Secretary'
    ) AS secretary_student
    JOIN manages ON secretary_student.roll = manages.roll
    JOIN event ON manages.eventid = event.eid
WHERE
    ename = 'Megaevent';

/* 
 Q3.
 Name of all the participants from the college “IITB” in “Megaevent”
 */
-- Approach: Join participant and college tables based on college name, filter for 'IITB', and then join with the event table based on the event ID.
SELECT
    name
FROM
    (
        SELECT
            participant.name,
            eventid
        FROM
            participant
            JOIN college ON participant.college_name = college.name
        WHERE
            college.name = 'IITB'
    ) AS participant_IITB
    JOIN event ON participant_IITB.eventid = event.eid
WHERE
    ename = 'Megaevent';

/*
 Q4. 
 Name of all the colleges who have at least one participant in “Megaevent”
 */
-- Approach: Use INNER JOIN to link participant and event tables based on the event ID, then retrieve distinct college names.
SELECT
    DISTINCT college_name
FROM
    participant
    JOIN event ON participant.eventid = event.eid
WHERE
    event.ename = 'Megaevent';

/* 
 Q5.
 Name of all the events which are managed by a “Secretary”
 */
-- Approach: Use INNER JOIN to connect student and role tables based on the role ID, filter for 'Secretary', and then join with the event table based on the event ID.
SELECT
    DISTINCT ename
FROM
    (
        SELECT
            roll
        FROM
            student
            JOIN role ON student.roleid = role.rid
        WHERE
            role.rname = 'Secretary'
    ) AS event_secretary
    JOIN manages ON event_secretary.roll = manages.roll
    JOIN event ON manages.eventid = event.eid;

/* 
 Q6.
 Name of all the “CSE” department student volunteers of “Megaevent”
 */
-- Approach: Join the student and volunteer tables based on the common event ID. Select students from the "CSE" department who are also volunteers for the specified event.
SELECT
    name
FROM
    student
    JOIN (
        SELECT
            roll
        FROM
            volunteer
            JOIN event ON volunteer.eventid = event.eid
        WHERE
            event.ename = 'Megaevent'
    ) AS mega_student ON student.roll = mega_student.roll
WHERE
    student.dept = 'CSE';

/*
 Q7.
 Name of all the events which have at least one volunteer from “CSE”
 */
-- Approach: Join the student and volunteer tables based on the common event ID. Retrieve distinct event names where students from the "CSE" department are volunteers.
SELECT
    DISTINCT ename
FROM
    student
    JOIN (
        SELECT
            roll,
            ename
        FROM
            volunteer
            JOIN event ON volunteer.eventid = event.eid
    ) AS student_volunteer ON student.roll = student_volunteer.roll
WHERE
    student.dept = 'CSE';

/*
 Q8.
 Name of the college with the largest number of participants in “Megaevent”
 */
-- Approach: Join the participant and event tables based on the event ID. Count the participants for each college, order by participant count in descending order, and limit the result to 1 to get the college with the largest number of participants.
SELECT
    college_name
FROM
    (
        SELECT
            participant.college_name,
            COUNT(*) AS participant_count
        FROM
            participant
            JOIN event ON participant.eventid = event.eid
        WHERE
            event.ename = 'Megaevent'
        GROUP BY
            participant.college_name
        ORDER BY
            participant_count DESC
        LIMIT
            1
    ) AS mega_college_count;

/* Alternative solution for Question 8 */
/*
 CREATE TEMPORARY TABLE participant_count AS
 SELECT
 p.college_name,
 COUNT(p.PID) AS p_count
 FROM
 participant p
 JOIN event e ON p.eventid = e.eid
 WHERE
 e.ename = 'Megaevent'
 GROUP BY
 p.college_name;
 
 SELECT
 pc.college_name
 FROM
 participant_count pc
 LEFT JOIN participant_count A ON pc.p_count < A.p_count
 WHERE
 A.college_name IS NULL;
 */
/* 
 Q9.
 Name of the college with the largest number of participants in any event
 */
-- Approach: Join the participant and event tables based on the event ID. Count the participants for each college, order by participant count in descending order, and limit the result to 1 to get the college with the largest number of participants across all events.
SELECT
    college_name
FROM
    (
        SELECT
            participant.college_name,
            COUNT(*) AS participant_count
        FROM
            participant
            JOIN event ON participant.eventid = event.eid
        GROUP BY
            participant.college_name
        ORDER BY
            participant_count DESC
        LIMIT
            1
    ) AS mega_college_count;

/* Alternative solution for Question 9 */
/*
 CREATE TEMPORARY TABLE participant_count AS
 SELECT
 participant.college_name,
 COUNT(participant.PID) AS p_count
 FROM
 participant
 JOIN event ON participant.eventid = event.eid
 GROUP BY
 participant.college_name;
 
 SELECT
 pc.college_name
 FROM
 participant_count pc
 LEFT JOIN participant_count A ON pc.p_count < A.p_count
 WHERE
 A.college_name IS NULL;
 */
/*
 Q10.
 Name of the department with the largest number of volunteers in all the events which have at least one participant from “IITB”
 */
-- Approach: Identify events with at least one participant from "IITB." Join with the volunteer table to find volunteers for those events. Count the volunteers for each department, order by volunteer count in descending order, and limit the result to 1 to get the department with the largest number of volunteers.
SELECT
    dept
FROM
    (
        SELECT
            dept,
            COUNT(*) AS student_count
        FROM
            (
                SELECT
                    roll
                FROM
                    (
                        SELECT
                            DISTINCT eid
                        FROM
                            participant
                            JOIN event ON participant.eventid = event.eid
                        WHERE
                            participant.college_name = 'IITB'
                    ) AS IITB_event
                    JOIN volunteer ON IITB_event.eid = volunteer.eventid
            ) AS volunteer_IITB
            JOIN student ON volunteer_IITB.roll = student.roll
        GROUP BY
            dept
        ORDER BY
            student_count DESC
        LIMIT
            1
    ) AS mega_dept_count;

/* Alternative solution for Question 10 */
/*
 CREATE TEMPORARY TABLE IITB_event AS
 SELECT
 DISTINCT event.eid
 FROM
 participant
 JOIN event ON participant.eventid = event.eid
 WHERE
 participant.college_name = 'IITB';
 
 CREATE TEMPORARY TABLE volunteer_IITB AS
 SELECT
 DISTINCT volunteer.roll
 FROM
 IITB_event
 JOIN volunteer ON IITB_event.eid = volunteer.eventid;
 
 CREATE TEMPORARY TABLE student_count AS
 SELECT
 student.dept,
 COUNT(student.roll) AS r_count
 FROM
 volunteer_IITB
 JOIN student ON volunteer_IITB.roll = student.roll
 GROUP BY
 student.dept;
 
 SELECT
 s.dept
 FROM
 student_count s
 LEFT JOIN student_count A ON s.r_count < A.r_count
 WHERE
 A.dept IS NULL;
 */
/* Clean up */
/* Drop the tables */
-- DROP TABLE IF EXISTS Volunteer CASCADE;
-- DROP TABLE IF EXISTS Manages CASCADE;
-- DROP TABLE IF EXISTS Participant CASCADE;
-- DROP TABLE IF EXISTS Student CASCADE;
-- DROP TABLE IF EXISTS College CASCADE;
-- DROP TABLE IF EXISTS Event CASCADE;
-- DROP TABLE IF EXISTS Role CASCADE;