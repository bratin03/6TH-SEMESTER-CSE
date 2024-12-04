DROP FUNCTION IF EXISTS get_sponsored_events;
DROP FUNCTION IF EXISTS get_participated_events;
DROP FUNCTION IF EXISTS get_winning_events;
DROP FUNCTION IF EXISTS get_event_volunteers;


CREATE OR REPLACE FUNCTION get_sponsored_events(organiser_id_input char(20))
RETURNS TABLE (
    event_id varchar(100),
    event_name varchar(100),
    event_type varchar(100),
    event_start timestamp,
    event_end timestamp,
    event_location varchar(100),
    sponsorship_amount integer,
    payment_status varchar(100),
    request_status varchar(100)
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        e.id AS event_id,
        e.name AS event_name,
        e.type AS event_type,
        e.start_date_time AS event_start,
        e.end_date_time AS event_end,
        e.location AS event_location,
        m.sponshorship_amount AS sponsorship_amount,
        m.payment_status AS payment_status,
        m.request_status AS request_status
    FROM 
        event e
    INNER JOIN 
        manages m ON e.id = m.event_id
    WHERE 
        m.organiser_id = organiser_id_input;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_participated_events(student_id_input char(20))
RETURNS TABLE (
    event_id varchar(100),
    event_name varchar(100),
    event_type varchar(100),
    event_start timestamp,
    event_end timestamp,
    event_location varchar(100),
    position_secured varchar(100)
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        e.id AS event_id,
        e.name AS event_name,
        e.type AS event_type,
        e.start_date_time AS event_start,
        e.end_date_time AS event_end,
        e.location AS event_location,
        p.position_secured AS position_secured
    FROM 
        event e
    INNER JOIN 
        participation p ON e.id = p.event_id
    WHERE 
        p.student_id = student_id_input;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_winning_events(student_name_input varchar(100))
RETURNS TABLE (
    event_id varchar(100),
    event_name varchar(100),
    position_secured varchar(100)
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        w.event_id,
        e.name AS event_name,
        CASE
            WHEN w.first_p = s.sid THEN 'first'::varchar(100)
            WHEN w.second_p = s.sid THEN 'second'::varchar(100)
            WHEN w.third_p = s.sid THEN 'third'::varchar(100)
            ELSE 'participant'::varchar(100)
        END AS position_secured
    FROM 
        winners w
    JOIN 
        event e ON w.event_id = e.id
    JOIN 
        student s ON s.sid = ANY (ARRAY[w.first_p, w.second_p, w.third_p])
    WHERE 
        s.name = student_name_input;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_event_volunteers(event_id_input varchar(100))
RETURNS TABLE (
    student_id char(20),
    student_name varchar(100),
    info varchar(1000),
    role varchar(100)
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        v.student_id,
        s.name AS student_name,
        v.info,
        v.role
    FROM 
        volunteers v
    JOIN 
        student s ON v.student_id = s.sid
    WHERE 
        v.event_id = event_id_input;
END;
$$ LANGUAGE plpgsql;