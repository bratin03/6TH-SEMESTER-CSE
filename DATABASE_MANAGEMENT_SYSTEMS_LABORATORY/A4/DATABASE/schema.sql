DROP TABLE IF EXISTS volunteers;

DROP TABLE IF EXISTS manages;

DROP TABLE IF EXISTS winners;

DROP TABLE IF EXISTS participation;

DROP TABLE IF EXISTS accomodated_at;

DROP TABLE IF EXISTS event_logistics;

DROP TABLE IF EXISTS event_logistics_item;

DROP TABLE IF EXISTS accomodation;

DROP TABLE IF EXISTS admin;

DROP TABLE IF EXISTS organisers;

DROP TABLE IF EXISTS student;

DROP TABLE IF EXISTS event;

-- Table 1
-- Event entities will be created by the Admin user
CREATE TABLE event (
    id varchar(100) PRIMARY KEY NOT NULL,
    -- to be generated by the application
    name varchar(100) NOT NULL UNIQUE,
    type varchar(100) NOT NULL CHECK (
        type IN (
            'cultural',
            'competition',
            'workshop',
            'talk',
            'other'
        )
    ),
    -- Five types of events are allowed
    -- Competition type events will have prize money
    -- Others will have set it to NULL
    info varchar(1000) NOT NULL,
    start_date_time timestamp NOT NULL,
    end_date_time timestamp NOT NULL,
    location varchar(100) NOT NULL,
    first_prize INTEGER CHECK (first_prize > 0),
    second_prize INTEGER CHECK (
        second_prize > 0
        AND second_prize < first_prize
    ),
    third_prize INTEGER CHECK (
        third_prize > 0
        AND third_prize < second_prize
    ),
    created_at timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP CONSTRAINT start_end CHECK (start_date_time < end_date_time),
    CONSTRAINT prizes_null_constraint CHECK (
        (
            type = 'competition'
            AND first_prize IS NOT NULL
            AND second_prize IS NOT NULL
            AND third_prize IS NOT NULL
        )
        OR (
            first_prize IS NULL
            AND second_prize IS NULL
            AND third_prize IS NULL
        )
    )
);

-- Table 2
-- Student entities will have to sign up and then they can log in
CREATE TABLE student(
    sid varchar(100) PRIMARY KEY NOT NULL,
    email varchar(100) NOT NULL UNIQUE,
    name varchar(100) NOT NULL,
    roll_number varchar(20) UNIQUE,
    -- Extenral students need not enter their roll number
    phone varchar(20) NOT NULL,
    college varchar(100) NOT NULL,
    department varchar(100) NOT NULL,
    year INTEGER NOT NULL CHECK (
        year > 0
        AND year <= 5
    ),
    type varchar(100) NOT NULL CHECK (
        type IN (
            'internal',
            'external'
        )
    ),
    password varchar(100) NOT NULL,
    -- password to be stored only after encryption
    CONSTRAINT valid_email CHECK (
        email ~* '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$'
    ),
    CONSTRAINT phone CHECK (length(phone) = 10)
);

-- Table 3
-- Organiser entities will have to sign up and then they can log in
CREATE TABLE organisers (
    oid varchar(100) PRIMARY KEY NOT NULL,
    email varchar(100) NOT NULL UNIQUE,
    name varchar(100) NOT NULL,
    phone varchar(20) NOT NULL UNIQUE,
    password varchar(100) NOT NULL,
    -- password to be stored only after encryption,
    CONSTRAINT valid_email CHECK (
        email ~* '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$'
    ),
    CONSTRAINT phone CHECK (length(phone) = 10)
);

-- Table 4
-- Admin will not have sign up option
-- Admin will be manually entried in the admin table by the database admin
-- Admin then can log in
CREATE TABLE admin(
    id varchar(100) PRIMARY KEY NOT NULL,
    name varchar(100) NOT NULL,
    email varchar(100) NOT NULL UNIQUE,
    password varchar(100) NOT NULL,
    -- password to be stored only after encryption
    CONSTRAINT valid_email CHECK (
        email ~* '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$'
    )
);

-- Table 5
-- The table accomodation will include the accomodation options available to the participant
-- The entries in the table can either be entered by the Admin or can be hardcoded
CREATE TABLE accomodation(
    id varchar(100) PRIMARY KEY NOT NULL,
    location varchar(100) NOT NULL,
    check_in date NOT NULL,
    check_out date NOT NULL,
    food_type varchar(100) NOT NULL CHECK (
        food_type IN (
            'veg',
            'non-veg',
        )
    ),
    cost INTEGER NOT NULL CHECK (cost > 0),
    CONSTRAINT check_in_out CHECK (check_in < check_out)
);

-- Table 6
-- The table accomodated_at will contain the entries of the students who have been accomodated
CREATE TABLE accomodated_at(
    participant_id varchar(100) NOT NULL,
    logistics_id varchar(100) NOT NULL,
    payment_status varchar(100) DEFAULT 'paid' NOT NULL CHECK (
        payment_status IN (
            'paid',
            'pending'
        )
    ),
    PRIMARY KEY (participant_id, logistics_id),
    FOREIGN KEY (participant_id) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (logistics_id) REFERENCES accomodation(id) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Table 7
-- The table participation will contain the entries of the students who have participated in the events
CREATE TABLE participation(
    event_id varchar(100) NOT NULL,
    student_id varchar(100) NOT NULL,
    PRIMARY KEY (event_id, student_id),
    FOREIGN KEY (event_id) REFERENCES event(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (student_id) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Table 8
-- The winner table will contain the entries of the students who have won the events
CREATE TABLE winners(
    event_id varchar(100) NOT NULL,
    first_p varchar(100) NOT NULL,
    second_p varchar(100) NOT NULL,
    third_p varchar(100) NOT NULL,
    PRIMARY KEY (event_id),
    FOREIGN KEY (event_id) REFERENCES event(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (first_p) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (second_p) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (third_p) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT first_second_third CHECK (
        first_p <> second_p
        AND first_p <> third_p
        AND second_p <> third_p
    )
);

-- Table 9
-- Manages table will contain the entries the entries of organisers who are managing the events
CREATE TABLE manages(
    event_id varchar(100) NOT NULL,
    organiser_id varchar(100) NOT NULL,
    sponshorship_amount INTEGER NOT NULL CHECK (sponshorship_amount > 0),
    payment_status varchar(100) DEFAULT 'pending' NOT NULL CHECK (
        payment_status IN (
            'paid',
            'pending'
        )
    ),
    request_status varchar(100) DEFAULT 'pending' NOT NULL CHECK (
        request_status IN (
            'approved',
            'pending',
            'rejected'
        )
    ),
    PRIMARY KEY (event_id, organiser_id),
    FOREIGN KEY (event_id) REFERENCES event(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (organiser_id) REFERENCES organisers(oid) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Table 10
-- Volunteers table will contain the entries of the students who are volunteering for the events
CREATE TABLE volunteers(
    event_id varchar(100) NOT NULL,
    student_id varchar(100) NOT NULL,
    info varchar(1000) NOT NULL,
    role varchar(100) NOT NULL,
    PRIMARY KEY (event_id, student_id),
    FOREIGN KEY (event_id) REFERENCES event(id) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (student_id) REFERENCES student(sid) ON DELETE CASCADE ON UPDATE CASCADE
);